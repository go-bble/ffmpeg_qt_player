#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

static XDemuxThread xdt;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->hide();
//    tt.Init();
//    ui->openGLWidget->Init(tt.demux.width,tt.demux.height);
//    tt.video = ui->openGLWidget;
      xdt.Start();
      startTimer(40);
}
//void MainWindow::ttest()
//{
//    xdt.Open("rtmp://58.200.131.2:1935/livetv/gdtv",ui->openGLWidget);
//    xdt.Start();
//    const char *url = "rtmp://202.69.69.180:443/webcast/bshdlive-pc";
//}
MainWindow::~MainWindow()
{
    xdt.Close();
    delete ui;
}


void MainWindow::SliderPressed()
{
    isPress = true;
}

void MainWindow::SetVolume(int vol)
{
    double d_vol = (double)((101-vol)/100.0);
    if(vol == 0)d_vol = 0;
    xdt.setVol(d_vol);
}

void MainWindow::SliderRelease()
{

    isPress = false;
    double pos = (double)ui->play_SLD->value()/(double)ui->play_SLD->maximum();
    xdt.Seek(pos);
}
QString MainWindow::formatTime(long long ms)
{

    int ss = 1000;
    int mi = ss * 60;
    int hh = mi * 60;
    int dd = hh * 24;

    long long day = ms / dd;
    long long hour = (ms - day * dd) / hh;
    long long minute = (ms - day * dd - hour * hh) / mi;
    long long second = (ms - day * dd - hour * hh - minute * mi) / ss;
    long long milliSecond = ms - day * dd - hour * hh - minute * mi - second * ss;


    QString str = QString("%1:%2:%3").arg(hour,2,10,QLatin1Char('0')).arg(minute,2,10,QLatin1Char('0')) .arg(second,2,10,QLatin1Char('0'));
    //qDebug() << "minute:" << min << "second" << sec << "ms" << msec <<endl;

    return str ;
}
void MainWindow::timerEvent(QTimerEvent *e)
{
//    static bool isPressed = false;
//    connect(ui->play_SLD,&QSlider::sliderPressed,[=]()mutable{
//        isPressed =true;
//    });
//    connect(ui->play_SLD,&QSlider::sliderReleased,[=]()mutable{
//        isPressed =false;
//    });
    if (isPress)return;
    long long total = xdt.totalMs;
    if (total > 0)
    {
        double pos = (double)xdt.pts / (double)xdt.totalMs;
        int v = ui->play_SLD->maximum() * pos;
        ui->play_SLD->setValue(v);
        QString timeStr = QString(formatTime(xdt.pts)+" / "+formatTime(total));
        ui->play_LB->setText(timeStr);
    }
}


void MainWindow::resizeEvent(QResizeEvent *e)
{
    ui->ctrl_wid->move(30,this->height()-ui->ctrl_wid->height()-ui->statusbar->height()-30);
    ui->ctrl_wid->resize(this->width()-60,ui->ctrl_wid->height());
    ui->openGLWidget->resize(this->size());
    ui->openGLWidget->move(0,0);
}


void MainWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(this->isFullScreen()){
        ui->statusbar->show();
        ui->menubar->show();
        this->showNormal();
    }else {

        ui->statusbar->hide();
        ui->menubar->hide();
        this->showFullScreen();
    }
}


void MainWindow::on_actionopen_file_triggered()
{
    //选择文件
     QString name = QFileDialog::getOpenFileName(this, "选择视频文件");
     if (name.isEmpty())return;
     this->setWindowTitle(name);
     if (!xdt.Open(name.toLocal8Bit(), ui->openGLWidget))
     {
         QMessageBox::information(0, "error", "open file failed!");
         return;
     }     
     xdt.setPause(xdt.isPause);
}

//播放暂停按钮，测试视频暂停响应模块响应很快,音频线程需要暂停读取缓冲
void MainWindow::on_play_BTN_clicked()
{
    this->isPause = !this->isPause;
    xdt.setPause(this->isPause);
}





void MainWindow::on_actionopen_url_triggered()
{
    if(ui->lineEdit->isVisible()){
        this->setWindowTitle(ui->lineEdit->text());
        ui->actionopen_url->setText("open_url");
        if (!xdt.Open(ui->lineEdit->text().toLocal8Bit(), ui->openGLWidget))
        {
            QMessageBox::information(0, "error", "open file failed!");
        }
        ui->lineEdit->clear();
        ui->lineEdit->hide();
        xdt.setPause(xdt.isPause);
    }else {
        ui->lineEdit->show();
        ui->actionopen_url->setText("OK");
    }

}
