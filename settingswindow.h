#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    void onSaveButtonClicked();
    void onLoadButtonClicked();

private:
    Ui::SettingsWindow *ui;

    void loadSettings();
    void saveSettings();
};

#endif // SETTINGSWINDOW_H
