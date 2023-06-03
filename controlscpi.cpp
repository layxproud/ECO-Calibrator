#include "controlscpi.h"

ControlSCPI::ControlSCPI(QObject *parent)
    : QObject{parent}
{

}

void ControlSCPI::connectToSCPI()
{
    if(connectedToSCPI) {
        disconnectFromSCPI();
        return;
    }
    socket = std::make_shared<QTcpSocket>();
    connect(socket.get(), &QTcpSocket::connected, this, &ControlSCPI::onConnected);
    connect(socket.get(), &QTcpSocket::disconnected, this, &ControlSCPI::onDisconnected);
    connect(socket.get(), &QTcpSocket::readyRead, this, &ControlSCPI::onReadyRead);

    socket->connectToHost(this->ip, this->port);
}

void ControlSCPI::disconnectFromSCPI()
{
    socket->write(TURN_OFF.toLocal8Bit()+"\n");
    socket->write(REMOTE_OFF.toLocal8Bit()+"\n");
    socket->disconnectFromHost();
}

void ControlSCPI::sendValues(const QString &curr, const QString &pf)
{
    socket->write((SET_CURR+curr).toLocal8Bit()+"\n");
    socket->write((SET_PF+pf).toLocal8Bit()+"\n");
    socket->write(TURN_ON.toLocal8Bit()+"\n");
}

QString ControlSCPI::getValues(const QString &val)
{
    socket->write(val.toLocal8Bit()+"\n");
    if(socket->waitForReadyRead(15000)){
        return dataArr.trimmed();;
    }
    else{
        qDebug() << "No Data";
        return "No Data";
    }
}

void ControlSCPI::onConnected()
{
    socket->write(REMOTE_ON.toLocal8Bit()+"\n");
    socket->write(MODE_AC.toLocal8Bit()+"\n");
    socket->write(MODE_PF.toLocal8Bit()+"\n");
    emit enableValuesInput(true);
    connectedToSCPI = true;
}

void ControlSCPI::onDisconnected()
{
    connectedToSCPI = false;
    emit enableValuesInput(false);
}

void ControlSCPI::onReadyRead()
{
    qDebug() << "ready read!";
    dataArr = socket->readAll();
    qDebug() << dataArr;
}
