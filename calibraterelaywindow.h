#ifndef CALIBRATERELAYWINDOW_H
#define CALIBRATERELAYWINDOW_H

#include <QDialog>
#include "controldali.h"
#include "jsoncontroller.h"
#include "controlscpi.h"

namespace Ui {
class CalibrateRelayWindow;
}

class CalibrateRelayWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrateRelayWindow(ControlDALI *controlDali, JsonController *jsonController, QWidget *parent = nullptr);
    ~CalibrateRelayWindow();

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::CalibrateRelayWindow *ui;
    ControlDALI *controlDali;
    JsonController *jsonController;
    ControlSCPI *controlSCPI;

    int channel;
    void fillRelayValues();

private slots:
    // СЛОТЫ ВЗАИМОДЕЙСТВИЯМИ С ЭЛЕМЕНТАМИ GUI
    void onConnectButtonClicked();
    void onChannelChanged(int index);
    void onSendValuesButtonLoadClicked();
    void getValuesButtonLoadClicked();

    // СЛОТЫ ДЛЯ ОБЩЕНИЯ МЕЖДУ КЛАССАМИ
    void appendCalibrateValues(QMap <QString, QString> map);
    void enableValuesInput(bool flag);
    void clearValues();
};

#endif // CALIBRATERELAYWINDOW_H
