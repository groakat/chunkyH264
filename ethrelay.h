#ifndef ETHRELAY_H
#define ETHRELAY_H
#include <QString>
#include <QtNetwork/QtNetwork>

class EthRelay
{
public:
    EthRelay();

    void setDevice(QString address, int port);
    void connect();
    int switchOn(int relay);
    int switchOff(int relay);
    int switchAllOn();
    int switchAllOff();

    enum RELAYSTATUS{
        SUCCESS = 0,
        NOT_CONNECTED = 1,
        ARGUMENT_OUT_OF_RANGE = 2
    };


private:
    QString address;
    int port;
    QTcpSocket socket;
    bool isConnected;
};

#endif // ETHRELAY_H
