#ifndef PLAYLISTSETTINGDIALOG_H
#define PLAYLISTSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class PlaylistSettingDialog;
}

class PlaylistSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlaylistSettingDialog(QWidget *parent = nullptr);
    ~PlaylistSettingDialog();

private slots:
    void on_timeCombox_currentTextChanged(const QString &arg1);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::PlaylistSettingDialog *ui;
};

#endif // PLAYLISTSETTINGDIALOG_H
