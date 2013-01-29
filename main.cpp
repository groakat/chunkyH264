#include <QtGui/QApplication>
#include <QFile>
#include "mainwindow.h"


int evalArgs(QStringList args, MainWindow* w){
    QStringList filter;

    filter = args.filter("--baseDir");
    if(!filter.isEmpty()){
        QStringList arg = filter[0].split("=");
        w->setBaseDir(arg[1]);
    }

    filter = args.filter("--device");
    if(!filter.isEmpty()){
        QStringList arg = filter[0].split("=");
        w->setDevice(arg[1]);
    }

    filter = args.filter("--isMaster");
    if(!filter.isEmpty()){
        QStringList arg = filter[0].split("=");
        w->setIsMaster(arg[1].toLower() == "true");
    }

    filter = args.filter("--relayDay");
    if(!filter.isEmpty()){
        QStringList arg = filter[0].split("=");
        w->setRelayDay(arg[1].toInt());
    }

    filter = args.filter("--relayNight");
    if(!filter.isEmpty()){
        QStringList arg = filter[0].split("=");
        w->setRelayNight(arg[1].toInt());
    }

    filter = args.filter("--directStart");
    if(!filter.isEmpty()){
        QStringList arg = filter[0].split("=");
        w->setDirectStart(arg[1].toLower() == "true");
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    evalArgs(a.arguments(), &w);
    w.show();

    return a.exec();
}
