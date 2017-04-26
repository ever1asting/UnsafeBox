#include "urlthread.h"
#include <QDebug>
#include <cstdlib>
#include <ctime>
#include <QStringList>
#include <memory.h>
#include <QThread>
#include <QTime>
#include <QCoreApplication>

UrlThread::UrlThread(QObject *parent) : QObject(parent)
{
//    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

UrlThread::UrlThread(AidsDlg* aidsDlg, QObject *parent)
 : QObject(parent)
{
//    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
//    qsrand((unsigned int)QThread::currentThreadId());

    // set path and clear the content of output TXT first
    QString path = QCoreApplication ::applicationDirPath() + "/";
    outPath = path.toStdString() + outPath;
    qDebug() << QString::fromStdString(outPath);
    fout.open(outPath.c_str());
    if (!fout.is_open())
        qDebug() << "failed to open the file\n";
    else
        qDebug() << "open the file. OK\n";
    fout.close();

    // init parameters for simulation
    prob = 0.007;
    totTime = 365;
    timeSpan = 3;

    // socket init
    aidsDlgPtr = aidsDlg;

    isFirstTime = true;
    clientConnect = false;
    allPairConnect = false;
    memset(connectState, 0, sizeof(connectState));
    memset(cubeArr, 0, sizeof(cubeArr));

    mServer = new QTcpServer(this);
    mSocket = new QTcpSocket(this);
//    mSocket -> setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(mServer, SIGNAL(newConnection()), this, SLOT(newConnectionSlot()));
    if (!mServer->isListening())
    {
        if (mServer -> listen(QHostAddress::Any, 4000))
            qDebug("succeed to open the listening port\n");
        else
            qDebug("failed to open the listening port\n");
    }
    else
    {
        qDebug("already listening\n");
    }
}

void UrlThread::resetEvent()
{
    fout.open(outPath.c_str());
    fout.close();

    // init parameters for simulation
    prob = 0.007;
    totTime = 365;
    timeSpan = 3;

    clientConnect = false;
    allPairConnect = false;
    memset(connectState, 0, sizeof(connectState));
    memset(cubeArr, 0, sizeof(cubeArr));
}

void UrlThread::roundInit()
{
    qDebug() << "in round init";

    roundInitTimer -> stop();

    allPairConnect = false;
    connectPairSize = 0;

    genColorPair(colorPair);
    qDebug("%d, %d, %d, %d, %d, %d\n", colorPair[0], colorPair[1],
            colorPair[2], colorPair[3], colorPair[4], colorPair[5]);
    for (int i = 0; i < 6; ++i) {
        connectState[i] = ((colorPair[i] == -1)? 1: 0);
        send("COLOR " + QString::number(i) + " " + num2rgb(colorPair[i]));
    }
}

void UrlThread::initAids(int N)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    memset(cubeArr, 0, sizeof(cubeArr));

    // output
    fout.open(outPath.c_str(), std::ios::app);

    int a[CUBE_ARR_SIZE];
    for (int i = 0; i < CUBE_ARR_SIZE; ++i)
        a[i] = i;

    for (int i = 0; i < N; ++i) {
        int ran = qrand();
//        qDebug() << "in init aids, random = " << ran;
        int index = ran % (CUBE_ARR_SIZE - i);
        int temp = a[index];
        a[index] = a[CUBE_ARR_SIZE - i - 1];
        a[CUBE_ARR_SIZE - i - 1] = temp;
        cubeArr[temp] = 1;
        qDebug() << "No." << temp << " was selected as an AIDS patient.";
        fout << "INFECT " << temp << "\n";
    }

    fout.close();

    qDebug("cubeArr %d, %d, %d, %d, %d, %d", cubeArr[0], cubeArr[1], cubeArr[2], cubeArr[3], cubeArr[4], cubeArr[5]);

    //info the aidsDlg
    aidsDlgPtr -> setCubeArr(cubeArr);
}

void UrlThread::onTimeout()
{
    // the first round, we should wait client and there is no need to EJECT
    qDebug("url thread time out\n");

    if (isFirstTime) {

        roundInitTimer = new QTimer(this);
        connect(roundInitTimer, SIGNAL(timeout()), this, SLOT(roundInit()));
        restoreColorTimer = new QTimer(this);
        connect(restoreColorTimer, SIGNAL(timeout()), this, SLOT(restoreColor()));

        // wait for client
        while (1) {
            if (clientConnect) break;
        }

        isFirstTime = false;
    }

    initAids(1);

    allPairConnect = false;
    connectPairSize = 0;

    genColorPair(colorPair);
    qDebug("%d, %d, %d, %d, %d, %d\n", colorPair[0], colorPair[1],
            colorPair[2], colorPair[3], colorPair[4], colorPair[5]);
    for (int i = 0; i < 6; ++i) {
        connectState[i] = ((colorPair[i] == -1)? 1: 0);
        send("COLOR " + QString::number(i) + " " + num2rgb(colorPair[i]));
    }
}


void UrlThread::send(QString vMsgStr)
{
    qDebug() << "Send: " << vMsgStr;
    vMsgStr += "\n";
    mSocket -> write(vMsgStr.toLatin1(), vMsgStr.length());
}

void UrlThread::newConnectionSlot()
{
     qDebug("new client connected to the host\n");
     emit(newConnectSignal());

     mSocket = mServer -> nextPendingConnection();
     connect(mSocket, SIGNAL(disconnected()), mSocket, SLOT(deleteLater()));
     connect(mSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
//     QString vMsgStr = QObject::tr("Welcome");
//     mSocket -> write(vMsgStr.toLatin1(), vMsgStr.length());
     clientConnect = true;
}

bool UrlThread::simulate()
{
    for (int i = 0; i < totTime / timeSpan; ++i) {
        int ran = qrand();
//        qDebug() << "in simulate, random = " << ran;
        if (((ran % 1000) / (double)1000) < prob) {
            return true;
        }
    }
    return false;
}

void UrlThread::restoreColor()
{
    int num = restoreColorNum;
    send("COLOR " + QString::number(num) + " " + num2rgb(colorPair[num]));
    restoreColorTimer -> stop();
}

void UrlThread::dataReceived()
{
    qDebug() << "receive data\n";
    while(mSocket -> bytesAvailable())
    {
        QByteArray vTemp;
        vTemp = mSocket -> readLine();
        QString vTempStr(vTemp);

        // TODO: ... do something when receive str
        QString rec = "Recv: " + vTempStr + "\n";
        qDebug() << rec;

        int fir, sec;
        str2pair(vTempStr, fir, sec);
        if (((fir + sec) % 2 == 0) || colorPair[fir] == -1 || colorPair[sec] == -1 || colorPair[fir] != colorPair[sec]) {
            send("EJECT " + QString::number(fir) + " " + QString::number(sec));
        }
        else if (colorPair[fir] == colorPair[sec] && connectState[fir] == 1 && connectState[sec] == 1) {

        }
        else {
            // play Collision.wav
            emit(collisionSignal());

            connectState[fir] = connectState[sec] = 1;

            // record the connected pair
//            qDebug("connect pair (%d %d) = (%d %d)", connectPairSize, connectPairSize+1,
//                   fir, sec);
            connectPair[connectPairSize] = fir;
            connectPair[connectPairSize + 1] = sec;
            connectPairSize += 2;

            if (isAllConnect()) {
                // output
                fout.open(outPath.c_str(), std::ios::app);
                fout << "NEXT\n";

                // simulation
                for (int i = 0; i < connectPairSize / 2; ++i) {
                    if ((cubeArr[connectPair[2*i]] == 0 && cubeArr[connectPair[2*i+1]] == 1) ||
                       (cubeArr[connectPair[2*i]] == 1 && cubeArr[connectPair[2*i+1]] == 0)) {
                        if (simulate()) {
                            int num = ((cubeArr[connectPair[2*i]] == 0)?
                                        connectPair[2*i]: connectPair[2*i+1]);
                            cubeArr[num] = 1;
                            // turn yellow and then restore
                            send("COLOR " + QString::number(num) + " 255 255 0");

                            restoreColorNum = num;
                            restoreColorTimer -> start(500);
//                            QTimer::singleShot(500, this, SLOT(restoreColor(num)));
                            // add link event (infection case)
                            aidsDlgPtr -> linkEventAdd(LinkPair(connectPair[2*i], connectPair[2*i+1], 0, 0, 1));
                            qDebug("IF: add link pair (%d %d)", connectPair[2*i], connectPair[2*i+1]);
                            fout << "LINK " << connectPair[2*i] << " " << connectPair[2*i+1] << "\n";
                            fout << "INFECT " << num << "\n";
                        }
                        else {
                            aidsDlgPtr -> linkEventAdd(LinkPair(connectPair[2*i], connectPair[2*i+1], 0, 0, 0));
                            qDebug("IF ELSE: add link pair (%d %d)", connectPair[2*i], connectPair[2*i+1]);
                            fout << "LINK " << connectPair[2*i] << " " << connectPair[2*i+1] << "\n";
                        }
                    }
                    else {// add link event
                        aidsDlgPtr -> linkEventAdd(LinkPair(connectPair[2*i], connectPair[2*i+1], 0, 0, 0));
                        qDebug("ELSE: add link pair (%d %d)", connectPair[2*i], connectPair[2*i+1]);
                        fout << "LINK " << connectPair[2*i] << " " << connectPair[2*i+1] << "\n";
                    }
                }

                fout.close();

                // next round
                // EJECT and all (0, 0, 0)
                for (int i = 0; i < connectPairSize / 2; ++i) {
                    send("EJECT " + QString::number(connectPair[2*i]) + " " +
                         QString::number(connectPair[2*i+1]));
                }

                QThread::sleep(2);
                for (int i = 0; i < CUBE_ARR_SIZE; ++i) {
                    send("COLOR " + QString::number(i) + " 0 0 0");
                }

                qDebug() << "before signle shot";
                roundInitTimer -> start(ROUND_TIME);
//                QTimer::singleShot(ROUND_TIME, this, SLOT(roundInit()));
            }
        } // end else

    } // end while
}

bool UrlThread::isAllConnect()
{
    for (int i = 0; i < 6; ++i) {
        if (connectState[i] == 0)
            return false;
    }
    return true;
}

QString UrlThread::num2rgb(int n)
{

    switch (n) {
    case -1:
        return "0 0 0";
    default:
        int r, g, b;
        colorLib[n].getRgb(&r, &g, &b);
        return QString::number(r) + " " + QString::number(g) + " " + QString::number(b);
    }
    return "0 0 0";
}

void UrlThread::genColorPair(int *a)
{
    int mark[6];
    memset(mark, 0, sizeof(mark));
    int odd[3] = {1, 3, 5};

    int colorNum = 4;

    int ranIndex = 3 + (qrand() % 3);
    int ranColor = qrand() % colorNum;
    a[0] = a[ranIndex] = ranColor;
    mark[0] = mark[ranIndex] = 1;

    ranIndex = (qrand() % 2);
    bool one = false;
    for (int i = 3; i < 6; ++i) {
        if (ranIndex == 0 && mark[i] == 0) {
            ranColor = qrand() % colorNum;
            a[1] = a[i] = ranColor;
            mark[1] = mark[i] = 1;
            break;
        }
        else if (ranIndex == 1) {
            if (!one && mark[i] == 0)
                one = true;
            else if (one && mark[i] == 0) {
                ranColor = qrand() % colorNum;
                a[1] = a[i] = ranColor;
                mark[1] = mark[i] = 1;
                break;
            }
        }
    }

    for (int i = 3; i < 6; ++i) {
        if (mark[i] == 0) {
            a[2] = a[i] = (qrand() % colorNum);
            break;
        }
    }

    int temp = a[1];
    a[1] = a[4];
    a[4] = temp;

    return;

    // -1 - invalide, 0 - r, 1 - g, 2 - b
    int arr[6], N = 3;
    for (int i = 0; i < 6; ++i)
        arr[i] = i;

    for (int i = 0; i < 3; ++i) {
        int ran = qrand();
//        qDebug() << "in gen color, random = " << ran;
        int index = 2 * (ran % (3 - i));
        int temp = arr[index];
        arr[index] = arr[6 - 2*i - 1];
        arr[6 - 2*i - 1] = temp;
        temp = arr[index + 1];
        arr[index + 1] = arr[6 - 2*i - 2];
        arr[6 - 2*i - 2] = temp;
    }

    while (1) {
        for (int i = 0; i < 6; ++i)
            a[i] = -1;

        int tot = 3;
        for (int i = 0; i < 3; ++i) {
            if (tot == 0) break;

            int ran = qrand();
//            qDebug() << "in gen color 2, random = " << ran;
            int n = (ran % (tot + 1));
            for (int j = 0; j < 2 * n; ++j)
                a[arr[2 * (3 - tot) + j]] = i;

            tot -= n;
        }

        int cnt = 0;
        for (int i = 0; i < 6; ++i)
            cnt += (a[i] == -1);
        if (cnt < 4) break;
    }

}

void UrlThread::str2pair(const QString& str, int& fir, int& sec)
{
    QStringList ql = str.split(" ");
    if (ql.size() != 3) {
        qDebug("illegal str comes up when converted to num pair\n");
        return;
    }
    fir = ql.at(1).toInt();
    sec = ql.at(2).toInt();
}

void UrlThread::setPara(double p, int tt, int ts)
{
    prob = p;
    totTime = tt;
    timeSpan = ts;
    qDebug() << "set para: " << prob << " " << tt << " " << ts;
}
