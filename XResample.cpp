#include "XResample.h"
extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}
#pragma comment(lib,"swresample.lib")
#include <QDebug>
XResample::XResample()
{

}
XResample::~XResample()
{

}
/**
 * @brief XResample::Open
 * @param para
 * @param isClearPara
 * @return
 */
bool XResample::Open(AVCodecParameters *para, bool isClearPara)
{
    if(!para){
        return false;
    }
    //(bo) close测试
    Close();

    mux.lock();
    //如果actx为NULL会分配空间
    actx = swr_alloc_set_opts(actx,
        av_get_default_channel_layout(para->channels),	//输出格式
        (AVSampleFormat)outFormat,			//输出样本格式 1 AV_SAMPLE_FMT_S16
        para->sample_rate,					//输出采样率
        av_get_default_channel_layout(para->channels),//输入格式
        (AVSampleFormat)para->format,
        para->sample_rate,
        0, 0
    );
    if(isClearPara)
            avcodec_parameters_free(&para);
    //设置用户参数后初始化上下文。
    int re = swr_init(actx);
    mux.unlock();
    if(re!=0){
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        qDebug() << "swr_init  failed! :" << buf ;
        return false;
    }
    return true;
}
int XResample::Resample(AVFrame *indata, unsigned char *data)
{
    if (!indata) return 0;
        if (!data)
        {
            av_frame_free(&indata);
            return 0;
        }

        uint8_t *data_t[2] = { 0 };
            data_t[0] = data;
            int re = swr_convert(actx,
                data_t, indata->nb_samples,		//输出
                (const uint8_t**)indata->data, indata->nb_samples	//输入
            );
            int outSize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
            av_frame_free(&indata);
            if (re <= 0)return re;

            return outSize;
}
void XResample::Close()
{
    mux.lock();

    if (actx)
        swr_free(&actx);

    mux.unlock();
}




