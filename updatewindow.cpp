#include "updatewindow.h"
#include "ui_updatewindow.h"
#include <QFileDialog>
#include <QMessageBox>

UpdateWindow::UpdateWindow(ControlDALI *controlDali, JsonController *jsonController, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateWindow)
{
    ui->setupUi(this);
    this->controlDali = controlDali;
    this->jsonController = jsonController;

    ui->updateDeviceButton->setEnabled(false);

    connect(ui->getDataButton, &QPushButton::clicked, this, &UpdateWindow::onGetDataButtonClicked);
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &UpdateWindow::selectItem);
    connect(ui->chooseFileButton, &QPushButton::clicked, this, &UpdateWindow::openFile);
    connect(ui->updateDeviceButton, &QPushButton::clicked, this, &UpdateWindow::update);

    // СЛОТЫ ДЛЯ ОБЩЕНИЯ МЕЖДУ КЛАССАМИ
    connect(jsonController, &JsonController::sendDataToComboBox, this, &UpdateWindow::updateComboBox);
    connect(jsonController, &JsonController::sendDevInfo, this, &UpdateWindow::getDevInfo);
    connect(jsonController, &JsonController::updateProgressBar, this, &UpdateWindow::displayProgress);

}

UpdateWindow::~UpdateWindow()
{
    delete ui;
}

void UpdateWindow::showEvent(QShowEvent *event)
{
    controlDali->connectTimer(false);
    QWidget::showEvent(event);
}

void UpdateWindow::closeEvent(QCloseEvent *event)
{
    controlDali->connectTimer(true);
    QWidget::closeEvent(event);
}

QString UpdateWindow::getDeviceName(const QString &type)
{
    jsonController->type = type;
    if (type == "sens_dust")
        return "Датчик пыли";

    else if (type == "sens_rain")
        return "Датчик осадков";

    else if (type == "sens_atm")
        return "Датчик среды";

    else if (type == "sens_temp")
        return "Датчик температуры поверхности";

    else if (type == "sens_rad")
        return "Датчик радиации";

    else if (type == "sens_uv")
        return "Датчик Ультрафиолета";

    else if (type == "sens_noise")
        return "Датчик шума";

    else if (type == "sens_co2" || type == "sens_co" || type == "sens_ch4" || type == "sens_h2s"
            || type == "sens_c3h8" || type == "sens_c4h10" || type == "sens_no" || type == "sens_no2" || type == "sens_o3")
        return "Датчик Газа";

    else if (type == "sens_lpg")
        return "Датчик Огнеопасных газов";

    else if (type == "dev_rel")
        return "Модуль реле";

    else if (type == "self")
        return "DALIGate";
    else
        return "Неизвестное устройство";
}

QString UpdateWindow::getID(const QString &text)
{
    QStringList list = text.split(" ");
    return list.last();
}

void UpdateWindow::onGetDataButtonClicked()
{
    controlDali->regenStatus = false;
    controlDali->getData();
    controlDali->regenStatus = true;
}

void UpdateWindow::updateComboBox()
{
    ui->comboBox->clear();
    ui->comboBox->addItem("Основной модуль");
    QFile file("./devicesList.txt");
    if (!file.open(QFile::ReadOnly | QFile::Text))
        QMessageBox::warning(this, "Внимание", "Не удалось открыть файл");
    QTextStream in(&file);
    QString text = in.readAll();
    file.close();
    int i = 0;
    while(i < text.size()) {
        QString buff = "";
        while(i < text.size() && text[i] != '\n') {
            buff.append(text[i]);
            i++;
        }
        if(buff != "")
            ui->comboBox->addItem(buff);
        i++;
    }
}

void UpdateWindow::selectItem()
{
    if (ui->comboBox->currentText() == "")
        return;
    if (ui->comboBox->currentIndex() == 0)
        controlDali->getDevInfo(DALI_ID);
    else {
        controlDali->getDevInfo(getID(ui->comboBox->currentText()));
        jsonController->devID = getID(ui->comboBox->currentText());
    }
}

void UpdateWindow::getDevInfo(const QString &type, const QString &version)
{
    ui->currentDevice->setText("Обновление устройства " + getDeviceName(type) + " (версия: " + version + ")");
}

void UpdateWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл прошивки", QDir::currentPath(), "Все файлы (*.*)");
    if (!fileName.isNull()) {
        ui->fileName->setText(fileName);
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly))
            QMessageBox::warning(this, "Внимание", "Не удалось открыть файл");
        QByteArray raw_data = file.readAll();
        jsonController->contents = raw_data;
        file.flush();
        file.close();
        ui->updateDeviceButton->setEnabled(true);
    }
    else
        return;
}

void UpdateWindow::update()
{
    jsonController->doUpdate(0);
}

void UpdateWindow::displayProgress(int progress)
{
    ui->progressBar->setValue(progress);
}
