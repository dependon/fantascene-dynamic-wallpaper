#ifndef OTHERSETDIALOG_H
#define OTHERSETDIALOG_H

#include <QDialog>

namespace Ui {
class OtherSetDialog;
}

class OtherSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OtherSetDialog(QWidget *parent = nullptr);
    ~OtherSetDialog();

private Q_SLOTS:
    void on_closeBtn_clicked();

    void on_comboBoxFormat_currentTextChanged(const QString &arg1);

    void on_x_editingFinished();

    void on_y_editingFinished();

    void on_fontComboBox_currentTextChanged(const QString &arg1);

    void on_checkBoxShow_clicked(bool checked);

    void on_comboBoxVisible_currentTextChanged(const QString &arg1);

    void on_selectBtn_clicked();

    void on_fontSizeEdit_editingFinished();

private:
    Ui::OtherSetDialog *ui;
};

#endif // OTHERSETDIALOG_H
