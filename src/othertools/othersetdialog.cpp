#include "othersetdialog.h"
#include "ui_othersetdialog.h"
#include "ini/inimanager.h"
#include "application.h"
#include <QColorDialog>
OtherSetDialog::OtherSetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OtherSetDialog)
{
    ui->setupUi(this);
    ui->checkBoxShow->setVisible(false);
    QString FontName = IniManager::instance()->value("TimeDisplay/FontName","Winter Trendy").toString();
    int FontSize = IniManager::instance()->value("TimeDisplay/FontSize",25).toInt();
    int FontX= IniManager::instance()->value("TimeDisplay/X",0).toInt();
    int FontY = IniManager::instance()->value("TimeDisplay/Y",0).toInt();

    QString timeFormat = IniManager::instance()->value("TimeDisplay/TimeFormat","yyyy-MM-dd HH:mm:ss").toString();

    bool bVisible = IniManager::instance()->value("TimeDisplay/Visible",false).toBool();
    if(bVisible)
    {
        ui->comboBoxVisible->setCurrentText(tr("True"));
    }
    else
    {
        ui->comboBoxVisible->setCurrentText(tr("False"));
    }

    bool bCpuVisible = IniManager::instance()->value("CpuDisplay/Visible",false).toBool();
    if(bCpuVisible)
    {
        ui->CpuVcombox->setCurrentText(tr("True"));
    }
    else
    {
        ui->CpuVcombox->setCurrentText(tr("False"));
    }

    bool bMemoryVisible = IniManager::instance()->value("MemoryDisplay/Visible",false).toBool();
    if(bMemoryVisible)
    {
        ui->MemoryVcombox->setCurrentText(tr("True"));
    }
    else
    {
        ui->MemoryVcombox->setCurrentText(tr("False"));
    }

    QColor color = QColor(170,255,255);
    QVariant variantColor = IniManager::instance()->value("TimeDisplay/Color",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }
    ui->trSlider->setValue(color.alpha());

    ui->comboBoxFormat->setCurrentText(timeFormat);
    ui->x->setText(QString::number(FontX));
    ui->y->setText(QString::number(FontY));
    ui->fontComboBox->setCurrentText(FontName);

    ui->fontSizeEdit->setText(QString::number(FontSize));


    this->setWindowTitle(tr("Other Display Tools"));

}

OtherSetDialog::~OtherSetDialog()
{
    delete ui;
}

void OtherSetDialog::on_closeBtn_clicked()
{
    this->close();
}

void OtherSetDialog::on_comboBoxFormat_currentTextChanged(const QString &arg1)
{
    IniManager::instance()->setValue("TimeDisplay/TimeFormat",arg1);
    Q_EMIT dApp->setTimeFormat(arg1);
    //刷新下字体大小
    on_fontSizeEdit_editingFinished();
}


void OtherSetDialog::on_x_editingFinished()
{
    IniManager::instance()->setValue("TimeDisplay/X",ui->x->text().toInt());
    Q_EMIT dApp->setTimeMove(ui->x->text().toInt(),ui->y->text().toInt());
}


void OtherSetDialog::on_y_editingFinished()
{
    IniManager::instance()->setValue("TimeDisplay/Y",ui->y->text().toInt());
    Q_EMIT dApp->setTimeMove(ui->x->text().toInt(),ui->y->text().toInt());
}


void OtherSetDialog::on_fontComboBox_currentTextChanged(const QString &arg1)
{
    IniManager::instance()->setValue("TimeDisplay/FontName",arg1);
    Q_EMIT dApp->setTimeFontFamily(arg1);
}


void OtherSetDialog::on_checkBoxShow_clicked(bool checked)
{
    IniManager::instance()->setValue("TimeDisplay/Visible",checked);
    Q_EMIT dApp->setTimeVisible(checked);
}


void OtherSetDialog::on_comboBoxVisible_currentTextChanged(const QString &arg1)
{
    if(arg1 == tr("True"))
    {
        IniManager::instance()->setValue("TimeDisplay/Visible",true);
        Q_EMIT dApp->setTimeVisible(true);
    }
    else if(arg1 == tr("False"))
    {
        IniManager::instance()->setValue("TimeDisplay/Visible",false);
        Q_EMIT dApp->setTimeVisible(false);
    }
}


void OtherSetDialog::on_selectBtn_clicked()
{
    QColor color = QColorDialog::getColor();
    int sliderValue = ui->trSlider->value();
    if (color.isValid())
    {
        color.setAlpha(sliderValue);
        IniManager::instance()->setValue("TimeDisplay/Color",color);
        Q_EMIT dApp->setTimeFontColor(color);
    }
}


void OtherSetDialog::on_fontSizeEdit_editingFinished()
{
    int FontSize = ui->fontSizeEdit->text().toInt();
    if(FontSize > 0 && FontSize < 200)
    {
        IniManager::instance()->setValue("TimeDisplay/FontSize",FontSize);
    }
    Q_EMIT dApp->setTimeFontPointSize(FontSize);
}


void OtherSetDialog::on_trSlider_sliderReleased()
{
    QColor color = QColor(170,255,255);
    QVariant variantColor = IniManager::instance()->value("TimeDisplay/Color",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }

    int sliderValue = ui->trSlider->value();
    if (color.isValid())
    {
        color.setAlpha(sliderValue);
        IniManager::instance()->setValue("TimeDisplay/Color",color);
        Q_EMIT dApp->setTimeFontColor(color);
    }
}


void OtherSetDialog::on_CpuVcombox_currentTextChanged(const QString &arg1)
{
    if(arg1 == tr("True"))
    {
        IniManager::instance()->setValue("CpuDisplay/Visible",true);
        Q_EMIT dApp->setCpuVisible(true);
    }
    else if(arg1 == tr("False"))
    {
        IniManager::instance()->setValue("CpuDisplay/Visible",false);
        Q_EMIT dApp->setCpuVisible(false);
    }
}

void OtherSetDialog::on_MemoryVcombox_currentTextChanged(const QString &arg1)
{
    if(arg1 == tr("True"))
    {
        IniManager::instance()->setValue("MemoryDisplay/Visible",true);
        Q_EMIT dApp->setMemoryVisible(true);
    }
    else if(arg1 == tr("False"))
    {
        IniManager::instance()->setValue("MemoryDisplay/Visible",false);
        Q_EMIT dApp->setMemoryVisible(false);
    }
}
