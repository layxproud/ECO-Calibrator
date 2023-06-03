#ifndef JSONCONTROLLER_H
#define JSONCONTROLLER_H

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItem>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

#define DALI_ID "255"
#define CHUNKS 512

class JsonController : public QObject
{
    Q_OBJECT
public:
    JsonController(QObject *parent = nullptr);
    void parseJson(const QString &message);
    void doUpdate(int from);

    int lastShift; // Последнее смещение по файлу прошивки
    QByteArray contents; // Содержимое файла прошивки
    QString devID = DALI_ID;
    QString type;
signals:
    void sendModelToMainUi(QStandardItemModel *model);
    void sendDataToComboBox();
    void sendDevInfo(const QString &type, const QString &version);
    void sendCalibrateValuesToUi(QMap <QString, QString> map);
    void askUiToSetSpan(QModelIndex index, int row, int column);
    void sendJsonToServer(const QString &strFromObj);
    void updateProgressBar(int lastShift);
    void updateFinished();
private:
    QStandardItem *sensorItem;
    QStandardItem *indicationItem;
    QStandardItem *valueItem;
    QStandardItem *headerItem;
    QStandardItem *relayItem;
    QMap <QString, QString> relayValues;
    QFile *devicesFile = Q_NULLPTR;

    QJsonObject objectFromString(const QString &in);

    void doCalibrate(QJsonObject jObj);
    void doDevices(QJsonObject jObj);
    void doDeviceInfo(QJsonObject jObj);
    void checkValidity(QJsonObject jObj);

    void buildSensorTable(const QString &indication, const QString &deviceID, QStandardItemModel *model, QJsonObject &ff, QFile *devicesFile); // Строит таблицу сенсоров
    void buildRelayTable(QStandardItemModel *model, QJsonObject &ff); // Строит таблицу реле
    QMap<QString, QString> buildCalibrateTable(QJsonObject &ff); // Строит таблицу значений реле для калибровки

    void insertItemToRow(QStandardItemModel *model, QStandardItem *item, int row, int column); // Создает ряд из одного айтема
    void insertListToRow(QStandardItemModel *model, QList<QStandardItem*> list, int row, int column); // Создает ряд из двух айтемов, хранящихся в списке
    void insertDeviceAndIndication(QStandardItemModel *model, QStandardItem *item, QList<QStandardItem*> list);

    QList<QStandardItem*> createSensorObjects(const QString &indication, QJsonObject &ff);
    void createRelayObjects(const QString &indication, QStandardItemModel *model, QJsonObject &ff);
};

#endif // JSONCONTROLLER_H
