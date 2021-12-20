#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QWidget>
class view;
namespace Ui {
class historyWidget;
}

class historyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit historyWidget(QWidget *parent = nullptr);
    ~historyWidget();
protected:
    void showEvent(QShowEvent *event) override;
private slots:
    void on_setWallPaper_clicked();

    void on_delWallPaper_clicked();

    void on_importBtn_clicked();

private:
    Ui::historyWidget *ui;
    view *m_view{nullptr};
};

#endif // HISTORYWIDGET_H
