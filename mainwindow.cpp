#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingswindow.h"
#include "calibraterelaywindow.h"
#include "updatewindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    logger = new Logger();
    jsonController = new JsonController();
    controlDali = new ControlDALI(logger, jsonController);
    enableButtons(false);

    // СЛОТЫ ДЛЯ ОБЩЕНИЯ С GUI
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(ui->getDataButton, &QPushButton::clicked, this, &MainWindow::onGetDataButtonClicked);
    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);
    connect(ui->regenStatus, &QCheckBox::stateChanged, this, &MainWindow::onRegenStatusChanged);
    connect(ui->logStatus, &QCheckBox::stateChanged, this, &MainWindow::onLogStatusChanged);
    connect(ui->calibrateButton, &QPushButton::clicked, this, &MainWindow::onCalibrateButtonClicked);
    connect(ui->updateButton, &QPushButton::clicked, this, &MainWindow::onUpdateButtonClicked);

    // СЛОТЫ ДЛЯ ОБЩЕНИЯ МЕЖДУ КЛАССАМИ
    connect(logger, &Logger::sendMessageToUi, this, &MainWindow::appendMessageToLogWindow);
    connect(jsonController, &JsonController::sendModelToMainUi, this, &MainWindow::appendModelToTableView);
    connect(jsonController, &JsonController::askUiToSetSpan, this, &MainWindow::changeTableViewSpan);
    connect(controlDali, &ControlDALI::changeButtonName, this, &MainWindow::changeButtonName);
    connect(controlDali, &ControlDALI::enableButtons, this, &MainWindow::enableButtons);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// PRESSING BUTTON "CONNECT"/"DISCONNECT"
void MainWindow::onConnectButtonClicked()
{
    controlDali->connectToDALIServer();
}

// PRESSING BUTTON "START"/"STOP"
void MainWindow::onGetDataButtonClicked()
{
    controlDali->getData();
}

// PRESSING BUTTON "SETTINGS"
void MainWindow::onSettingsButtonClicked()
{
    SettingsWindow settingsWindow;
    settingsWindow.setModal(true);
    settingsWindow.exec();
}

void MainWindow::onCalibrateButtonClicked()
{
    CalibrateRelayWindow calibrateWindow(controlDali, jsonController, this);
    calibrateWindow.setModal(true);
    calibrateWindow.exec();
}

// CHANGING REGEN STATUS
void MainWindow::onRegenStatusChanged(int arg1)
{
    if (arg1 == 0) {
        controlDali->regenStatus = false;
        controlDali->connectTimer(false);
    }
    else if (arg1 == 2) {
        controlDali->regenStatus = true;
        controlDali->connectTimer(true);
    }
}

// CHANGING WRITE TO LOGFILE STATUS
void MainWindow::onLogStatusChanged(int arg1)
{
    if (arg1 == 0) {
        logger->logToFile = false;
    }
    else if (arg1 == 2) {
        logger->logToFile = true;
    }
}

void MainWindow::onUpdateButtonClicked()
{
    UpdateWindow updateWindow(controlDali, jsonController, this);
    updateWindow.setModal(true);
    updateWindow.exec();
}

void MainWindow::appendMessageToLogWindow(const QString &message)
{
    ui->logWindow->append(message);
}

void MainWindow::appendModelToTableView(QStandardItemModel *model)
{
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::changeTableViewSpan(QModelIndex index, int row, int column)
{
    ui->tableView->setSpan(index.row(), index.column(), row, column);
}

void MainWindow::changeButtonName(const QString &buttonName, const QString &text)
{
    auto btn = findChild<QPushButton*>(buttonName);
      if(btn)
          btn->setText(text);
}

void MainWindow::enableButtons(bool flag)
{
    ui->calibrateButton->setEnabled(flag);
    ui->getDataButton->setEnabled(flag);
    ui->updateButton->setEnabled(flag);
}

