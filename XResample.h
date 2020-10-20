#ifndef XRESAMPLE_H
#define XRESAMPLE_H

struct AVCodecParameters;
struct AVFrame;
struct SwrContext;
#include<QMutex>
class XResample
{
public:
    XResample();
    virtual bool Open(AVCodecParameters *para,bool isClearPara = false);
    virtual void Close();

    virtual int Resample(AVFrame *indata,unsigned char *data);
    ~XResample();
//   enum  	AVSampleFormat {
//    AV_SAMPLE_FMT_NONE = -1, AV_SAMPLE_FMT_U8, AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S32,
//    AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_U8P, AV_SAMPLE_FMT_S16P,
//    AV_SAMPLE_FMT_S32P, AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_DBLP, AV_SAMPLE_FMT_NB
//  }AV_SAMPLE_FMT_S16 = 1
    int outFormat = 1;

protected:
    QMutex mux;
    SwrContext *actx = 0;
};

#endif // XRESAMPLE_H
