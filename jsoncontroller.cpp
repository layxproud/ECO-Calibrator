#include "jsoncontroller.h"

JsonController::JsonController(QObject *parent)
    : QObject{parent}
{

}

QJsonObject JsonController::objectFromString(const QString &in)
{
    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

    if(!doc.isNull()) {
        if(doc.isObject())
            obj = doc.object();
        else
            qDebug() << "Документ не является объектом";
    }
    else
        qDebug() << "Неправильный JSON...\n" << in;

    return obj;
}

void JsonController::doCalibrate(QJsonObject jObj)
{
    QJsonObject ff = jObj["cal_data"].toObject();
    emit sendCalibrateValuesToUi(buildCalibrateTable(ff));
}

void JsonController::doDevices(QJsonObject jObj)
{
    devicesFile = new QFile;
    devicesFile->setFileName("./devicesList.txt");
    devicesFile->open(QIODevice::Append | QIODevice::Text);
    devicesFile->resize(0);

    QStandardItemModel *model = new QStandardItemModel(this);
    // ТАБЛИЦА ДАТЧИКОВ
    if (jObj.contains("devices") && jObj.contains("data") && jObj["data"].toObject().contains("sensors")) {

        QJsonArray devices = QJsonValue(jObj["devices"]).toArray();
        QJsonObject ff = jObj["data"].toObject()["sensors"].toObject();

        for (int i = 0; i < devices.count(); ++i) {
           QString deviceType = (devices.at(i).toObject())["type"].toString();
           QString deviceId = QString::number((devices.at(i).toObject())["id"].toInt());
           buildSensorTable (deviceType, deviceId, model, ff, devicesFile);
        }

    }
    // ТАБЛИЦА РЕЛЕ
    if (jObj.contains("data") && jObj["data"].toObject().contains("relay")) {
        QTextStream out(devicesFile);
        QJsonArray relays = QJsonValue(jObj["data"].toObject()["relay"]).toArray();

        for (int j = 0; j < relays.count(); ++j) {
            QJsonObject ff2 = relays[j].toObject();
            QString relayId = QString::number((relays.at(j).toObject())["id"].toInt());
            QStandardItem *relayItem = new QStandardItem("Модуль реле " + QString::number(j+1) + "\t id = " + relayId);
            out << "Модуль реле " + QString::number(j+1) + "\t id = " + relayId << "\n";
            insertItemToRow(model, relayItem, 1, 4);
            buildRelayTable(model, ff2);
        }
    }
    devicesFile->close();
    emit sendModelToMainUi(model);
    emit sendDataToComboBox();
}

void JsonController::doDeviceInfo(QJsonObject jObj)
{
    QJsonObject ff4 = jObj["dev_info"].toObject();
    QString type = ff4.value("type").toString();
    int version = ff4["version"].toInt();
    int first, second;
    first = ((version >> 8) & 0xFF);
    second = (version & 0xFF);
    qDebug() << first << "." << second;
    QString ver = QString::number(first) + "." + QString::number(second);
    emit sendDevInfo(type, ver);
}

void JsonController::doUpdate(int from)
{
    QJsonObject fw;
    QJsonObject firmware;
    firmware.insert("addr", devID);
    firmware.insert("shift", from);
    QByteArray part = contents.mid(from, CHUNKS);
    if(from+CHUNKS >= contents.length())
        firmware.insert("last", true);
    else
        firmware.insert("last", false);
    firmware.insert("data", QJsonValue::fromVariant(part.toBase64()));
    fw.insert("firmware", firmware);
    lastShift = from + part.length();
    QString strFromObj = QJsonDocument(fw).toJson(QJsonDocument::Compact);
    emit sendJsonToServer(strFromObj);
    emit updateProgressBar(lastShift);
}

void JsonController::checkValidity(QJsonObject jObj)
{
    qDebug() << lastShift;
    if(jObj["firmware"].toObject()["written"] == lastShift && jObj["firmware"].toObject().contains("status") && lastShift != contents.length())
        doUpdate(lastShift);
    else if(jObj["firmware"].toObject()["written"] == contents.length() && type == 'self')
        emit updateFinished();
}

void JsonController::parseJson(const QString &message) {
    QJsonObject jObj = objectFromString(message);

    // ТАБЛИЦА КАЛИБРОВКИ РЕЛЕ
    if (jObj.contains("cal_data")) {
        doCalibrate(jObj);
    }
    else if(jObj.contains("dev_info")) {
        doDeviceInfo(jObj);
    }
    else if(jObj.contains("firmware")) {
        checkValidity(jObj);
    }
    else {
        doDevices(jObj);
    }
}

// СЕНСОРЫ
void JsonController::buildSensorTable(const QString &sensor, const QString &deviceID, QStandardItemModel *model, QJsonObject &ff, QFile* devicesFile)
{
    QTextStream out(devicesFile);
    QString sensorName;
    QList<QStandardItem*> itemList;
    // ДАТЧИК ПЫЛИ
    if (sensor == "sens_dust") {
        sensorName = "Датчик пыли \t id = ";
        for (const auto &str : QStringList()<<"dust_small"<<"dust_big"<<"smoke")
            if (ff.contains(str))
                itemList = createSensorObjects(str, ff);
    }

    // ДАТЧИК ОСАДКОВ
    else if (sensor == "sens_rain") {
        sensorName = "Датчик осадков \t id = ";
        if (ff.contains("rain"))
            itemList = createSensorObjects("rain", ff);
    }

    // ДАТЧИК СРЕДЫ
    else if (sensor == "sens_atm") {
        sensorName = "Датчик среды \t id = ";
        for (const auto &str : QStringList()<<"atm_temp"<<"atm_humidity"<<"atm_pressure")
            if (ff.contains(str))
                itemList = createSensorObjects(str, ff);
    }

    // ДАТЧИК ТЕМПЕРАТУРЫ ПОВЕРХНОСТИ
    else if (sensor == "sens_temp") {
        sensorName = "Датчик температуры поверхности \t id = ";
        if (ff.contains("obj_temp"))
            itemList = createSensorObjects("obj_temp", ff);
    }

    // ДАТЧИК РАДИАЦИИ
    else if (sensor == "sens_rad") {
        sensorName = "Датчик радиации \t id = ";
        if (ff.contains("rad"))
            itemList = createSensorObjects("rad", ff);
    }

    // ДАТЧИК УЛЬТРАФИОЛЕТА
    else if (sensor == "sens_uv") {
        sensorName = "Датчик ультрафиолета \t id = ";
        for (const auto &str : QStringList()<<"uva"<<"uvb"<<"uvi")
            if (ff.contains(str))
                itemList = createSensorObjects(str, ff);
    }

    // ДАТЧИК ШУМА
    else if (sensor == "sens_noise") {
        sensorName = "Датчик шума \t id = ";
        if (ff.contains("noise"))
            itemList = createSensorObjects("noise", ff);
    }

    // ДАТЧИК CO2
    else if (sensor == "sens_co2") {
        sensorName = "Датчик газа CO2 \t id = ";
        if (ff.contains("co2"))
            itemList = createSensorObjects("co2", ff);
    }

    // ДАТЧИК CO
    else if (sensor == "sens_co") {
        sensorName = "Датчик газа CO \t id = ";
        if (ff.contains("co"))
            itemList = createSensorObjects("co", ff);
    }

    // ДАТЧИК СН4
    else if (sensor == "sens_ch4") {
        sensorName = "Датчик газа СН4 \t id = ";
        if (ff.contains("ch4"))
            itemList = createSensorObjects("ch4", ff);
    }

    // ДАТЧИК H2S
    else if (sensor == "sens_h2s") {
        sensorName = "Датчик газа H2S \t id = ";
        if (ff.contains("h2s"))
            itemList = createSensorObjects("h2s", ff);
    }

    // ДАТЧИК С3H8
    else if (sensor == "sens_c3h8") {
        sensorName = "Датчик газа С3H8 \t id = ";
        if (ff.contains("c3h8"))
            itemList = createSensorObjects("c3h8", ff);
    }

    // ДАТЧИК C4H10
    else if (sensor == "sens_c4h10") {
        sensorName = "Датчик газа C4H10 \t id = ";
        if (ff.contains("c4h10"))
            itemList = createSensorObjects("c4h10", ff);
    }

    // ДАТЧИК NO
    else if (sensor == "sens_no") {
        sensorName = "Датчик газа NO \t id = ";
        if (ff.contains("no"))
            itemList = createSensorObjects("no", ff);
    }

    // ДАТЧИК NO2
    else if (sensor == "sens_no2") {
        sensorName = "Датчик газа NO2 \t id = ";
        if (ff.contains("no2"))
            itemList = createSensorObjects("no2", ff);
    }

    // ДАТЧИК O3
    else if (sensor == "sens_o3") {
        sensorName = "Датчик газа O3 \t id = ";
        if (ff.contains("o3"))
            itemList = createSensorObjects("o3", ff);
    }

    // ДАТЧИК ОГНЕОПАСНЫХ ГАЗОВ
    else if (sensor == "sens_lpg") {
        sensorName = "Датчик огнеопасных газов \t id = ";
        if (ff.contains("lpg"))
            itemList = createSensorObjects("lpg", ff);
    }

    else if (sensor == "dev_rel")
        return;

    out << sensorName + deviceID << "\n";
    sensorItem = new QStandardItem(sensorName + deviceID);
    insertDeviceAndIndication(model, sensorItem, itemList);
}

void JsonController::buildRelayTable(QStandardItemModel *model, QJsonObject &ff)
{
    // ЧАСТОТА
    if (ff.contains("frequency")) {
        indicationItem = new QStandardItem("Частота");
        valueItem = new QStandardItem(QString::number(ff["frequency"].toDouble()) + " Гц");
        insertListToRow(model, QList<QStandardItem*>()<<indicationItem<<valueItem, 1, 2);
    }

    // ТЕМПЕРАТУРА ЯДРА
    if (ff.contains("core_temp")) {
        indicationItem = new QStandardItem("Температура ядра");
        valueItem = new QStandardItem(QString::number(ff["core_temp"].toDouble()) + " C");
        insertListToRow(model, QList<QStandardItem*>()<<indicationItem<<valueItem, 1, 2);
    }

    // КАНАЛЫ
    if (ff.contains("num_channels")) {
        QList<QStandardItem*> channelItems;
        QStandardItem *channelHeader = new QStandardItem("Каналы");
        channelHeader->setTextAlignment(Qt::AlignCenter);
        channelItems.append(channelHeader);
        int num_channels = ff["num_channels"].toInt();
        for (int k = 0; k < num_channels; ++k) {
            QStandardItem *channelItem = new QStandardItem("Канал " + QString::number(k+1));
            channelItem->setTextAlignment(Qt::AlignCenter);
            channelItems.append(channelItem);
        }
        model->appendRow(channelItems);
    }

    // ПРОЧИЕ ХАРАКТЕРИСТИКИ
    QStringList list;
    list<<"status"<<"voltage"<<"current"<<"energy"<<"power"<<"cos_f"<<"energy_curr";
    for (const auto &str : list) {
        if (ff.contains(str)) {
            createRelayObjects(str, model, ff);
        }
    }
}

QMap<QString, QString> JsonController::buildCalibrateTable(QJsonObject &ff)
{
    // НАПРЯЖЕНИЕ
    if (ff.contains("voltage")) {
        relayValues.insert("voltage", QString::number(ff["voltage"].toDouble(), 'f'));
    }

    // НАПРЯЖЕНИЕ
    if (ff.contains("current")) {
        relayValues.insert("current", QString::number(ff["current"].toDouble(), 'f'));
    }

    // МОЩНОСТЬ
    if (ff.contains("power")) {
        relayValues.insert("power", QString::number(ff["power"].toDouble(), 'f'));
    }

    // КОЭФФИЦИЕНТ МОЩНОСТИ
    if (ff.contains("cos_f")) {
        relayValues.insert("cos_f", QString::number(ff["cos_f"].toDouble(), 'f'));
    }
    return relayValues;
}

// ЗНАЧЕНИЯ СЕНСОРОВ
QList<QStandardItem*> JsonController::createSensorObjects(const QString &indication, QJsonObject &ff)
{
    QString indicationName;
    QString valueName;
    // ДАТЧИК ПЫЛИ
    if (indication == "dust_small") {
        indicationName = "Маленьких частиц пыли";
        valueName = QString::number(ff["smoke"].toDouble()) + " на 1см3";
    }
    else if (indication == "dust_big") {
        indicationName = "Больших частиц пыли";
        valueName = QString::number(ff["smoke"].toDouble()) + " на 1см3";
    }
    else if (indication == "smoke") {
        indicationName = "Дым";
        valueName = QString::number(ff["smoke"].toDouble()) + " на 1см3";
    }

    // ДАТЧИК ОСАДКОВ
    else if (indication == "rain") {
        if (ff["rain"].toInt() == 0) {
            indicationName = "Осадки";
            valueName = "Нет осадков";
        }
        else if (ff["rain"].toInt() == 1) {
            indicationName = "Осадки";
            valueName = "Слабые осадки";
        }
        else if (ff["rain"].toInt() == 2) {
            indicationName = "Осадки";
            valueName = "Средние осадки";
        }
        else if (ff["rain"].toInt() == 3) {
            indicationName = "Осадки";
            valueName = "Сильные осадки";
        }
    }

    // ДАТЧИК СРЕДЫ
    else if (indication == "atm_temp") {
        indicationName = "Температура среды";
        valueName = QString::number(ff["atm_temp"].toDouble()) + " С";
    }
    else if (indication == "atm_humidity") {
        indicationName = "Влажность";
        valueName = QString::number(ff["atm_humidity"].toDouble()) + "%";
    }
    else if (indication == "atm_pressure") {
        indicationName = "Давление";
        valueName = QString::number(ff["atm_pressure"].toDouble()) + " мм рт ст";
    }

    // ДАТЧИК ТЕМПЕРАТУРЫ ПОВЕРХНОСТИ
    else if (indication == "obj_temp") {
        indicationName = "Температура объекта";
        valueName = QString::number(ff["obj_temp"].toDouble()) + " С";
    }

    // ДАТЧИК РАДИАЦИИ
    else if (indication == "rad") {
        indicationName = "Радиация";
        valueName = QString::number(ff["rad"].toDouble()) + " мкР/ч";
    }

    // ДАТЧИК УЛЬТРАФИОЛЕТА
    else if (indication == "uva") {
        indicationName = "Ультрафиолет-А";
        valueName = QString::number(ff["uva"].toDouble()) + " УФ-А";
    }
    else if (indication == "uvb") {
        indicationName = "Ультрафиолет-Б";
        valueName = QString::number(ff["uvb"].toDouble()) + " УФ-Б";
    }
    else if (indication == "uvi") {
        indicationName = "УФ Индекс";
        valueName = QString::number(ff["uvi"].toDouble()) + " УФ-Индекс";
    }

    // ДАТЧИК ШУМА
    else if (indication == "noise") {
        indicationName = "Шум";
        valueName = QString::number(ff["noise"].toDouble()) + " дБ";
    }

    // ДАТЧИК CO2
    else if (indication == "co2") {
        indicationName = "Концентрация";
        valueName = QString::number(ff["co2"].toDouble()) + " на миллион";
    }

    // ДАТЧИК CO
    else if (indication == "co") {
        indicationName = "Концентрация";
        valueName = QString::number(ff["co"].toDouble()) + " на миллион";
    }

    // ДАТЧИК CH4
    else if (indication == "ch4") {
        indicationName = "Концентрация";
        valueName = QString::number(ff["ch4"].toDouble()) + " на миллион";
    }

    // ДАТЧИК H2S
    else if (indication == "h2s") {
        indicationName = "Концентрация";
        valueName = QString::number(ff["h2s"].toDouble()) + " на миллион";
    }

    // ДАТЧИК C3H8
    else if (indication == "c3h8") {
        indicationName = "Концентрация";
        valueName = QString::number(ff["c3h8"].toDouble()) + " на миллион";
    }

    // ДАТЧИК C4H10
    else if (indication == "c4h10") {
        indicationName = "Концентрация";
        valueName = QString::number(ff["c4h10"].toDouble()) + " на миллион";
    }

    // ДАТЧИК NO
    else if (indication == "no") {
        indicationName = "Концентрация";
        valueName = QString::number(ff["no"].toDouble()) + " на миллион";
    }

    // ДАТЧИК NO2
    else if (indication == "no2") {
        indicationName = "Концентрация";
        valueName = QString::number(ff["no2"].toDouble()) + " на миллион";
    }

    // ДАТЧИК O3
    else if (indication == "o3") {
        indicationName = "Концентрация";
        valueName = QString::number(ff["o3"].toDouble()) + " на миллион";
    }

    // ДАТЧИК ОГНЕОПАСНЫХ ГАЗОВ
    else if (indication == "lpg") {
        indicationName = "Концентрация";
        valueName = QString::number(ff["lpg"].toDouble()) + " на миллион";
    }

    indicationItem = new QStandardItem(indicationName);
    valueItem = new QStandardItem(valueName);
    QList<QStandardItem*> list;
    list << indicationItem << valueItem;
    return list;
}

// ЗНАЧЕНИЯ РЕЛЕ
void JsonController::createRelayObjects(const QString &indication, QStandardItemModel *model, QJsonObject &ff) {
    QJsonArray relayArray = ff[indication].toArray();
    QList<QStandardItem*> relayItems;
    QString headerName;
    QString relayItemName;

    if (indication == "status")
        headerName = "Статус";
    else if (indication == "voltage")
        headerName = "Напряжение";
    else if (indication == "current")
        headerName = "Ток";
    else if (indication == "energy")
        headerName = "Энергия суммарная";
    else if (indication == "power")
        headerName = "Мощность";
    else if (indication == "cos_f")
        headerName = "Коэффициент мощности";
    else if (indication == "energy_curr")
        headerName = "Энергия сессии";
    headerItem = new QStandardItem(headerName);
    headerItem->setTextAlignment(Qt::AlignCenter); 
    relayItems.append(headerItem);

    for (int k = 0; k < relayArray.count(); ++k) {
        if (indication == "status") {
            if (relayArray[k] == 0)
                relayItemName = "Откл";
            else if (relayArray[k] == 1)
                relayItemName = "Ошибка";
            else if (relayArray[k] == 2)
                relayItemName = "Вкл";
            else if (relayArray[k] == 3)
                relayItemName = "Вкл-нагр";
        }
        else if (indication == "voltage")
            relayItemName = QString::number(relayArray[k].toDouble()) + " В";
        else if (indication == "current")
            relayItemName = QString::number(relayArray[k].toDouble()) + " А";
        else if (indication == "energy")
            relayItemName = QString::number(relayArray[k].toDouble()) + " Вт/ч";
        else if (indication == "power")
            relayItemName = QString::number(relayArray[k].toDouble()) + " Вт";
        else if (indication == "cos_f")
            relayItemName = QString::number(relayArray[k].toDouble());
        else if (indication == "energy_curr")
            relayItemName = QString::number(relayArray[k].toDouble()) + " Вт/ч";
        relayItem = new QStandardItem(relayItemName);
        relayItem->setTextAlignment(Qt::AlignCenter);
        relayItems.append(relayItem);
    }
    model->appendRow(relayItems);
}

// СОЗДАТЬ РЯД ИЗ ОДНОГО АЙТЕМА
void JsonController::insertItemToRow(QStandardItemModel *model, QStandardItem *item, int row, int column)
{
    model->appendRow(item);
    item->setTextAlignment(Qt::AlignCenter);
    QModelIndex index = item->index();
    emit askUiToSetSpan(index, row, column);
}

// СОЗДАТЬ РЯД ИЗ ДВУХ АЙТЕМОВ
void JsonController::insertListToRow(QStandardItemModel *model, QList<QStandardItem*> list, int row, int column)
{
    QStandardItem* item_1 = list.at(0);
    item_1->setTextAlignment(Qt::AlignCenter);
    QStandardItem* item_2 = list.at(1);
    item_2->setTextAlignment(Qt::AlignCenter);
    model->appendRow(QList<QStandardItem*>()<<item_1<<nullptr<<item_2);

    QModelIndex index1 = item_1->index();
    emit askUiToSetSpan(index1, row, column);
    QModelIndex index2 = item_2->index();
    emit askUiToSetSpan(index2, row, column);
}

void JsonController::insertDeviceAndIndication(QStandardItemModel *model, QStandardItem *sensorItem, QList<QStandardItem*> list)
{
    insertItemToRow(model, sensorItem, 1, 4);
    insertListToRow(model, list, 1, 2);
}
