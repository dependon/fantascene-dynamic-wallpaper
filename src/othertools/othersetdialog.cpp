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
    int CpuX= IniManager::instance()->value("CpuDisplay/X",0).toInt();
    int CpuY = IniManager::instance()->value("CpuDisplay/Y",0).toInt();
    int CpuWidth= IniManager::instance()->value("CpuDisplay/Width",0).toInt();
    int CpuHeight= IniManager::instance()->value("CpuDisplay/Height",0).toInt();

    color = QColor(170,255,255);
    variantColor = IniManager::instance()->value("CpuDisplay/Color",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }
    ui->trSlider_cpu->setValue(color.alpha());
    ui->x_cpu->setText(QString::number(CpuX));
    ui->y_cpu->setText(QString::number(CpuY));
    ui->width_cpu->setText(QString::number(CpuWidth));
    ui->height_cpu->setText(QString::number(CpuHeight));

    if(bCpuVisible)
    {
        ui->CpuVcombox->setCurrentText(tr("True"));
    }
    else
    {
        ui->CpuVcombox->setCurrentText(tr("False"));
    }

    bool bMemoryVisible = IniManager::instance()->value("MemoryDisplay/Visible",false).toBool();

    int MemoryX= IniManager::instance()->value("MemoryDisplay/X",0).toInt();
    int MemoryY = IniManager::instance()->value("MemoryDisplay/Y",0).toInt();

    int MemoryWidth= IniManager::instance()->value("MemoryDisplay/Width",0).toInt();
    int MemoryHeight= IniManager::instance()->value("MemoryDisplay/Height",0).toInt();

    color = QColor(170,255,255);
    variantColor = IniManager::instance()->value("MemoryDisplay/Color",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }
    ui->trSlider_memory->setValue(color.alpha());
    ui->x_memory->setText(QString::number(MemoryX));
    ui->y_memory->setText(QString::number(MemoryY));
    ui->width_memory->setText(QString::number(MemoryWidth));
    ui->height_memory->setText(QString::number(MemoryHeight));

    if(bMemoryVisible)
    {
        ui->MemoryVcombox->setCurrentText(tr("True"));
    }
    else
    {
        ui->MemoryVcombox->setCurrentText(tr("False"));
    }


    bool bNetworkVisible = IniManager::instance()->value("NetworkDisplay/Visible",false).toBool();
    int NetworkX= IniManager::instance()->value("NetworkDisplay/X",0).toInt();
    int NetworkY = IniManager::instance()->value("NetworkDisplay/Y",0).toInt();

    int NetworkWidth= IniManager::instance()->value("NetworkDisplay/Width",0).toInt();
    int NetworkHeight= IniManager::instance()->value("NetworkDisplay/Height",0).toInt();

    color = QColor(170,255,255);
    variantColor = IniManager::instance()->value("NetworkDisplay/Color_Up",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }
    ui->trSlider_network->setValue(color.alpha());

    ui->x_network->setText(QString::number(NetworkX));
    ui->y_network->setText(QString::number(NetworkY));
    ui->width_network->setText(QString::number(NetworkWidth));
    ui->height_network->setText(QString::number(NetworkHeight));

    if(bNetworkVisible)
    {
        ui->NetworkVcombox->setCurrentText(tr("True"));
    }
    else
    {
        ui->NetworkVcombox->setCurrentText(tr("False"));
    }

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

void OtherSetDialog::on_x_memory_editingFinished()
{
    IniManager::instance()->setValue("MemoryDisplay/X",ui->x_memory->text().toInt());
    Q_EMIT dApp->setMemoryMove(ui->x_memory->text().toInt(),ui->y_memory->text().toInt());
}

void OtherSetDialog::on_y_memory_editingFinished()
{
    IniManager::instance()->setValue("MemoryDisplay/Y",ui->y_memory->text().toInt());
    Q_EMIT dApp->setMemoryMove(ui->x_memory->text().toInt(),ui->y_memory->text().toInt());
}

void OtherSetDialog::on_selectBtn_memory_clicked()
{
    QColor color = QColorDialog::getColor();
    int sliderValue = ui->trSlider_memory->value();
    if (color.isValid())
    {
        color.setAlpha(sliderValue);
        IniManager::instance()->setValue("MemoryDisplay/Color",color);
        Q_EMIT dApp->setMemoryColor(color);
    }
}

void OtherSetDialog::on_trSlider_memory_sliderReleased()
{
    QColor color = QColor(170,255,255);
    QVariant variantColor = IniManager::instance()->value("MemoryDisplay/Color",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }

    int sliderValue = ui->trSlider_memory->value();
    if (color.isValid())
    {
        color.setAlpha(sliderValue);
        IniManager::instance()->setValue("MemoryDisplay/Color",color);
        Q_EMIT dApp->setMemoryColor(color);
    }
}

void OtherSetDialog::on_x_cpu_editingFinished()
{
    IniManager::instance()->setValue("CpuDisplay/X",ui->x_cpu->text().toInt());
    Q_EMIT dApp->setCpuMove(ui->x_cpu->text().toInt(),ui->y_cpu->text().toInt());
}

void OtherSetDialog::on_y_cpu_editingFinished()
{
    IniManager::instance()->setValue("CpuDisplay/Y",ui->y_cpu->text().toInt());
    Q_EMIT dApp->setCpuMove(ui->x_cpu->text().toInt(),ui->y_cpu->text().toInt());
}

void OtherSetDialog::on_selectBtn_cpu_clicked()
{
    QColor color = QColorDialog::getColor();
    int sliderValue = ui->trSlider_cpu->value();
    if (color.isValid())
    {
        color.setAlpha(sliderValue);
        IniManager::instance()->setValue("CpuDisplay/Color",color);
        Q_EMIT dApp->setCpuColor(color);
    }
}

void OtherSetDialog::on_trSlider_cpu_sliderReleased()
{
    {
        QColor color = QColor(170,255,255);
        QVariant variantColor = IniManager::instance()->value("CpuDisplay/Color",color);
        if (variantColor.canConvert<QColor>()) {
            color = variantColor.value<QColor>();
        } else {
            color = QColor(170,255,255);
        }

        int sliderValue = ui->trSlider_cpu->value();
        if (color.isValid())
        {
            color.setAlpha(sliderValue);
            IniManager::instance()->setValue("CpuDisplay/Color",color);
            Q_EMIT dApp->setCpuColor(color);
        }
    }
}

void OtherSetDialog::on_width_cpu_editingFinished()
{
    IniManager::instance()->setValue("CpuDisplay/Width",ui->width_cpu->text().toInt());
    Q_EMIT dApp->setCpuSize(ui->width_cpu->text().toInt(),ui->height_cpu->text().toInt());
}


void OtherSetDialog::on_height_cpu_editingFinished()
{
    IniManager::instance()->setValue("CpuDisplay/Height",ui->height_cpu->text().toInt());
    Q_EMIT dApp->setCpuSize(ui->width_cpu->text().toInt(),ui->height_cpu->text().toInt());
}


void OtherSetDialog::on_width_memory_editingFinished()
{
    IniManager::instance()->setValue("MemoryDisplay/Width",ui->width_memory->text().toInt());
    Q_EMIT dApp->setMemorySize(ui->width_memory->text().toInt(),ui->height_memory->text().toInt());
}


void OtherSetDialog::on_height_memory_editingFinished()
{
    IniManager::instance()->setValue("MemoryDisplay/Height",ui->height_memory->text().toInt());
    Q_EMIT dApp->setMemorySize(ui->width_memory->text().toInt(),ui->height_memory->text().toInt());
}


void OtherSetDialog::on_NetworkVcombox_currentTextChanged(const QString &arg1)
{
    if(arg1 == tr("True"))
    {
        IniManager::instance()->setValue("NetworkDisplay/Visible",true);
        Q_EMIT dApp->setNetworkVisible(true);
    }
    else if(arg1 == tr("False"))
    {
        IniManager::instance()->setValue("NetworkDisplay/Visible",false);
        Q_EMIT dApp->setNetworkVisible(false);
    }
}

void OtherSetDialog::on_x_network_editingFinished()
{
    IniManager::instance()->setValue("NetworkDisplay/X",ui->x_network->text().toInt());
    Q_EMIT dApp->setNetworkMove(ui->x_network->text().toInt(),ui->y_network->text().toInt());
}

void OtherSetDialog::on_y_network_editingFinished()
{
    IniManager::instance()->setValue("NetworkDisplay/Y",ui->y_network->text().toInt());
    Q_EMIT dApp->setNetworkMove(ui->x_network->text().toInt(),ui->y_network->text().toInt());
}

void OtherSetDialog::on_selectBtn_networkUp_clicked()
{
    QColor color = QColorDialog::getColor();
    int sliderValue = ui->trSlider_network->value();
    if (color.isValid())
    {
        color.setAlpha(sliderValue);
        IniManager::instance()->setValue("NetworkDisplay/Color_Up",color);
        Q_EMIT dApp->setNetworkUpFontColor(color);
    }
}

void OtherSetDialog::on_selectBtn_networkDown_clicked()
{
    QColor color = QColorDialog::getColor();
    int sliderValue = ui->trSlider_network->value();
    if (color.isValid())
    {
        color.setAlpha(sliderValue);
        IniManager::instance()->setValue("NetworkDisplay/Color_Down",color);
        Q_EMIT dApp->setNetworkDownFontColor(color);
    }
}

void OtherSetDialog::on_trSlider_network_sliderReleased()
{
    {
        QColor color = QColor(170,255,255);
        QVariant variantColor = IniManager::instance()->value("NetworkDisplay/Color_Up",color);
        if (variantColor.canConvert<QColor>()) {
            color = variantColor.value<QColor>();
        } else {
            color = QColor(170,255,255);
        }

        int sliderValue = ui->trSlider_network->value();
        if (color.isValid())
        {
            color.setAlpha(sliderValue);
            IniManager::instance()->setValue("NetworkDisplay/Color_Up",color);
            Q_EMIT dApp->setNetworkUpFontColor(color);
        }
    }

    {
        QColor color = QColor(170,255,255);
        QVariant variantColor = IniManager::instance()->value("NetworkDisplay/Color_Down",color);
        if (variantColor.canConvert<QColor>()) {
            color = variantColor.value<QColor>();
        } else {
            color = QColor(170,255,255);
        }

        int sliderValue = ui->trSlider_network->value();
        if (color.isValid())
        {
            color.setAlpha(sliderValue);
            IniManager::instance()->setValue("NetworkDisplay/Color_Down",color);
            Q_EMIT dApp->setNetworkDownFontColor(color);
        }
    }

}

void OtherSetDialog::on_width_network_editingFinished()
{
    IniManager::instance()->setValue("NetworkDisplay/Width",ui->width_network->text().toInt());
    Q_EMIT dApp->setNetworkSize(ui->width_network->text().toInt(),ui->height_network->text().toInt());
}

void OtherSetDialog::on_height_network_editingFinished()
{
    IniManager::instance()->setValue("NetworkDisplay/Height",ui->height_network->text().toInt());
    Q_EMIT dApp->setNetworkSize(ui->width_network->text().toInt(),ui->height_network->text().toInt());
}


void OtherSetDialog::on_selectBtn_memory_2_clicked()
{
    // 设置选项，使对话框显示透明度选择
    QColorDialog::ColorDialogOptions options = QColorDialog::ShowAlphaChannel;

    QColor color = QColorDialog::getColor(Qt::white, nullptr, "Select Color", options);
    if (color.isValid())
    {
        IniManager::instance()->setValue("MemoryDisplay/Color_Font",color);
        Q_EMIT dApp->setMemoryFontColor(color);
    }
}


void OtherSetDialog::on_selectBtn_memory_3_clicked()
{
    // 设置选项，使对话框显示透明度选择
    QColorDialog::ColorDialogOptions options = QColorDialog::ShowAlphaChannel;

    QColor color = QColorDialog::getColor(Qt::white, nullptr, "Select Color", options);
    if (color.isValid())
    {
        IniManager::instance()->setValue("MemoryDisplay/Color_Background",color);
        Q_EMIT dApp->setMemoryBackgroundColor(color);
    }
}


void OtherSetDialog::on_selectBtn_cpu_2_clicked()
{
    // 设置选项，使对话框显示透明度选择
    QColorDialog::ColorDialogOptions options = QColorDialog::ShowAlphaChannel;

    QColor color = QColorDialog::getColor(Qt::white, nullptr, "Select Color", options);
    if (color.isValid())
    {
        IniManager::instance()->setValue("CpuDisplay/Color_Font",color);
        Q_EMIT dApp->setCpuFontColor(color);
    }
}


void OtherSetDialog::on_selectBtn_cpu_3_clicked()
{
    // 设置选项，使对话框显示透明度选择
    QColorDialog::ColorDialogOptions options = QColorDialog::ShowAlphaChannel;

    QColor color = QColorDialog::getColor(Qt::white, nullptr, "Select Color", options);
    if (color.isValid())
    {
        IniManager::instance()->setValue("CpuDisplay/Color_Background",color);
        Q_EMIT dApp->setCpuBackgroundColor(color);
    }
}

