#include "bgmthread.h"
#include <QCoreApplication>

BgmThread::BgmThread(QObject *parent) : QObject(parent)
{
    QString path = QCoreApplication ::applicationDirPath();
    bgmPlayer = new QSound(path + "/Sound/BGM.wav", this);
    collisionPlayer = new QSound(path + "/Sound/Collision.wav", this);
    successPlayer = new QSound(path + "/Sound/Success.wav", this);

}

void BgmThread::playBgm()
{
    bgmPlayer -> play();
}

void BgmThread::playCollision()
{
    collisionPlayer -> play();
}

void BgmThread::playSuccess()
{
    successPlayer -> play();
}

void BgmThread::stopPlay()
{
    bgmPlayer -> stop();
    collisionPlayer -> stop();
    successPlayer -> stop();
}
