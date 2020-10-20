#include "XDecode.h"
extern "C"{
    #include "libavcodec/avcodec.h"
}
#include <QDebug>
//调用者不用导入avcodec头文件
void XFreePacket(AVPacket **pkt)
{
    if (!pkt || !(*pkt))return;
    av_packet_free(pkt);
}
void XFreeFrame(AVFrame **frame)
{
    if (!frame || !(*frame))return;
    av_frame_free(frame);
}

XDecode::XDecode()
{

}

bool XDecode::Open(AVCodecParameters *para)
{
    if(!para) return false;
    Close();
    //1.打开所需解码器
    AVCodec *vcodec = avcodec_find_decoder(para->codec_id);
    if(!vcodec){
        qDebug()<<"can't find the codec id" << para->codec_id;
    }
    //QMutexLocker vmlock(&vmutex);
    mux.lock();
    //2.为解码上下文分配空间,记得释放
    codec = avcodec_alloc_context3(vcodec);

    //3.配置上下文参数
    avcodec_parameters_to_context(codec,para);
    //用完释放AVCodecParameters
    avcodec_parameters_free(&para);

    //设置解码线程数
    codec->thread_count = 8;

    //4.打开解码器上下文
    int re = avcodec_open2(codec,0,0);
    if(re != 0){
        avcodec_free_context(&codec);
        mux.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        qDebug() << "avcodec_open2  failed! :" << buf << endl;
        return false;
    }
    mux.unlock();
    return true;
}

bool XDecode::Send(AVPacket *pkt)
{
    if (!pkt || pkt->size <= 0 || !pkt->data)return false;
    mux.lock();
    if(!codec){
        av_packet_free(&pkt);
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec,pkt);
    mux.unlock();
    av_packet_free(&pkt);
    if(re != 0){
        return false;
    }
    return true;
}
//调用者自己释放frame
AVFrame *XDecode::Recv()
{
    mux.lock();
    if(!codec){
        mux.unlock();
        return nullptr;
    }
    AVFrame *frame = av_frame_alloc();
    int re = avcodec_receive_frame(codec,frame);
    mux.unlock();
    if(re != 0){
        av_frame_free(&frame);
        return nullptr;
    }
    pts = frame->pts;
    return frame;
}

void XDecode::Clear()
{
    mux.lock();
    if(codec)
        avcodec_flush_buffers(codec);
    mux.unlock();
}

void XDecode::Close()
{
    mux.lock();
    if(codec){
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    this->pts = 0;
    mux.unlock();
}

XDecode::~XDecode()
{

}
