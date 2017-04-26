#include "aidsdlg.h"
#include "urlthread.h"
#include "bgmthread.h"
#include <QApplication>
#include <QThread>
#include <QTimer>
#include <ctime>
#include <cstdlib>
#include <QtGlobal>
#include <QTime>

int main(int argc, char *argv[])
{
//    srand((unsigned int) time(NULL));
//    qsrand((unsigned int)QThread::currentThreadId());

    QApplication a(argc, argv);
    AidsDlg w;

    UrlThread* uThr = new UrlThread(&w);
    QThread thr;

    BgmThread* bgmThr = new BgmThread();
    QThread thr2;

    QObject::connect(w.getStartBtn(), SIGNAL(clicked()), uThr, SLOT(onTimeout()));
    QObject::connect(w.getResetBtn(), SIGNAL(clicked()), uThr, SLOT(resetEvent()));
    QObject::connect(&w, SIGNAL(setSignal(double,int,int)), uThr, SLOT(setPara(double,int,int)));
    QObject::connect(uThr, SIGNAL(newConnectSignal()), &w, SLOT(newConnectEvent()));
//    QTimer::singleShot(300, uThr, SLOT(onTimeout()));
    uThr -> moveToThread(&thr);
    thr.start();

    QObject::connect(w.getStartBtn(), SIGNAL(clicked()), bgmThr, SLOT(playBgm()));
    QObject::connect(uThr, SIGNAL(collisionSignal()), bgmThr, SLOT(playCollision()));
    QObject::connect(w.getPlayBtn(), SIGNAL(clicked()), bgmThr, SLOT(playCollision()));
    QObject::connect(&w, SIGNAL(stopMusicSignal()), bgmThr, SLOT(stopPlay()));
    bgmThr -> moveToThread(&thr2);
    thr2.start();

    w.show();

    return a.exec();
}
