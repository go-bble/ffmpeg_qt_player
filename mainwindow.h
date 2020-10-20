 #ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <XDemux.h>
#include <XDecode.h>
#include <XVideoWidget.h>
#include <QThread>
#include <XAudioPlay.h>
#include <XResample.h>
#include <XAudioThread.h>
#include <XDemuxThread.h>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void timerEvent(QTimerEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e)override;

    QString formatTime(long long ms);
private slots:
    void on_actionopen_file_triggered();

    void on_play_BTN_clicked();
    void SliderRelease();
    void SliderPressed();
    void SetVolume(int vol);
    void on_actionopen_url_triggered();

private:
    Ui::MainWindow *ui;
    bool isPause = false;
	bool isPress = false;
};
#endif // MAINWINDOW_H


