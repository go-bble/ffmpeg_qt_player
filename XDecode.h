#ifndef XDECODE_H
#define XDECODE_H

struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
#include <QMutex>
#include <QMutexLocker>
//(!-!)
extern void XFreePacket(AVPacket **pkt);
extern void XFreeFrame(AVFrame **frame);
class XDecode
{
public:
    XDecode();
    bool isAudio =false;
    long long pts = 0;//recv会给pts赋值
    virtual bool Open(AVCodecParameters *para);
    virtual bool Send(AVPacket *pkt);
    virtual AVFrame* Recv();
    virtual void Clear();
    virtual void Close();
    virtual ~XDecode();
protected:
    QMutex mux;
    AVCodecContext *codec = 0;
};

#endif // XDECODE_H
