#include "aidsdlg.h"
#include "ui_aidsdlg.h"
#include <QPen>
#include <QColor>
#include <cmath>
#include <QDebug>
#include <list>
#include <memory.h>
#include <ctime>
#include <cstdlib>
#include <QTime>
#include <QThread>

using std::list;


AidsDlg::AidsDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AidsDlg)
{
//    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    uiInit();

    // used for remain timer before animation
    startAniFlag = false;
    remainTimer = new QTimer(this);
    remainTime = MAX_REMAIN_TIME;

    lpColorTimer = new QTimer(this);
    lpAngleTimer = new QTimer(this);
    lpEventTimer = new QTimer(this);
    cubeWidthTimer = new QTimer(this);
    connect(setBtn, SIGNAL(clicked()), this, SLOT(setBtnOnClicked()));
    connect(startBtn, SIGNAL(clicked()), this, SLOT(startBtnOnClicked()));
    connect(playBtn, SIGNAL(clicked()), this, SLOT(playBtnOnClicked()));
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(resetBtnOnClicked()));
    connect(lpColorTimer, SIGNAL(timeout()), this, SLOT(colorTimeEvent()));
    connect(lpAngleTimer, SIGNAL(timeout()), this, SLOT(angleTimeEvent()));
    connect(lpEventTimer, SIGNAL(timeout()), this, SLOT(eventTimeEvent()));
    connect(cubeWidthTimer, SIGNAL(timeout()), this, SLOT(cubeWidthTimeEvent()));
    connect(remainTimer, SIGNAL(timeout()), this, SLOT(remainTimeEvent()));

    dispLabelWidth = dispLabel -> width();
    dispLabelHeight = dispLabel -> height();
    radius = 0.83 * dispLabelHeight / 2;
    baseX = dispLabelWidth / 2;
    baseY = dispLabelHeight / 2;

    qDebug("%d, %d, %d, %d\n", dispLabelHeight, dispLabel -> height(),
           dispLabelWidth, dispLabel -> width());

    for (int i = 0; i < CUBE_ARR_SIZE; ++i) {
        cubeArr[i] = 0;
        cubeHeightArr[i] = CUBE_HEIGHT_INIT;
        cubeWidthArr[i] = CUBE_WIDTH_INIT;
    }

    memset(linkTimes, 0, sizeof(int)*CUBE_ARR_SIZE*CUBE_ARR_SIZE);
    memset(aidsState, 0, sizeof(int)*CUBE_ARR_SIZE);
    memset(infectAniFlag, 0, sizeof(int)*CUBE_ARR_SIZE);
    memset(infectAniCnt, 0, sizeof(int)*CUBE_ARR_SIZE);

    // parameters for simulation
//    srand((unsigned int) time(NULL));
}

AidsDlg::~AidsDlg()
{
    delete ui;
}

void AidsDlg::initAids(int N)
{
    // set text
    botmLabel -> append("There are " + QString::number(N) + " AIDS patients in group.");
    for (int i = 0; i < CUBE_ARR_SIZE; ++i) {
        if (cubeArr[i] == 1)
            botmLabel -> append("No." + QString::number(i) + " is an AIDS patient.");
    }

}

bool AidsDlg::simulate()
{
    for (int i = 0; i < totTime / timeSpan; ++i) {
        if (((rand() % 1000) / (double)1000) < prob) {
            return true;
        }
    }
    return false;
}


void AidsDlg::setBtnOnClicked()
{
    // TODO: .....
    qDebug("pressed\n");
    // ignore checking the legality
    prob = getProb();
    totTime = getTotTime();
    timeSpan = getTimeSpan();
    emit(setSignal(prob, totTime, timeSpan));
}

void AidsDlg::playBtnOnClicked()
{
    // TODO:...
    qDebug("playBtn pressed, %d\n", linkEventQueue.size());

    botmLabel -> clear();
    initAids(1);

    startAniFlag = true;
    lpEventTimer -> start(EVENT_INTERVAL);
    lpColorTimer -> start(1000);
    lpAngleTimer -> start(100);
}


void AidsDlg::startBtnOnClicked()
{
    // TODO:...
    qDebug("startBtn pressed\n");
    remainTimer -> start(1000);
    setBtn -> setEnabled(false);
    startBtn -> setEnabled(false);
}

void AidsDlg::resetBtnOnClicked()
{
    qDebug() << "reset btn was pressed";

    emit(stopMusicSignal());

    startAniFlag = false;
    playBtn -> setEnabled(false);
    startBtn -> setEnabled(true);
    setBtn -> setEnabled(true);
    remainTime = MAX_REMAIN_TIME;

    lpColorTimer -> stop();
    lpAngleTimer -> stop();
    lpEventTimer -> stop();
    cubeWidthTimer -> stop();
    remainTimer -> stop();

    for (int i = 0; i < CUBE_ARR_SIZE; ++i) {
        cubeArr[i] = 0;
        cubeHeightArr[i] = CUBE_HEIGHT_INIT;
        cubeWidthArr[i] = CUBE_WIDTH_INIT;
    }

    memset(linkTimes, 0, sizeof(int)*CUBE_ARR_SIZE*CUBE_ARR_SIZE);
    memset(aidsState, 0, sizeof(int)*CUBE_ARR_SIZE);
    memset(infectAniFlag, 0, sizeof(int)*CUBE_ARR_SIZE);
    memset(infectAniCnt, 0, sizeof(int)*CUBE_ARR_SIZE);

    prob = 0.01;
    totTime = 365;
    timeSpan = 3;

    botmLabel -> clear();
    probTextLine -> clear();
    totTimeTextLine -> clear();
    timeSpanTextLine -> clear();

    while (!linkPairList.empty()) linkPairList.erase(linkPairList.begin());
    while (!linkEventQueue.empty()) linkEventQueue.erase(linkEventQueue.begin());
}


void AidsDlg::remainTimeEvent()
{
    if (remainTime > 0)
        --remainTime;
    else {
        playBtn -> setEnabled(true);
        startBtn -> setEnabled(false);
    }
}

void AidsDlg::angleTimeEvent()
{
    for (list<LinkPair>::iterator it = linkPairList.begin(); it != linkPairList.end(); ++it) {
        if (it -> arg < 120)
            it -> arg += DELTA_ANGLE;
    }
}

void AidsDlg::colorTimeEvent()
{
    for (list<LinkPair>::iterator it = linkPairList.begin(); it != linkPairList.end(); ++it) {
        it -> state += 10;
    }
}

void AidsDlg::eventTimeEvent()
{
    if (!linkEventQueue.empty()) {
        LinkPair lpTemp(linkEventQueue.front());
        if (lpTemp.isInfect) {
            int f = ((cubeArr[lpTemp.fir] == 1)? lpTemp.fir: lpTemp.sec);
            int s = ((cubeArr[lpTemp.fir] == 0)? lpTemp.fir: lpTemp.sec);
            // set text
            if (cubeArr[s] == 0)
                botmLabel -> append("No." + QString::number(s) + " was infected by No." + QString::number(f));
            // set animation flag
            infectAniFlag[s] = 1;
            cubeWidthTimer -> start(CUBE_WIDTH_INTERVAL);
        }

        qDebug() << "LINK " << lpTemp.fir << " " << lpTemp.sec << " " << lpTemp.isInfect;

        linkPairList.push_back(lpTemp);
        linkEventQueue.erase(linkEventQueue.begin());
    }
}

void AidsDlg::cubeWidthTimeEvent()
{
//    qDebug("cube width time event\n");
//    qDebug("infectAniFlag: %d, %d, %d, %d, %d, %d\n", infectAniFlag[0], infectAniFlag[1], infectAniFlag[2],
//            infectAniFlag[3], infectAniFlag[4], infectAniFlag[5]);


    for (int i = 0; i < CUBE_ARR_SIZE; ++i) {
        if (infectAniFlag[i] == 1) {
            cubeWidthArr[i] += DELTA_CUBE_WIDTH;
            cubeHeightArr[i] += DELTA_CUBE_HEIGHT;
            infectAniCnt[i]++;
        }
    }

    bool end = true;
    for (int i = 0; i < CUBE_ARR_SIZE; ++i) {
        if (infectAniCnt[i] >= INFECT_ANI_CNT_MAX) {
//            qDebug("exceed cnt max, %d\n", i);
            infectAniCnt[i] = 0;
            infectAniFlag[i] = 0;
            cubeWidthArr[i] = CUBE_WIDTH_INIT;
            cubeHeightArr[i] = CUBE_HEIGHT_INIT;
            cubeArr[i] = 1;
        }
        if (infectAniCnt[i] != 0) {
            end = false;
        }
    }
    if (end) {
        cubeWidthTimer -> stop();
    }
}

void AidsDlg::lpl_add(LinkPair lp)
{
    linkPairList.push_back(lp);
}

void AidsDlg::linkEventAdd(LinkPair lp)
{
    int t = linkTimes[lp.fir][lp.sec];
    linkTimes[lp.fir][lp.sec] = linkTimes[lp.sec][lp.fir] = t + DELTA_WIDTH;

    linkEventQueue.push_back(LinkPair(lp.fir, lp.sec, lp.state, lp.arg, lp.isInfect, t + DELTA_WIDTH));
}


void AidsDlg::LinkArc(QPainter& painter, int f, int s, int a)
{
    int fir = ((f > s)? s: f),
        sec = ((f > s)? f: s);
    if (sec - fir > 3) {
        int temp = fir;
        fir = sec;
        sec = temp;
    }

    int dist[4] = {0, 2*radius / 1.732, 1.733*radius, 2 * radius};
    int dif = ((abs(fir - sec) > 3)? abs(fir - sec) - 6: abs(fir - sec));
    int arcR = dist[abs(dif)];
    int arcHeight = 0.139746 * radius;

    painter.save();
    painter.translate(baseX, baseY);
    painter.rotate(90 - fir * 60);
    painter.translate(radius, 0);
//    painter.drawRect(0, -arcHeight, arcR, arcHeight);
//    painter.drawArc(0, -arcHeight, arcR, arcHeight, 30 * 16, 120 * 16);

    painter.rotate(getRotateAngle(fir, sec));
//    painter.drawRect(0, -arcHeight, arcR, 4*arcHeight);
    painter.drawArc(0, -arcHeight, arcR, 4 * arcHeight, 30 * 16, a * 16);
    painter.restore();
}

int AidsDlg::getRotateAngle(int fir, int sec)
{
    if (fir == 0) {
        return -(90 + 30 * sec);
    }
    if (fir == 1) {
        return -(90 + 30 * (sec - 1));
    }
    if (fir == 2) {
        return -(90 + 30 * (sec - 2));
    }
    if (fir == 3) {
        if (sec == 4) return -120;
        if (sec == 5) return -150;
        if (sec == 0) return -180;
    }
    if (fir == 4) {
        if (sec == 5) return -120;
        if (sec == 0) return -150;
        if (sec == 1) return -180;
    }
    if (fir == 5) {
        return -(90 + 30 * (1 + sec));
    }
    return 0;
}

//QPoint AidsDlg::num2pt(int i)
//{
//    return QPoint(baseX + radius * sin(i * PI / 3)/* + (-cubeWidth / 2)*/,
//                  baseY + radius * cos(i * PI / 3)/* + (-cubeHeight / 2)*/);
//}

QColor AidsDlg::getLinkArcColor(int state)
{
    int green;
    green = (state > 255)? 255: state;
    return QColor(0, 255 - green, 0);

    switch (state) {
    case 0:
        return QColor(0, 255, 0);
    case 1:
        return QColor(0, 205, 0);
    case 2:
        return QColor(0, 155, 0);
    case 3:
        return QColor(0, 105, 0);
    default:
        return QColor(0, 55, 0);
    }
}

void AidsDlg::setCubeArr(int* c)
{
    for (int i = 0; i < CUBE_ARR_SIZE; ++i)
        cubeArr[i] = c[i];
}

void AidsDlg::paintEvent(QPaintEvent* e)
{
    QPainter painter;
    QImage image(dispLabelWidth, dispLabelHeight ,QImage::Format_ARGB32);//定义图片，并在图片上绘图方便显示
    painter.begin(&image);
    painter.fillRect(0, 0, dispLabelWidth, dispLabelHeight, Qt::white);

    // the remain time is not zero, we should not play the animation
    if (!startAniFlag) {
        QPen pen(Qt::black, 22);
        painter.setPen(pen);
        QFont font;
        font.setPointSize(120);
        painter.setFont(font);
        painter.drawText(QRect(0, 0, dispLabelWidth, dispLabelHeight), QString::number(remainTime));

        painter.end();
        dispLabel->setPixmap(QPixmap::fromImage(image));
        return;
    }

    // draw the link-arcs
    QPen pen(Qt::green, 5);
    for (list<LinkPair>::iterator it = linkPairList.begin(); it != linkPairList.end(); ++it) {
        pen.setColor(getLinkArcColor(it -> state));
        pen.setWidth(it -> width);
        painter.setPen(pen);
        LinkArc(painter, it -> fir, it -> sec, it -> arg);
    }

    // draw the cube/circles
    for (int i = 0; i < CUBE_ARR_SIZE; ++i) {
        QColor qColor(Qt::white);
        switch (cubeArr[i]) {
            case 0:
                qColor = Qt::blue;
                break;
            case 1:
                qColor = Qt::red;
                break;
            default:
                break;
        }

        painter.setPen(QPen(Qt::white, 0));
        painter.setBrush(QBrush(qColor));
        painter.save();
        painter.translate(baseX, baseY);
        painter.rotate(90 - 60 * i);

        // draw standard line
//        painter.setPen(QPen(Qt::black, 5));
//        if (i == 0) painter.setPen(QPen(Qt::red, 5));
//        if (i == 1) painter.setPen(QPen(Qt::yellow, 5));
//        painter.drawLine(0, 0, radius, 0);
//        painter.setPen(QPen(Qt::white, 0));

        painter.drawEllipse(radius + (-cubeWidthArr[i] / 2), (-cubeHeightArr[i] / 2), cubeWidthArr[i], cubeHeightArr[i]);
        painter.restore();
    }


    painter.end();
    dispLabel->setPixmap(QPixmap::fromImage(image));
}

void AidsDlg::newConnectEvent()
{
    botmLabel -> append("new client connected to the host");
}

void AidsDlg::uiInit()
{
    setFixedSize(800, 600);

    vLayout = new QVBoxLayout();
    paraGrp = new QVBoxLayout();
    hLayout = new QHBoxLayout();
    dispLabel = new QLabel();
    botmLabel = new QTextEdit();
    botmLabel -> setReadOnly(true);
    setBtn = new QPushButton("SET", this);
    startBtn = new QPushButton("START", this);
    playBtn = new QPushButton("PLAY", this);
    resetBtn = new QPushButton("RESET", this);

    playBtn -> setEnabled(false);

    // set border
    botmLabel -> setFrameShape(QFrame::Box);
    dispLabel -> setFrameShape(QFrame::Box);

    // parameters label and textline
    probLabel = new QLabel("Probability", this);
    probTextLine = new QTextEdit();//QTextLine();
    probLabel -> setBuddy(probTextLine);

    totTimeLabel = new QLabel("Total Time", this);
    totTimeTextLine = new QTextEdit();//QTextLine();
    totTimeLabel -> setBuddy(totTimeTextLine);

    timeSpanLabel = new QLabel("Time Span", this);
    timeSpanTextLine = new QTextEdit();//QTextLine();
    timeSpanLabel -> setBuddy(timeSpanTextLine);


    // construct para group
    paraGrp -> addWidget(probLabel);
    paraGrp -> addWidget(probTextLine);
    paraGrp -> addWidget(totTimeLabel);
    paraGrp -> addWidget(totTimeTextLine);
    paraGrp -> addWidget(timeSpanLabel);
    paraGrp -> addWidget(timeSpanTextLine);
    paraGrp -> addWidget(setBtn);
    paraGrp -> addWidget(startBtn);
    paraGrp -> addWidget(playBtn);
    paraGrp -> addWidget(resetBtn);

    // construct hboxLayout
    hLayout -> addWidget(dispLabel);
    hLayout -> addLayout(paraGrp);
    hLayout -> setStretchFactor(dispLabel, 7);
    hLayout -> setStretchFactor(paraGrp, 3);

    //construct vboxLayout
    vLayout -> addLayout(hLayout);
    vLayout -> addWidget(botmLabel);
    vLayout -> setStretchFactor(hLayout, 4);
    vLayout -> setStretchFactor(botmLabel, 1);

    setLayout(vLayout);
}
