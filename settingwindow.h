#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>
#include "application.h"
#include <DMainWindow>
class QSystemTrayIcon;
class QMenu;
class historyWidget;

namespace Ui {
class settingWindow;
}

class settingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit settingWindow(QWidget *parent = nullptr, DMainWindow *mainWindow = nullptr);
    ~settingWindow();

    void readSettings();



    QString getCurrentPath();

    int getCurrentNumber();

    int isAutoStart();


    void setScreenMode(const QString &arg);
private slots:

    void saveSettings();

    void on_pathBtn_clicked();

    void on_setBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pauseBtn_clicked();

    void on_stopBtn_clicked();

    void on_Slider_valueChanged(int value);

    void on_startBtn_clicked();

    void on_startScreen_clicked();

    void on_autoStart_clicked();

    void on_noAutoStart_clicked();

    void on_autoStartBox_stateChanged(int arg1);

    void pathChanged(const QString &path);
    void on_comboBox_activated(const QString &arg1);

    void on_setManual_clicked();

    void quitApp();

    void on_mainWeb_clicked();

    void on_history_clicked();

    void slotWallPaper(const QString &path);

private:
    Ui::settingWindow *ui;
    QSystemTrayIcon *m_trayIcon{nullptr};
    QMenu *m_traymenu{nullptr};
    DMainWindow *m_parentMainWindow{nullptr};
    int index = 1;

    QString m_currentPath;
    int m_crrenNumber{0};
    int m_isAutoStart{0};
    QString m_currentMode;
    int m_voiceVolume{0};

    historyWidget *m_history{nullptr};
};

#endif // SETTINGWINDOW_H
