#ifndef BGMTHREAD_H
#define BGMTHREAD_H

#include <QObject>
#include <QSound>

class BgmThread : public QObject
{
    Q_OBJECT
public:
    explicit BgmThread(QObject *parent = 0);

    QSound *bgmPlayer, *collisionPlayer, *successPlayer;

signals:

public slots:
    void playBgm();
    void playCollision();
    void playSuccess();
    void stopPlay();
};

#endif // BGMTHREAD_H
