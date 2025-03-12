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

    //Date Tool
    void on_x_editingFinished();

    void on_y_editingFinished();

    void on_fontComboBox_currentTextChanged(const QString &arg1);

    void on_checkBoxShow_clicked(bool checked);

    void on_comboBoxVisible_currentTextChanged(const QString &arg1);

    void on_selectBtn_clicked();

    void on_fontSizeEdit_editingFinished();

    void on_trSlider_sliderReleased();


   //Cpu Tool
    void on_CpuVcombox_currentTextChanged(const QString &arg1);

    void on_x_cpu_editingFinished();

    void on_y_cpu_editingFinished();

    void on_selectBtn_cpu_clicked();

    void on_trSlider_cpu_sliderReleased();

    void on_width_cpu_editingFinished();

    void on_height_cpu_editingFinished();

    void on_selectBtn_cpu_2_clicked();

    void on_selectBtn_cpu_3_clicked();

    //Memory tool

    void on_MemoryVcombox_currentTextChanged(const QString &arg1);

    void on_x_memory_editingFinished();

    void on_y_memory_editingFinished();

    void on_selectBtn_memory_clicked();

    void on_trSlider_memory_sliderReleased();

    void on_width_memory_editingFinished();

    void on_height_memory_editingFinished();

    void on_selectBtn_memory_2_clicked();

    void on_selectBtn_memory_3_clicked();


    //NetWork tool

    void on_NetworkVcombox_currentTextChanged(const QString &arg1);

    void on_x_network_editingFinished();

    void on_y_network_editingFinished();

    void on_selectBtn_networkUp_clicked();

    void on_selectBtn_networkDown_clicked();

    void on_trSlider_network_sliderReleased();

    void on_width_network_editingFinished();

    void on_height_network_editingFinished();
private:
    Ui::OtherSetDialog *ui;
};

#endif // OTHERSETDIALOG_H
