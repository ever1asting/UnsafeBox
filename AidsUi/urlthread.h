#ifndef URLTHREAD_H
#define URLTHREAD_H

#include "aidsdlg.h"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <fstream>

class UrlThread : public QObject
{
    Q_OBJECT
public:
    explicit UrlThread(QObject *parent = 0);
    UrlThread(AidsDlg* aidsDlg, QObject *parent = 0);

private:
    bool isFirstTime;
    int totTime, timeSpan;
    double prob;
    const QColor colorLib[4] = {    (QColor(0xFF0000)),
                                 (QColor(0xFF7F00)),
//                                    QColor(0x8B008B), // DarkMagenta
                                 (QColor(0x00FF00)),
                                 (QColor(0x0000FF)),};

    QTimer* roundInitTimer;
    QTimer* restoreColorTimer;
    int restoreColorNum;

    std::ofstream fout;
    std::string outPath = "out.txt";

    AidsDlg* aidsDlgPtr;
    QTcpServer *mServer;
    QTcpSocket *mSocket;
    bool clientConnect, allPairConnect;
    int colorPair[6];
    int connectState[6];
    const int ROUND_TIME = 3500;
    int connectPair[6];
    int connectPairSize;
    static const int CUBE_ARR_SIZE = 6;
    int cubeArr[CUBE_ARR_SIZE];

    void send(QString);
    void genColorPair(int *);
    void str2pair(const QString&, int&, int&);
    bool isAllConnect();
    QString num2rgb(int);
    bool simulate();
    void initAids(int);

private slots:
    void newConnectionSlot();
    void dataReceived();
    void roundInit();
    void restoreColor();
    void resetEvent();

signals:
    void startSignal();
    void collisionSignal();
    void newConnectSignal();

public slots:
    void onTimeout();
    void setPara(double, int, int);
};

#endif // URLTHREAD_H
