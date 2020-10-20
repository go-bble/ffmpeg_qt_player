#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H

#include <QThread>
class XDecode;
struct AVPacket;
#include <QMutex>
#include <QList>
class XDecodeThread : public QThread
{
public:
    XDecodeThread();
    virtual ~XDecodeThread();
    virtual void Push(AVPacket *pkt);
    virtual AVPacket *Pop();
    virtual void Clear();
    virtual void Close();


    int maxList = 100;
    bool isExit = false;
protected:
    XDecode *decode = nullptr;
    QList <AVPacket *> packs;
    QMutex dtmux;
};

#endif // XDECODETHREAD_H
