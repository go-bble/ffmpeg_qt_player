#include "XDecodeThread.h"
#include "XDecode.h"
#include <QDebug>
XDecodeThread::XDecodeThread()
{
    if(!decode)decode = new XDecode;
}

XDecodeThread::~XDecodeThread()
{
    isExit = true;
    wait();
}

void XDecodeThread::Push(AVPacket *pkt)
{
    if(!pkt)return;
    while (!isExit)
    {
        dtmux.lock();
        if(packs.size()<maxList)
        {
            packs.push_back(pkt);
            dtmux.unlock();
            break;
        }
          dtmux.unlock();
          msleep(1);

    }
}
AVPacket *XDecodeThread::Pop()
{
    dtmux.lock();
    if (packs.empty())
    {
        dtmux.unlock();
        return nullptr;
    }
    AVPacket *pkt = packs.front();
    packs.pop_front();
    dtmux.unlock();
    return pkt;
}
void XDecodeThread::Clear()
{
    dtmux.lock();
    decode->Clear();
    while (!packs.empty())
    {
        AVPacket *pkt = packs.front();
        XFreePacket(&pkt);
        packs.pop_front();
    }

    dtmux.unlock();
}

void XDecodeThread::Close()
{
    Clear();
    isExit = true;
    wait();
    decode->Close();

    dtmux.lock();
    delete decode;
    decode = nullptr;
    dtmux.unlock();
}


