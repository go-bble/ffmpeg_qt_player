#include "XVideoThread.h"
#include <QDebug>
XVideoThread::XVideoThread()
{

}

XVideoThread::~XVideoThread()
{
    isExit = true;
    wait();
}

bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height)
{
    if (!para)return false;
    Clear();
        vtmux.lock();

        this->call = call;
        if (call)
        {
            call->Init(width, height);
        }

        if (!decode) decode = new XDecode();
        int re = true;
        if (!decode->Open(para))
        {
            re = false;
        }
        vtmux.unlock();
        return re;
}


void XVideoThread:: run()
{
    while (!isExit)
    {
        vtmux.lock();
        if (this->isPause)
        {
            vtmux.unlock();
            msleep(5);
            continue;
        }
        if (packs.empty() || !decode)
        {
            vtmux.unlock();
            msleep(1);
            continue;
        }
        //同步
        if(synpts < decode->pts){
            vtmux.unlock();
            msleep(1);
            continue;
        }

        AVPacket *pkt = Pop();
        bool re = decode->Send(pkt);
        if (!re)
        {
            vtmux.unlock();
            msleep(1);
            continue;
        }
        while (!isExit) {
            AVFrame * frame = decode->Recv();
            if (!frame) break;
            //显示视频
            if (call)
            {
                call->Repaint(frame);
            }
        }
       vtmux.unlock();
    }
}

void XVideoThread::setPause(bool isPause)
{
    vtmux.lock();
    this->isPause = isPause;
    vtmux.unlock();

}


bool XVideoThread::RepaintPts(AVPacket *pkt, long long seekpts)
{
    vtmux.lock();
    bool re = decode->Send(pkt);
    if (!re)
    {
        vtmux.unlock();
        msleep(1);
        return true;
    }
    AVFrame *frame = decode->Recv();
    if (!frame)
    {
        vtmux.unlock();
        return false;
    }
    //到达位置
    if (decode->pts >= seekpts)
    {
        if(call)
            call->Repaint(frame);
        vtmux.unlock();
        return true;
    }
    XFreeFrame(&frame);
    vtmux.unlock();
    return false;
}
