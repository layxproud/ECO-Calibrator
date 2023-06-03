#include "calibraterelaywindow.h"
#include "ui_calibraterelaywindow.h"

CalibrateRelayWindow::CalibrateRelayWindow(ControlDALI *controlDali, JsonController *jsonController, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrateRelayWindow)
{
    ui->setupUi(this);
    this->controlDali = controlDali;
    this->jsonController = jsonController;
    controlSCPI = new ControlSCPI();

    enableValuesInput(false); // Отключаю кнопки для отправки данных, пока нет подключения

    // СЛОТЫ ДЛЯ ОБЩЕНИЯ С GUI
    connect(ui->connectButton_Load, &QPushButton::clicked, this, &CalibrateRelayWindow::onConnectButtonClicked);
    connect(ui->sendValuesButton_Load, &QPushButton::clicked, this, &CalibrateRelayWindow::onSendValuesButtonLoadClicked);
    connect(ui->getValuesButton_Load, &QPushButton::clicked, this, &CalibrateRelayWindow::getValuesButtonLoadClicked);
    connect(ui->channelComboBox_Relay, SIGNAL(currentIndexChanged(int)), this, SLOT(onChannelChanged(int)));

    // СЛОТЫ ДЛЯ ОБЩЕНИЯ МЕЖДУ КЛАССАМИ
    connect(controlSCPI, &ControlSCPI::enableValuesInput, this, &CalibrateRelayWindow::enableValuesInput);
    connect(jsonController, &JsonController::sendCalibrateValuesToUi, this, &CalibrateRelayWindow::appendCalibrateValues);
    connect(controlDali, &ControlDALI::clearValues, this, &CalibrateRelayWindow::clearValues);
}

CalibrateRelayWindow::~CalibrateRelayWindow()
{
    delete ui;
}

void CalibrateRelayWindow::showEvent(QShowEvent *event)
{
    controlDali->calibrateStart();
    controlDali->connectTimer(false);
    QWidget::showEvent(event);
}

void CalibrateRelayWindow::closeEvent(QCloseEvent *event)
{
    controlDali->calibrateStop();
    controlDali->connectTimer(true);
    if (controlSCPI->connectedToSCPI)
        controlSCPI->disconnectFromSCPI();
    QWidget::closeEvent(event);
}

void CalibrateRelayWindow::fillRelayValues()
{
    ui->voltageInput_Relay->setText(ui->voltageValue_Load->text());
    ui->cfInput_Relay->setText(ui->cfValue_Load->text());
    ui->powerInput_Relay->setText(ui->powerValue_Load->text());
    ui->currentInput_Relay->setText(ui->currentValue_Load->text());
}

void CalibrateRelayWindow::onConnectButtonClicked()
{
    controlSCPI->connectToSCPI();
}

void CalibrateRelayWindow::enableValuesInput(bool flag)
{
    if (flag) {
        ui->currentInput_Load->setReadOnly(false);
        ui->cfInput_Load->setReadOnly(false);
        ui->sendValuesButton_Load->setEnabled(true);
        ui->getValuesButton_Load->setEnabled(true);
        ui->connectionStatus_Load->setText("Подключение установлено");
        ui->connectButton_Load->setText("Отключиться");
    }
    else {
        ui->currentInput_Load->setReadOnly(true);
        ui->cfInput_Load->setReadOnly(true);
        ui->sendValuesButton_Load->setEnabled(false);
        ui->getValuesButton_Load->setEnabled(false);
        ui->connectionStatus_Load->setText("Нет подключения");
        ui->connectButton_Load->setText("Подключиться");
    }
}

void CalibrateRelayWindow::clearValues()
{
    ui->voltageValue_Relay->setText("");
    ui->currentValue_Relay->setText("");
    ui->powerValue_Relay->setText("");
    ui->cfValue_Relay->setText("");
}

void CalibrateRelayWindow::appendCalibrateValues(QMap<QString, QString> map)
{
    if (map.contains("voltage")) {
        ui->voltageValue_Relay->setText(map["voltage"]);
    }

    // НАПРЯЖЕНИЕ
    if (map.contains("current")) {
        ui->currentValue_Relay->setText(map["current"]);
    }

    // МОЩНОСТЬ
    if (map.contains("power")) {
        ui->powerValue_Relay->setText(map["power"]);
    }

    // КОЭФФИЦИЕНТ МОЩНОСТИ
    if (map.contains("cos_f")) {
        ui->cfValue_Relay->setText(map["cos_f"]);
    }
}

void CalibrateRelayWindow::onChannelChanged(int index)
{
    controlDali->changeChannel(index);
}

void CalibrateRelayWindow::onSendValuesButtonLoadClicked()
{
    controlSCPI->sendValues(ui->currentInput_Load->text(), ui->cfInput_Load->text());
}

void CalibrateRelayWindow::getValuesButtonLoadClicked()
{
    ui->currentValue_Load->setText(controlSCPI->getValues(GET_CURR));
    ui->voltageValue_Load->setText(controlSCPI->getValues(GET_VOLT));
    ui->cfValue_Load->setText(controlSCPI->getValues(GET_PF));
    ui->powerValue_Load->setText(controlSCPI->getValues(GET_POW));

    if (ui->autoFillButton->isChecked())
        fillRelayValues();
}


