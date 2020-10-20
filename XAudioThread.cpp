#include "XAudioThread.h"
#include "XAudioThread.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include <QDebug>
XAudioThread::XAudioThread()
{
    if (!res) res = new XResample();
    if (!ap) ap = XAudioPlay::Get();
}

bool XAudioThread::Open(AVCodecParameters *para, int sampleRate, int channels)
{
    if(!para)return false;
    Clear();
    atmux.lock();
    this->pts = 0;
    if(!decode)decode = new XDecode();
    if (!res) res = new XResample();
    if (!ap) ap = XAudioPlay::Get();

    bool re = true;
    if (!res->Open(para, false))
    {
        re = false;
    }
    ap->sampleRate = sampleRate;
    ap->channels = channels;
    if (!ap->Open())
    {
        re = false;
    }
    if (!decode->Open(para))
    {
        re = false;
    }
    atmux.unlock();
    return re;
}



void XAudioThread::run()
{
    unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit)
    {
        atmux.lock();
        if(packs.isEmpty() || decode == nullptr|| ap == nullptr|| res == nullptr)
        {
            atmux.unlock();
            msleep(1);
            continue;
        }
        if(isPause){
            atmux.unlock();
            msleep(5);
            continue;
        }

        AVPacket *pkt = Pop();
        int re = decode->Send(pkt);
        if (!re)
        {
            atmux.unlock();
            msleep(1);
            continue;
        }
        while (!isExit) {
            AVFrame * frame = decode->Recv();

            if (!frame) break;
            //单位毫秒
            pts =decode->pts - ap->GetNoPlayMs();
            int size = res->Resample(frame,pcm);
            while (!isExit) {
                if(size <=0)break;
                if(ap->GetFree() <size){
                    msleep(1);
                    continue;
                }
                ap->Write(pcm,size);
                break;
            }

        }
       atmux.unlock();
    }    
    delete[] pcm;
    pcm = nullptr;

}

XAudioThread::~XAudioThread()
{
    isExit = true;
    wait();
}

void XAudioThread::Clear()
{
    //重载了父类需要调用清理
    XDecodeThread::Clear();
    atmux.lock();
    if (ap) ap->Clear();
    atmux.unlock();
}

void XAudioThread::Close()
{
    //重载了父类需要调用清理
    XDecodeThread::Close();
    if (res)
    {
        res->Close();
        atmux.lock();
        delete res;
        res = nullptr;
        atmux.unlock();
    }
    if (ap)
    {
        ap->Close();
        atmux.lock();
        ap = nullptr;
        atmux.unlock();
    }
}

void XAudioThread::setPause(bool isPause)
{
    atmux.lock();
    this->isPause = isPause;
    ap->setPause(isPause);
    atmux.unlock();


}

void XAudioThread::setVol(double vol)
{
    atmux.lock();
    if(ap)ap->setVol(vol);
    atmux.unlock();
}
