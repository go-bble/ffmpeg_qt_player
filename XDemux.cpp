#include "XDemux.h"

#include <QDebug>
extern "C"{
    #include"libavformat/avformat.h"

}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
XDemux::XDemux()
{
    static bool isFirst = true;
    static QMutex dmux;
    dmux.lock();
    if(isFirst){
//        av_q2d();
        av_register_all();
        avformat_network_init();
        isFirst = false;
    }
    dmux.unlock();
}

XDemux::~XDemux()
{

}

bool XDemux::Open(const char *url)
{
   Close();

    //set parameters
    AVDictionary *opts = nullptr;

    av_dict_set(&opts,"rtsp_transport" ,"tcp",0);
    av_dict_set(&opts,"max_delay" ,"500",0);
    mux.lock();
    int re = avformat_open_input(
                &ic,
                url,
                0,//auto decoder
                &opts //import opts
                );
    if (re != 0)
    {
        mux.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        qDebug() << "open " << url << " failed! :" << buf << endl;
        return false;
    }

    re = avformat_find_stream_info(ic,0);

    this->totalMs = ic->duration/(AV_TIME_BASE/1000);
    //打印
    av_dump_format(ic, 0, url, 0);

    //获取视频流
    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream *as = ic->streams[videoStream];
    width = as->codecpar->width;
    height = as->codecpar->height;

    qDebug() << "=======================================================" << endl;
    qDebug() << videoStream << "视频信息" << endl;
    qDebug() << "codec_id = " << as->codecpar->codec_id << endl;
    qDebug() << "format = " << as->codecpar->format << endl;
    qDebug() << "width=" << as->codecpar->width << endl;
    qDebug() << "height=" << as->codecpar->height << endl;
    //帧率 fps 分数转换
    //qDebug() << "video fps = " << av_q2d(as->avg_frame_rate) << endl;

    qDebug() << "=======================================================" << endl;
    qDebug() << audioStream << "音频信息" << endl;
    //获取音频流
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if(audioStream<0){//(bo)
        mux.unlock();
        return false;
    }
    as = ic->streams[audioStream];
    sampleRate = as->codecpar->sample_rate;
    channels = as->codecpar->channels;

    qDebug() << "codec_id = " << as->codecpar->codec_id << endl;
    qDebug() << "format = " << as->codecpar->format << endl;
    qDebug() << "sample_rate = " << as->codecpar->sample_rate << endl;
    //AVSampleFormat;
    qDebug() << "channels = " << as->codecpar->channels << endl;
    //一帧数据？？ 单通道样本数
    qDebug() << "frame_size = " << as->codecpar->frame_size << endl;
    //1024 * 2 * 2 = 4096  fps = sample_rate/frame_size
    mux.unlock();


    return true;
}

AVPacket *XDemux::Read()
{
    mux.lock();
    if(!ic){
        mux.unlock();
        return 0;
    }
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(ic,pkt);
    if (re != 0)
        {
            mux.unlock();
            av_packet_free(&pkt);
            return 0;
        }
    //pts转换为毫秒
    pkt->pts = pkt->pts*(1000 * (av_q2d(ic->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts*(1000 * (av_q2d(ic->streams[pkt->stream_index]->time_base)));
    mux.unlock();
    return pkt;
}

AVPacket *XDemux::ReadVideo()
{
    mux.lock();
        if (!ic) //容错
        {
            mux.unlock();
            return 0;
        }
        mux.unlock();

        AVPacket *pkt = NULL;
        //防止阻塞
        for (int i = 0; i < 200; i++)
        {
            pkt = Read();
            if (!pkt)break;
            if (pkt->stream_index == videoStream)
            {
                break;
            }
            av_packet_free(&pkt);
        }
        return pkt;
}

/**
 * @brief XDemux::copyVPara avcodec_parameters_free手动释放空间
 * @return
 */
AVCodecParameters *XDemux::CopyVPara()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
    mux.unlock();
    return pa;
}

AVCodecParameters *XDemux::CopyAPara()
{
    mux.lock();
        if (!ic)
        {
            mux.unlock();
            return NULL;
        }
        AVCodecParameters *pa = avcodec_parameters_alloc();
        avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
        mux.unlock();
        return pa;
}

void XDemux::Clear()
{
    mux.lock();
        if (!ic)
        {
            mux.unlock();
            return ;
        }
        //清理读取缓冲
        avformat_flush(ic);
        mux.unlock();
}

void XDemux::Close()
{
    mux.lock();
        if (!ic)
        {
            mux.unlock();
            return;
        }
        avformat_close_input(&ic);
        //媒体总时长（毫秒）
        totalMs = 0;
        mux.unlock();
}

bool XDemux::IsAudio(AVPacket *pkt)
{
    if (!pkt) return false;
        if (pkt->stream_index == videoStream)
            return false;
        return true;
}

bool XDemux::Seek(double pos)
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return false;
    }
    //清理读取缓冲,不清理可能出现粘包网络流
    avformat_flush(ic);

    long long seekPos = 0;
    seekPos = ic->streams[videoStream]->duration * pos;
    int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();
    if (re < 0) return false;
    return true;
}
