#ifndef XVIDEOTHREAD_H
#define XVIDEOTHREAD_H
struct AVCodecParameters;
struct AVPacket;
#include "XDecode.h"
#include "IVideoCall.h"
#include <QMutex>
#include <QList>
#include "XDecodeThread.h"
class XVideoThread:public XDecodeThread
{
public:
    XVideoThread();
    virtual ~XVideoThread();
    /**
     * @brief Open
     * @param para 必须释放的参数
     */
    virtual bool Open(AVCodecParameters *para,IVideoCall *call,int width,int height);
    void run();
    //视频根据音频同步
    long long synpts = 0;
    virtual void setPause(bool isPause);
    bool isPause = false;
    //根据pts，返回解码数据
    virtual bool RepaintPts(AVPacket *pkt, long long seekpts);
protected:
    QMutex vtmux;
    IVideoCall *call = nullptr;
};

#endif // XVIDEOTHREAD_H
