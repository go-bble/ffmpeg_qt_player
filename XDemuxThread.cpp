#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <QDebug>
bool XDemuxThread::Open(const char *url, IVideoCall *call)
{
    if (url == 0 || url[0] == '\0')
            return false;
    mux.lock();
    if (!demux) demux = new XDemux();
    if (!vt) vt = new XVideoThread();
    if (!at) at = new XAudioThread();


    bool re = demux->Open(url);
    if (!re)
    {
        return false;
    }
    //打开视频解码器和处理线程
    if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
    {
        re = false;
    }
    //打开音频解码器和处理线程
    if (!at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels))
    {
        re = false;
    }
    this->totalMs = demux->totalMs;
    mux.unlock();
    return re;
}

void XDemuxThread::Start()
{
    mux.lock();
    if (!demux) demux = new XDemux();
    if (!vt) vt = new XVideoThread();
    if (!at) at = new XAudioThread();
    QThread::start();
    if (vt)vt->start();
    if (at)at->start();
    mux.unlock();
}

void XDemuxThread::Close()
{
    isExit = true;
    wait();
    if (vt) vt->Close();
    if (at) at->Close();
    mux.lock();
    delete vt;
    delete at;
    vt = nullptr;
    at = nullptr;
    mux.unlock();
}
void XDemuxThread::Clear()
{
    mux.lock();
    if (demux)demux->Clear();
    if (vt) vt->Clear();
    if (at) at->Clear();
    mux.unlock();
}

void XDemuxThread::Seek(double pos)
{
    mux.lock();
    bool status = this->isPause;
    mux.unlock();
    this->setPause(true);
    Clear();   
    mux.lock();

    if (demux)demux->Seek(pos);
    long long seekPts = pos*demux->totalMs;
    while (!isExit)
    {
        AVPacket *pkt = demux->ReadVideo();
        if(!pkt)break;
        if(vt->RepaintPts(pkt,seekPts))
        {
            this->pts = seekPts;
            break;
        }
    }
    mux.unlock();
    if(!status)
        this->setPause(false);

}

void XDemuxThread::run()
{
    while (!isExit)
    {
        mux.lock();
        //
        if(isPause){
            mux.unlock();
            msleep(5);
            continue;
        }
        if (!demux)
        {
            mux.unlock();
            msleep(5);
            continue;
        }
        if(at && vt){
            this->pts = at->pts;
            vt->synpts = at->pts;
        }
        AVPacket *pkt = demux->Read();
        if (!pkt)
        {
            mux.unlock();
            msleep(5);
            continue;
        }

        //判断数据是音频
        if (demux->IsAudio(pkt))
        {
            if(at)at->Push(pkt);
        }
        else //视频
        {
            if (vt)vt->Push(pkt);
        }
        mux.unlock();

    }
}

XDemuxThread:: XDemuxThread()
{

}

XDemuxThread::~XDemuxThread()
{
    isExit = true;
    wait();
}

void XDemuxThread::setPause(bool isPause)
{
    mux.lock();
    this->isPause = isPause;
    if(at)at->setPause(isPause);
    if(vt)vt->setPause(isPause);
    mux.unlock();
}

void XDemuxThread::setVol(double vol)
{   mux.lock();
    if(at)at->setVol(vol);
    mux.unlock();
}
