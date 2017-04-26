#include "aidsclient.h"
#include "ui_aidsclient.h"

AidsClient::AidsClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AidsClient)
{
    ui->setupUi(this);

    msgEdt = new QTextEdit(this);
    sendBtn = new QPushButton(QObject::tr("发送"), this);
    infoEdt = new QTextEdit(this);
    mSocket = new QTcpSocket(this);

    uiInit();

    connect(sendBtn, SIGNAL(clicked()), this, SLOT(sendBtnEvent()));
    connect(mSocket, SIGNAL(readyRead()),this, SLOT(dataReceived()));

    mSocket -> connectToHost("127.0.0.1", 4000);
}

AidsClient::~AidsClient()
{
    mSocket -> disconnectFromHost();
    delete ui;
}

void AidsClient::sendBtnEvent()
{
    QString vMsgStr = msgEdt -> toPlainText();
    mSocket -> write(vMsgStr.toLatin1(), vMsgStr.length());
    infoEdt -> append(vMsgStr);
    msgEdt -> clear();
}

void AidsClient::dataReceived()
{
     while(mSocket -> bytesAvailable())
     {
          QByteArray vTemp;
          vTemp = mSocket -> readLine();
          QString vTempStr(vTemp);
          infoEdt -> append(vTempStr);
          qDebug() << "Recv: " << vTemp;
//          int length = 0;
//          QString vMsgStr = vTempStr;
//          mSocket -> write(vMsgStr, vMsgStr.length());
    }

}

void AidsClient::uiInit()
{
    vLayout = new QVBoxLayout();

    vLayout -> addWidget(infoEdt);
    vLayout -> addWidget(msgEdt);
    vLayout -> addWidget(sendBtn);

    setLayout(vLayout);
}
