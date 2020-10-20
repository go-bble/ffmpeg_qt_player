#ifndef XDEMUX_H
#define XDEMUX_H


struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
#include<QMutex>
class XDemux
{
public:
    XDemux();
    virtual ~XDemux();

    virtual bool Open(const char* url);//open file or url
    virtual AVPacket *Read();//需要手动清理，AVpacket 数据空间
    virtual AVPacket *ReadVideo();
    virtual AVCodecParameters *CopyVPara(); //vidoe
    virtual AVCodecParameters *CopyAPara(); //audio
    virtual void Clear();
    virtual void Close();
    virtual bool IsAudio(AVPacket *pkt);
    virtual bool Seek(double pos);

    long long totalMs =0;//duration
    int width = 0;
    int height = 0;
    int sampleRate = 0;
    int channels = 0;

protected:
    QMutex mux;
    AVFormatContext *ic = NULL;
    int videoStream = 0;
    int audioStream = 1;
};

#endif // XDEMUX_H
