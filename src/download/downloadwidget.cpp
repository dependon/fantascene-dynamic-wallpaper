#include "downloadwidget.h"
#include "ui_downloadwidget.h"
#include <QDesktopServices>
#include <QUrl>
#include <QScreen>
DownloadWidget::DownloadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadWidget)
{
    ui->setupUi(this);
    ui->label->setText(tr("The download address comes from the network and may not be usable"));
    ui->comboBox->addItem("https://livewallp.com/");
    ui->comboBox->addItem("https://zhutix.com/animated/");
    ui->comboBox->addItem("https://livewallpaper.giantapp.cn/wallpapers");
    ui->comboBox->addItem("https://mylivewallpapers.com/");
    ui->comboBox->addItem("https://www.livewallpaperpc.com/");
    ui->comboBox->addItem("https://wallpaperwaifu.com/");
    ui->comboBox->addItem("https://www.videezy.com/");
    ui->comboBox->addItem("https://www.pexels.com/videos/");
    ui->comboBox->addItem("https://pixabay.com/videos/");
    ui->comboBox->addItem("https://coverr.co/");
    ui->comboBox->addItem("https://www.videvo.net/");
    QLocale locale = QLocale::system();
    QString language = locale.name();
    if (language == "zh_CN") {
        // 当前语言环境是中文
        ui->comboBox->setCurrentIndex(1);

    } else {
        // 当前语言环境不是中文
        ui->comboBox->setCurrentIndex(0);
    }
    setWindowTitle(QObject::tr("Download"));
}

DownloadWidget::~DownloadWidget()
{
    delete ui;
}

void DownloadWidget::on_loadBtn_clicked()
{
    QUrl url(ui->comboBox->currentText());
    QDesktopServices::openUrl(url);
}
