#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTextCodec>

class Logger : public QObject
{
    Q_OBJECT
public:
    Logger(QObject *parent = nullptr);
    ~Logger();
    void printMessageToLog(const QString &text);

    bool logToFile = true; // Флаг записи лога в файл
signals:
    void sendMessageToUi(const QString &message);
private:
    QFile *logFile = Q_NULLPTR;
};

#endif // LOGGER_H
