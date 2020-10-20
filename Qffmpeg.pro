QT       += core gui opengl  multimedia widgets openglextensions

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#INCLUDEPATH += $$PWD/../../include
#libpath = $$PWD/../../lib
#LIBS += -L$$PWD./../../lib
##for(var,libpath){
##    message($$var)
##    LIBS += -l
##}

##根据Path当层寻找.so和.a文件，返回相对路径
#LibSo = $$files($$join(libpath, , , /*.lib), false)
#message(LibSo)
#message($$LibSo)

##去路径
#LibSo = $$basename(LibSo)

##去后缀
#LibSo = $$section(LibSo, ., 0, 0)


###lib头替换-l
#for(var,LibSo){
#    message($$var)
#    err0 = $$find(var,"swscale")
#    err1 = $$find(var,"swresample")
#    err = err0 + err1
#    isEmpty(err){
#        LIBS =$$join(var, , -l, )
#    }
#}

###LibName = $$replace(LibName, lib, -l)
##LibName =$$join(Sample, , -l, )
##message(LibName)
##message($$LibName)
###添加库
##LIBS += $$LibName


#DESTDIR = ./../../bin
CONFIG(debug, debug|release){
    DESTDIR = $$PWD/../../bin
}else{
    DESTDIR = $$PWD/../../bin
}

SOURCES += \
    XAudioPlay.cpp \
    XAudioThread.cpp \
    XDecode.cpp \
    XDecodeThread.cpp \
    XDemux.cpp \
    XDemuxThread.cpp \
#    XFilter.cpp \
    XResample.cpp \
    XVideoThread.cpp \
    XVideoWidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    IVideoCall.h \
    XAudioPlay.h \
    XAudioThread.h \
    XDecode.h \
    XDecodeThread.h \
    XDemux.h \
    XDemuxThread.h \
#    XFilter.h \
    XResample.h \
    XVideoThread.h \
    XVideoWidget.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



unix:!macx|win32: LIBS += -L$$PWD/../../lib/ -lavcodec


INCLUDEPATH += $$PWD/../../include
DEPENDPATH += $$PWD/../../include

#include(D:\\Library\\opencvLib\\QT_pri\\OpenCV3100_GNU_x86_Dynamic.pri)

#DISTFILES += \
#    ../../lib/opencvLib/QT_pri/OpenCV3100_GNU_x86_Dynamic.pri
message("qmake file run")
