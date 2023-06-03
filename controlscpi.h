#ifndef CONTROLSCPI_H
#define CONTROLSCPI_H

#include "scpicommands.h"
#include <QObject>
#include <QTcpSocket>
#include <memory>

class ControlSCPI : public QObject
{
    Q_OBJECT
public:
    explicit ControlSCPI(QObject *parent = nullptr);

    void connectToSCPI();
    void disconnectFromSCPI();
    void sendValues(const QString &curr, const QString &pf); // Отправляет на нагрузку ток и коэффициент мощности
    QString getValues(const QString &val);
    bool connectedToSCPI = false;

signals:
    void enableValuesInput(bool flag);
private:
    QString ip = "192.168.001.155";
    int port = 30000;
    std::shared_ptr<QTcpSocket> socket;
    QString dataArr;

private slots:
    void onConnected(); // socket connected to server
    void onDisconnected(); // socket disconnected from server
    void onReadyRead(); // socket recieved message from server

};

#endif // CONTROLSCPI_H
