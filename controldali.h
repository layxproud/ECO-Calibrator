#ifndef CONTROLDALI_H
#define CONTROLDALI_H

#include <QObject>
#include <QTimer>
#include <QWebSocket>
#include <QSettings>
#include "logger.h"
#include "jsoncontroller.h"

class ControlDALI : public QObject
{
    Q_OBJECT
public:
    explicit ControlDALI(Logger *logger, JsonController *jsonController, QObject *parent = nullptr);
    void connectToDALIServer();
    void getData();
    void getDevInfo(const QString &id);
    void connectTimer(bool flag);
    void changeChannel(int channel);
    void calibrateStart();
    void calibrateStop();
    bool regenStatus = true;

signals:
    void changeButtonName(const QString &buttonName, const QString &text); // просит поменять надпись на кнопке buttonName
    void enableButtons(bool flag);
    void clearValues();

private:
    Logger *logger;
    JsonController *jsonController;
    QTimer *timer;
    QTimer *calibTimer;
    std::shared_ptr<QWebSocket> socket;

    QString ip; // server ip
    int port; // server port
    int regenTime; // interval of messages to server
    int channel; // Выбранный для калибровки канал

    bool connectedToHost = false; // connection flag
    bool sendingData = false; // sending data flag
    bool timerRunning = false; // timer running flag
    bool calibrating = false; // Флаг ведения калибровки

    void sendMessage(const QString &message); // sends message to server

private slots:
    void onConnected(); // socket connected to server
    void onDisconnected(); // socket disconnected from server
    void onTextMessageRecieved(const QString &message); // socket recieved message from server
    void sendData();
    void sendCalibrate();
    void sendUpdate(const QString &message);
    void updateFinished();
};

#endif // CONTROLDALI_H
