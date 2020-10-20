#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H


#include <QMutex>
#include <QList>
struct AVCodecParameters;
class XAudioPlay;
class XResample;
#include "XDecodeThread.h"
class XAudioThread:public XDecodeThread
{
public:
    XAudioThread();
    /**
     * @brief Open
     * @param para 会被清理
     */
    virtual bool Open(AVCodecParameters *para,int sampleRate,int channels);
    void run();

    virtual ~XAudioThread();
    //XAudioPlay 需要清理
    virtual void Clear();

    //清理XAudioPlay，停止线程
    virtual void Close();

    virtual void setPause(bool isPause);
    long long pts = 0;//在Open()中置零,run()中赋值
    virtual void setVol(double vol);
protected:
    QMutex atmux;
    XAudioPlay *ap = nullptr;
    XResample *res = nullptr;
    bool isPause = false;
};

#endif // XAUDIOTHREAD_H
