#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Help"));
    // 设置文本浏览器的内容
    QString helpText = "<html>"
                       "<head>"
                       "<style>"
                       "body { text-align: center; }"
                       "</style>"
                       "</head>"
                       "<body>"
                       "<h1>帮助</h1>"
                       "<p>你可以通过以下地址和qq群了解项目的最新动态和程序</p>"
                       "<p><a href=\"https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper\">最新版下载地址</a></p>"
                       "<p><a href=\"https://gitee.com/liuminghang/fantascene-dynamic-wallpaper\">gitee项目地址</a></p>"
                       "<p><a href=\"https://github.com/dependon/fantascene-dynamic-wallpaper\">github项目地址</a></p>"
                       "<p>新建的qq群,期待你的加入</p>"
                       "<h1>917552088</h1>"
                       "<p>如有疑问和程序打不开,崩溃,使用问题等的地方请加群问,如果没有对应发行版的程序也可以找我们提供编译方案等等,有想法的代码可以直接合入,新增功能等等,有需求的也可以提出来探讨.</p>"
                       "<p>如果您有linux的动态壁纸方案和改进建议可以探讨交流,望不宁赐教</p>"
                       "<p>软件可以支持视频格式和html的两种方式打开,可以由本地的html或者一个视频壁纸网址来使用web壁纸,视频壁纸为本地文件,需要自行下载(有线上壁纸仓库的下载的想法,没有想好实现方案和没服务器)</p>"
                       "</body>"
                       "</html>";
    ui->textBrowser->setHtml(helpText);
    ui->textBrowser->setOpenExternalLinks(true);
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::on_closeBtn_clicked()
{
    close();
}
