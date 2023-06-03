#include "logger.h"

Logger::Logger(QObject *parent)
    : QObject{parent}
{
    logFile = new QFile;
    logFile->setFileName("./DALIGateLog.log");
    logFile->open(QIODevice::Append | QIODevice::Text);
    logFile->resize(0);
}

Logger::~Logger()
{
    if (logFile != Q_NULLPTR) {
        logFile->close();
        delete logFile;
    }
}

void Logger::printMessageToLog(const QString &text)
{
    QString message = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss ") + " | " + text;
    emit sendMessageToUi(message);
    if (logToFile) {
        QTextStream out(logFile);
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);
        out.setCodec("UTF-8");
        out << message << "\n";
    }
}
