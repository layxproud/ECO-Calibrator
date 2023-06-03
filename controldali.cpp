#include "controldali.h"

ControlDALI::ControlDALI(Logger *logger, JsonController *jsonController, QObject *parent)
    : QObject{parent}
{
    this->logger = logger;
    this->jsonController = jsonController;
    connect(jsonController, &JsonController::sendJsonToServer, this, &ControlDALI::sendUpdate);
    connect(jsonController, &JsonController::updateFinished, this, &ControlDALI::updateFinished);

    QSettings settings("./config.ini", QSettings::IniFormat, this);
    ip = settings.value("ip", "192.168.1.220").toString();
    port = settings.value("port", 1337).toInt();
    regenTime = settings.value("regentime", 5).toInt();
}

void ControlDALI::connectToDALIServer()
{
    // CONNECT
    if(!connectedToHost) {
        logger->printMessageToLog("Идет подключение к серверу...");
        socket = std::make_shared<QWebSocket>();
        connect(socket.get(), SIGNAL(connected()), this, SLOT(onConnected()));
        connect(socket.get(), SIGNAL(disconnected()), this, SLOT(onDisconnected()));
        connect(socket.get(), SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageRecieved(QString)));

        QString url = "ws://" + ip + ":" + QString::number(port);
        socket->open(QUrl(url));
    }
    // DISCONNECT
    else {
        logger->printMessageToLog("Отключен от сервера.");
        socket->close();
    }
}

// CONNECTED TO SERVER
void ControlDALI::onConnected()
{
    connectedToHost = true;
    logger->printMessageToLog("Подключение установлено.");
    emit changeButtonName("connectButton", "Отключиться");
    emit enableButtons(true);

    timer = new QTimer();
    timerRunning = false;
    connect(timer, &QTimer::timeout, this, &ControlDALI::sendData);
}

// DISCONNECTED FROM SERVER
void ControlDALI::onDisconnected()
{
    connectedToHost = false;
    sendingData = false;
    logger->printMessageToLog("Подключение разорвано.");
    emit changeButtonName("connectButton", "Подключиться");
    emit enableButtons(false);

    if (timerRunning) {
        timer->stop();
        timerRunning = false;
        delete timer;
    }
    emit changeButtonName("getDataButton", "Старт");
}

// RECIEVED MESSAGE FROM SERVER
void ControlDALI::onTextMessageRecieved(const QString &message)
{
    if (message == "error")
        logger->printMessageToLog("Ошибка!");

    else if (message == "building")
        logger->printMessageToLog("Производится сборка устройств на линии DALI...");

    else if (message == "reset_ok")
        logger->printMessageToLog("Перезагрузка устройства...");

    else if (message == "no_devices")
        logger->printMessageToLog("Устройства не найдены.");

    else if (message[0] == '{') {
        logger->printMessageToLog("Получена JSON строка:\n" + message);
        jsonController->parseJson(message);
    }
    else if (message == "rel_calib_start_ok")
        logger->printMessageToLog("Калибровка началась");
    else if (message == "rel_calibrate_stop_ok")
        logger->printMessageToLog("Калибровка завершена");
    else if (message == "rel_calibrate_channel_set")
        logger->printMessageToLog("Включен канал " + QString::number(channel));
    else
        logger->printMessageToLog("Необработанный случай:\n" + message);
}

// SEND MESSAGE TO SERVER
void ControlDALI::sendMessage(const QString &message)
{
    socket->sendTextMessage(message);
    logger->printMessageToLog("Отправлено сообщение '" + message + "' на сервер");
}

void ControlDALI::sendData()
{
    sendMessage("data");
}

void ControlDALI::sendCalibrate()
{
    sendMessage("rel_calibrate_get_data");
}

void ControlDALI::sendUpdate(const QString &message)
{
    sendMessage(message);
}

void ControlDALI::updateFinished()
{
    logger->printMessageToLog("Отключен от сервера.");
    socket->close();
}

void ControlDALI::getData()
{
    // НЕТ ПОДКЛЮЧЕНИЯ
    if (!connectedToHost) {
        logger->printMessageToLog("Нет подключения!");
        return;
    }
    // НЕ ВКЛЮЧЕНО АВТООБНОВЛЕНИЕ, ПОЛУЧИТЬ ДАННЫЕ 1 РАЗ
    if (!regenStatus) {
        sendMessage("data");
        return;
    }
    // ЗАПУСТИТЬ ПОЛУЧЕНИЕ ДАННЫХ
    if(!sendingData) {
        timer->start(regenTime * 1000);
        timerRunning = true;
        sendingData = true;
        sendMessage("data");
        emit changeButtonName("getDataButton", "Стоп");
    }
    // ПРЕКРАТИТЬ ПОЛУЧЕНИЕ ДАННЫХ
    else {
        timer->stop();
        timerRunning = false;
        sendingData = false;
        emit changeButtonName("getDataButton", "Старт");
    }
}

void ControlDALI::getDevInfo(const QString &id)
{
    if (!connectedToHost) {
        logger->printMessageToLog("Нет подключения!");
        return;
    }
    else {
        sendMessage("get_type_" + id);
    }
}

void ControlDALI::connectTimer(bool flag)
{
    if (flag) {
        connect(timer, &QTimer::timeout, this, &ControlDALI::sendData);
        return;
    }

    if (timerRunning) {
        timer->stop();
    }
    disconnect(timer, &QTimer::timeout, this, &ControlDALI::sendData);
    timerRunning = false;
    sendingData = false;

    emit changeButtonName("getDataButton" , "Старт");
}

void ControlDALI::calibrateStart()
{
    sendMessage("rel_calibrate_start_0");
    calibTimer = new QTimer();
    connect(calibTimer, &QTimer::timeout, this, &ControlDALI::sendCalibrate);
    calibTimer->start(regenTime * 1000);
    calibrating = true;
}

void ControlDALI::calibrateStop()
{
    sendMessage("rel_calibrate_stop_0"); // хардкод айди текущей реле
    if(calibrating) {
        disconnect(calibTimer, &QTimer::timeout, this, &ControlDALI::sendCalibrate);
        delete calibTimer;
    }
    calibrating = false;
}

void ControlDALI::changeChannel(int channel)
{
    this->channel = channel - 1;
    if (channel == 0) {
        qDebug() << "Channels turned off";
        sendMessage("rel_calibrate_channel_128");
        emit clearValues();
        return;
    }
    qDebug() << "Channel changed to " << this->channel;
    sendMessage("rel_calibrate_channel_" + QString::number(this->channel));
}


