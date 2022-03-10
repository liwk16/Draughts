#include "draughts.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Draughts w;
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
    //w.initServer();
    w.show();

    return a.exec();
}
