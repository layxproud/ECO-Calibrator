#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include "jsoncontroller.h"
#include "logger.h"
#include "controldali.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // СЛОТЫ ВЗАИМОДЕЙСТВИЯМИ С ЭЛЕМЕНТАМИ GUI
    void onConnectButtonClicked(); // нажата кнопка "Подключиться/Отключиться"
    void onGetDataButtonClicked(); // нажата кнопка "Старт/Стоп"
    void onSettingsButtonClicked(); // нажата кнопка "Настройки"
    void onCalibrateButtonClicked(); // нажата кнопка "Калибровка"
    void onRegenStatusChanged(int arg1); // изменен флаг автообновления
    void onLogStatusChanged(int arg1); // изменен флаг записи в логфайл
    void onUpdateButtonClicked();

    // СЛОТЫ ДЛЯ ОБЩЕНИЯ МЕЖДУ КЛАССАМИ
    void appendMessageToLogWindow(const QString &message); // добавляет сообщение в окно лога
    void appendModelToTableView(QStandardItemModel *model); // добавляет model в tableView
    void changeTableViewSpan(QModelIndex index, int row, int column); // меняет параметры ряда в таблице tableView
    void changeButtonName(const QString &buttonName, const QString &text); // меняет текст на кнопке buttonName
    void enableButtons(bool flag); // включает/выключает кнопки "Подключиться" и "Калибровка"

private:
    Ui::MainWindow *ui;
    Logger *logger;
    JsonController *jsonController; // json data controller
    ControlDALI *controlDali;
};
#endif // MAINWINDOW_H
