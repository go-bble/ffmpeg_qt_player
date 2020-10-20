#include "XAudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <QMutex>
#include <QDebug>
class CXAudioPlay :public XAudioPlay
{
public:
    QAudioOutput *output = nullptr;
    QIODevice *io = nullptr;
    QMutex mux;

    virtual long long GetNoPlayMs(){
        mux.lock();
        if(!output){
            mux.unlock();
            return 0;
        }
        long long size = output->bufferSize() - output->bytesFree();
        //一秒音频字节大小
        long long secSize = (sampleRate*sampleSize*channels)/8;
        long long pts = 0;
        if (secSize <= 0)
        {
            pts = 0;
        }
        else
        {
            pts = (size/secSize) * 1000;
        }
        mux.unlock();
        return pts;
    }
    virtual void Close()
    {
        mux.lock();
        if (io)
        {
            io->close ();
            io = nullptr;
        }
        if (output)
        {
            output->stop();
            delete output;
            output = nullptr;
        }
        mux.unlock();
    }
    virtual void Clear(){
        mux.lock();
        if (io)
        {
            io->reset();
        }
        mux.unlock();
    }
    virtual bool Open()
    {
        Close();
        QAudioFormat fmt;
        fmt.setSampleRate(sampleRate);
        fmt.setSampleSize(sampleSize);
        fmt.setChannelCount(channels);
        fmt.setCodec("audio/pcm");
        fmt.setByteOrder(QAudioFormat::LittleEndian);
        fmt.setSampleType(QAudioFormat::UnSignedInt);
        mux.lock();
        output = new QAudioOutput(fmt);
        io = output->start(); //开始播放
        mux.unlock();
        if(io)
            return true;
        return false;
    }
    virtual bool Write(const unsigned char *data, int datasize)
    {
        if (!data || datasize <= 0)return false;
        mux.lock();
        if (!output || !io)
        {
            mux.unlock();
            return false;
        }
        int size = io->write((char *)data, datasize);
        mux.unlock();
        if (datasize != size)
            return false;
        return true;
    }

    virtual int GetFree()
    {
        mux.lock();
        if (!output)
        {
            mux.unlock();
            return 0;
        }
        int free = output->bytesFree();
        mux.unlock();
        return free;
    }
    virtual void setPause(bool isPause) override{
        mux.lock();
        if(!output){
            mux.unlock();
            return;
        }

        if(isPause){
            output->suspend();
        }else {
            output->resume();
        }
        mux.unlock();
    }
    virtual void setVol(double vol)override{
        setPause(true);
        mux.lock();
        if(output)
           output->setVolume((qreal)vol);
        mux.unlock();
        setPause(false);
    }
};



XAudioPlay::XAudioPlay()
{

}

XAudioPlay *XAudioPlay::Get()
{
    static CXAudioPlay play;
    return &play;
}



XAudioPlay::~XAudioPlay()
{

}


