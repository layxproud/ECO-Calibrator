#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    ui->setupUi(this);

    connect(ui->loadButton, &QPushButton::clicked, this, &SettingsWindow::onLoadButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &SettingsWindow::onSaveButtonClicked);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

// Save
void SettingsWindow::onSaveButtonClicked()
{  
    saveSettings();
}

// Load
void SettingsWindow::onLoadButtonClicked()
{   
    loadSettings();
}

void SettingsWindow::loadSettings()
{
    QSettings settings("./config.ini", QSettings::IniFormat, this);
    ui->ipInput->setText(settings.value("ip").toString());
    ui->portInput->setText(settings.value("port").toString());
    ui->regenTimeInput->setValue(settings.value("regentime").toInt());
}

void SettingsWindow::saveSettings()
{
    QSettings settings("./config.ini", QSettings::IniFormat, this);
    settings.setValue("ip", ui->ipInput->text());
    settings.setValue("port", ui->portInput->text());
    settings.setValue("regentime", ui->regenTimeInput->text());
    this->close();
}
