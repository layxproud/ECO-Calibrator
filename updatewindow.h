#ifndef UPDATEWINDOW_H
#define UPDATEWINDOW_H

#include <QDialog>
#include "controldali.h"
#include "jsoncontroller.h"

namespace Ui {
class UpdateWindow;
}

class UpdateWindow : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateWindow(ControlDALI *controlDali, JsonController *jsonController, QWidget *parent = nullptr);
    ~UpdateWindow();

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::UpdateWindow *ui;
    ControlDALI *controlDali;
    JsonController *jsonController;
    QString getDeviceName(const QString &type);
    QString getID(const QString &text);

private slots:
    void onGetDataButtonClicked();
    void updateComboBox();
    void selectItem();
    void getDevInfo(const QString &type, const QString &version);
    void openFile();
    void update();
    void displayProgress(int progress);
};

#endif // UPDATEWINDOW_H
