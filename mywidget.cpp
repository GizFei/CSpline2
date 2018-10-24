#include "mywidget.h"
#include "ui_mywidget.h"
#include <vector>

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    paintWindow = new PaintWindow;
    paintWindow->setObjectName("paintwindow");
    paintWindow->setAttribute(Qt::WA_StyledBackground, true); // 使自定义控件的QSS起作用
    ui->paintContainer->addWidget(paintWindow);
}

MyWidget::~MyWidget()
{
    delete ui;
}

void MyWidget::on_goBtn_clicked()
{
    int grain = ui->grainSpinB->text().toInt();
    float tension = ui->tensionSpinB->text().toFloat();
    paintWindow->drawSpline(grain, tension);
}

void MyWidget::on_checkBox_toggled(bool checked)
{
    paintWindow->setIsDrawLine(checked);
}

void MyWidget::on_grainSpinB_valueChanged(int arg1)
{
    float tension = ui->tensionSpinB->text().toFloat();
    paintWindow->drawSpline(arg1, tension);
}

void MyWidget::on_tensionSpinB_valueChanged(double arg1)
{
    int grain = ui->grainSpinB->text().toInt();
    paintWindow->drawSpline(grain, arg1);
}

void MyWidget::on_clearBtn_clicked()
{
    paintWindow->clear();
}

void MyWidget::on_moveCarBtn_clicked()
{
    int grain = ui->grainSpinB->text().toInt();
    float tension = ui->tensionSpinB->text().toFloat();
    paintWindow->drawSpline(grain, tension);
    paintWindow->startCar();
}

void MyWidget::on_checkBox_2_toggled(bool checked)
{
    paintWindow->setShowGrains(checked);
}

void MyWidget::on_horizontalSlider_valueChanged(int value)
{
    ui->label->setText(QString::number(value));
    this->paintWindow->setSpeed(value);
}
