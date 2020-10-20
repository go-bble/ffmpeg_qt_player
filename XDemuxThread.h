#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H


#include <QThread>
#include "IVideoCall.h"
#include <QMutex>
class XDemux;
class XVideoThread;
class XAudioThread;
class XDemuxThread:public QThread
{
public:
    //创建对象并打开
    virtual bool Open(const char *url, IVideoCall *call);

    //启动所有线程
    virtual void Start();
    virtual void Close();
    virtual void Clear();
    virtual void Seek(double pos);
    void run();
    XDemuxThread();
    virtual ~XDemuxThread();
    bool isExit = false;

    bool isPause = false;
    //用音频pts，//同步时赋值
    long long pts = 0;
    //用demux的duration，open时赋值
    long long totalMs = 0;
    //1.停止demux->Read();2.停止音频缓冲
    void setPause(bool isPause);
    void setVol(double vol);
protected:
    QMutex mux;
    XDemux *demux = nullptr;
    XVideoThread *vt = nullptr;
    XAudioThread *at = nullptr;
};

#endif // XDEMUXTHREAD_H
