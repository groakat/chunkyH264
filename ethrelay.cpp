#include <ethrelay.h>

EthRelay::EthRelay()
{
    this->address = "192.168.0.200";
    this->port = 17494;
    this->isConnected = false;
}

void EthRelay::setDevice(QString address, int port)
{
    this->address = address;
    this->port = port;
}

void EthRelay::connect()
{
    this->socket.connectToHost(this->address, this->port);
    if (this->socket.waitForConnected(1000)){
        qDebug() << "connected to relay " << this->address << ":" << this->port;
        this->isConnected = true;
    }else{
        qDebug() << "could not connect to relay";
    }
}

int EthRelay::switchOn(int relay)
{
    if (this->isConnected){
        int payLoad;
        switch(relay){
            case 0: payLoad = 101; break;
            case 1: payLoad = 102; break;
            case 2: payLoad = 103; break;
            case 3: payLoad = 104; break;
            case 4: payLoad = 105; break;
            case 5: payLoad = 106; break;
            case 6: payLoad = 107; break;
            case 7: payLoad = 108; break;
            default: payLoad = -1;
        }
        QDataStream os(&this->socket);
        if (payLoad > 0){
            os << payLoad;
            if (this->socket.waitForBytesWritten(300)){
                qDebug() << "wrote bytes";
                return SUCCESS;
            }
        }else{
            qDebug() << "relay number out of valid range";
            return ARGUMENT_OUT_OF_RANGE;
        }
    }else{
        qDebug() << "not connected";
        return NOT_CONNECTED;
    }
}

int EthRelay::switchOff(int relay)
{
    if (this->isConnected){
        int payLoad;
        switch(relay){
            case 0: payLoad = 111; break;
            case 1: payLoad = 112; break;
            case 2: payLoad = 113; break;
            case 3: payLoad = 114; break;
            case 4: payLoad = 115; break;
            case 5: payLoad = 116; break;
            case 6: payLoad = 117; break;
            case 7: payLoad = 118; break;
            default: payLoad = -1;
        }
        QDataStream os(&this->socket);
        if (payLoad > 0){
            os << payLoad;
            if (this->socket.waitForBytesWritten(300)){
                qDebug() << "wrote bytes";
                return SUCCESS;
            }
        }else{
            qDebug() << "relay number out of valid range";
            return ARGUMENT_OUT_OF_RANGE;
        }
    }else{
        qDebug() << "not connected";
        return NOT_CONNECTED;
    }
}

int EthRelay::switchAllOn()
{
    if (this->isConnected){
        QDataStream os(&this->socket);
        os << 100;
        if (this->socket.waitForBytesWritten(300)){
            qDebug() << "wrote bytes";
            return SUCCESS;
        }else{
            qDebug() << "relay number out of valid range";
            return ARGUMENT_OUT_OF_RANGE;
        }
    }else{
        qDebug() << "not connected";
        return NOT_CONNECTED;
    }
}

int EthRelay::switchAllOff()
{
    if (this->isConnected){
        QDataStream os(&this->socket);
        os << 110;
        if (this->socket.waitForBytesWritten(300)){
            qDebug() << "wrote bytes";
            return SUCCESS;
        }else{
            qDebug() << "relay number out of valid range";
            return ARGUMENT_OUT_OF_RANGE;
        }
    }else{
        qDebug() << "not connected";
        return NOT_CONNECTED;
    }
}

