#ifndef AIDSCLIENT_H
#define AIDSCLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace Ui {
class AidsClient;
}

class AidsClient : public QWidget
{
    Q_OBJECT

public:
    explicit AidsClient(QWidget *parent = 0);
    ~AidsClient();

private:
    Ui::AidsClient *ui;

private:
    QTcpSocket *mSocket;
    QTextEdit *msgEdt;
    QTextEdit *infoEdt;
    QPushButton *sendBtn;
    QVBoxLayout *vLayout;

    void uiInit();

private slots:
    void dataReceived();
    void sendBtnEvent();
};

#endif // AIDSCLIENT_H
