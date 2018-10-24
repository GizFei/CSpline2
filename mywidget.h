#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include "paintwindow.h"

namespace Ui {
class MyWidget;
}

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();
private slots:
    void on_goBtn_clicked();
    void on_checkBox_toggled(bool checked);

    void on_grainSpinB_valueChanged(int arg1);

    void on_tensionSpinB_valueChanged(double arg1);

    void on_clearBtn_clicked();

    void on_moveCarBtn_clicked();

    void on_checkBox_2_toggled(bool checked);

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::MyWidget *ui;
    PaintWindow *paintWindow;
};

#endif // MYWIDGET_H
