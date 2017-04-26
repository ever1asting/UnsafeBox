#include "aidsclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AidsClient w;
    w.show();

    return a.exec();
}
