#ifndef MORESETTING_H
#define MORESETTING_H

#include <QWidget>
#include "data.h"
namespace Ui {
class MoreSetting;
}

class MoreSetting : public QWidget
{
    Q_OBJECT

public:
    explicit MoreSetting(QWidget *parent = nullptr);
    ~MoreSetting();

    void setData(const MoreSetData &data);

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_hwdecBox_activated(const QString &arg1);

private:
    Ui::MoreSetting *ui;
};

#endif // MORESETTING_H
