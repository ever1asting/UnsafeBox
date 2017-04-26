#ifndef AIDSDLG_H
#define AIDSDLG_H

#include "linkpair.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextLine>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QPainter>
#include <list>
#include <QTimer>

using std::list;

namespace Ui {
class AidsDlg;
}

class AidsDlg : public QWidget
{
    Q_OBJECT

public:
    explicit AidsDlg(QWidget *parent = 0);
    ~AidsDlg();
    double getProb() {return (probTextLine -> toPlainText()).toDouble();}
    int getTotTime() {return (totTimeTextLine -> toPlainText()).toInt();} // seconds
    int getTimeSpan() {return (timeSpanTextLine -> toPlainText()).toInt();} // seconds
    QPushButton* getStartBtn() {return startBtn;}
    QPushButton* getResetBtn() {return resetBtn;}
    QPushButton* getPlayBtn() {return playBtn;}

    static const int CUBE_ARR_SIZE = 6;
    int cubeWidthArr[CUBE_ARR_SIZE]; //= 55;
    int cubeHeightArr[CUBE_ARR_SIZE];// = 55;
    int dispLabelWidth, dispLabelHeight;
    int radius;
    int baseX, baseY;
    static const int labelBiasX = 10;
    static const int labelBiasY = 10;
//    static const int PI = acos(-1);

    // add lp to linkPairList
    void lpl_add(LinkPair lp);
    void linkEventAdd(LinkPair lp);
    void setCubeArr(int* c);

private:
    // used for timer before animation
    bool startAniFlag;
    QTimer *remainTimer;
    int remainTime;
    const int MAX_REMAIN_TIME = 120;

    // used for simulate the process of AIDS
    double prob;
    int totTime;
    int timeSpan;
    int initAidsNum;
    int aidsState[CUBE_ARR_SIZE];
    void initAids(int);
    bool simulate();

    // used for piant
    const int EVENT_INTERVAL = 2000; // the interval of two link event's happening
    list<LinkPair> linkPairList;   // the list of those link pair that need to be draw
    list<LinkPair> linkEventQueue; // the seq of the happening of link event
    QTimer *lpColorTimer, *lpAngleTimer, *lpEventTimer;
    int linkTimes[CUBE_ARR_SIZE][CUBE_ARR_SIZE]; // [i][j] records how many times i and j are linked
    const int DELTA_WIDTH = 2;     // the increase of pen width for each link
    const int DELTA_ANGLE = 30;     // the increase of angle for each link arc
    QTimer *cubeWidthTimer;
    const int CUBE_WIDTH_INTERVAL = 500;
    int infectAniFlag[CUBE_ARR_SIZE];
    int infectAniCnt[CUBE_ARR_SIZE];
    const int DELTA_CUBE_WIDTH = 3;
    const int DELTA_CUBE_HEIGHT = 3;
    const int INFECT_ANI_CNT_MAX = 5;
    const int CUBE_WIDTH_INIT = 55;
    const int CUBE_HEIGHT_INIT = 55;

    // used for UI
    Ui::AidsDlg *ui;
    QVBoxLayout *vLayout, *paraGrp;
    QHBoxLayout *hLayout;
    QLabel *dispLabel; // temp <label>， may be changed
    QTextEdit *botmLabel; // unknown use
    QPushButton *setBtn;
    QPushButton *startBtn;
    QPushButton *playBtn;
    QPushButton *resetBtn;
    QTextEdit *probTextLine, *totTimeTextLine, *timeSpanTextLine;
    QLabel *probLabel, *totTimeLabel, *timeSpanLabel;
    void uiInit();
//    QPoint num2pt(int i);

    // draw the label
    void paintEvent(QPaintEvent*);
    int cubeArr[CUBE_ARR_SIZE]; // 0 - blue, 1 - red

    int getRotateAngle(int, int);
    void LinkArc(QPainter&, int, int, int);
    QColor getLinkArcColor(int);

signals:
    void setSignal(double, int, int);
    void stopMusicSignal();

private slots:
    void setBtnOnClicked();
    void startBtnOnClicked();
    void playBtnOnClicked();
    void resetBtnOnClicked();
    void colorTimeEvent();
    void angleTimeEvent();
    void eventTimeEvent();
    void cubeWidthTimeEvent();
    void remainTimeEvent();
    void newConnectEvent();
};

#endif // AIDSDLG_H
