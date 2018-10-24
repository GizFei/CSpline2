#include "paintwindow.h"
#include "cspline.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <vector>
#include <QPointF>
#include <cmath>
#include <iostream>

PaintWindow::PaintWindow(QWidget *parent):QWidget(parent), drawLine(true), drawCar(false)
{
    cspline = NULL;
    car = new Car;

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(moveCar()));
}

PaintWindow::~PaintWindow()
{
    delete timer;
    delete cspline;
    delete car;
}

void PaintWindow::drawSpline(int grain, float tension)
{
    int n = orgPoints.size();
    int *x = new int[n];
    int *y = new int[n];
    for(int i = 0; i < n; i++)
    {
        x[i] = orgPoints[i].x;
        y[i] = orgPoints[i].y;
    }
    cspline = new CSpline(x, y, n, grain, tension);
    repaint();
}

void PaintWindow::setIsDrawLine(bool isDraw)
{
    drawLine = isDraw;
    repaint();
}

void PaintWindow::setSpeed(int v)
{
    this->car->v = float(v);
}

void PaintWindow::startCar()
{
    car->reset();
    car->totalLen = cspline->getTotalLen();
    car->preX = car->x = this->orgPoints[0].x;
    car->preY = car->y = this->orgPoints[0].y;
    //std::cout << car->totalLen << std::endl;

    timer->setInterval(100); // 每隔0.1s，移动小车
    timer->start();
}

void PaintWindow::clear()
{
    orgPoints.clear();
    car->reset();
    cspline = NULL;
    repaint();
}

void PaintWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // 绘制路径点
    painter.setBrush(QBrush(QColor("#33B5E6"), Qt::SolidPattern));
    painter.setPen(QColor("#619FFF"));
    for(unsigned i = 0; i < orgPoints.size(); i++)
    {
        painter.drawEllipse(QPointF(orgPoints[i].x, orgPoints[i].y), 5, 5);
    }

    // 绘制原始线段（直线）
    if(orgPoints.size() > 1 && drawLine)
    {
        painter.setPen(QColor("#CCC"));
        for(unsigned i = 0; i < orgPoints.size() - 1; i++)
        {
            painter.drawLine(orgPoints[i].x, orgPoints[i].y,
                             orgPoints[i+1].x, orgPoints[i+1].y);
        }
    }

    // 绘制插值点
    if(cspline != NULL && showGrain){
        painter.setPen(QColor("#619FFF"));
        Point* ps = cspline->getPoints();
        for(unsigned i = 0; i < cspline->pn; i++){
            painter.drawEllipse(QPoint(ps[i].x, ps[i].y),5,5);
        }
    }

    // 绘制样条
    if(cspline != NULL)
    {
        painter.setPen(QPen(QColor("#33B5E6"), 2));
        Point *points = cspline->getPoints();
        Point *startPoint = points;
        Point *nextPoint = points + 1;
        for(int i = 0; i < cspline->pn-1; i++)
        {
            painter.drawLine(QPoint(startPoint->x, startPoint->y),
                             QPoint(nextPoint->x, nextPoint->y));
            startPoint++;
            nextPoint++;
        }
    }

    if(drawCar)
    {
        painter.translate(car->x, car->y);
        if(car->angle * (car->y-car->preY) < 0)
        {
            painter.rotate(car->angle*180/3.14 + 180);
            painter.drawPixmap(-car->width/2,-car->height, car->width, car->height, *car->carImg);
            painter.rotate(-car->angle*180/3.14 - 180);
            painter.translate(-car->x, -car->y);
        }
        else
        {
            painter.rotate(car->angle*180/3.14);
            painter.drawPixmap(-car->width/2,-car->height, car->width, car->height, *car->carImg);
            painter.rotate(-car->angle*180/3.14);
            painter.translate(-car->x, -car->y);
        }
        //painter.drawRect(-car->width/2,-car->height, car->width, car->height);
    }else
    {
        if(car->endState)
        {
            //std::cout << car->x << "," << car->y << std::endl;
            painter.translate(car->x, car->y);
            //std::cout << car->angle * (car->y-car->preY) << std::endl;
            if(car->angle * (car->y-car->preY) < 0)
            {
                painter.rotate(car->angle*180/3.14 + 180);
                painter.drawPixmap(-car->width/2,-car->height, car->width, car->height, *car->carImg);
                painter.rotate(-car->angle*180/3.14 - 180);
                painter.translate(-car->x, -car->y);
            }
            else
            {
                painter.rotate(car->angle*180/3.14);
                painter.drawPixmap(-car->width/2,-car->height, car->width, car->height, *car->carImg);
                painter.rotate(-car->angle*180/3.14);
                painter.translate(-car->x, -car->y);
            }
//            painter.translate(car->x, car->y);
//            painter.rotate(car->angle*180/3.14 + 180);
//            painter.drawPixmap(-car->width/2,-car->height, car->width, car->height, *car->carImg);
//            painter.rotate(-car->angle*180/3.14 - 180);
//            painter.translate(-car->x, -car->y);
        }else
        {
            painter.translate(0,0);
            painter.drawPixmap(0,0, car->width, car->height, *car->carImg);
        }
    }
}

void PaintWindow::mousePressEvent(QMouseEvent *e)
{
    Point point;
    point.x = e->x();
    point.y = e->y();
    //point.print();
    orgPoints.push_back(point);

    repaint();
}

void PaintWindow::moveCar()
{
    drawCar = true;
    //std::cout << "prevLen:" << car->prevLen << " totalLen:" << car->totalLen << std::endl;
    car->prevLen += car->v * 0.1;  // time:0.1s

    if(car->totalLen <= car->prevLen)
    {
        car->endState = true;
        drawCar = false;
        //car->reset();
        timer->stop();
        repaint();
        return;
    }

    car->preX = car->x;
    car->preY = car->y;
    Point *p = cspline->getPointByS(car->prevLen);
    car->x = p->x;
    car->y = p->y;
    //std::cout << car->x << "," << car->y << std::endl;
    car->angle = atan((car->y-car->preY)/(car->x-car->preX)); // 弧度


    repaint();
}

Car::Car():endState(false)
{
    carImg = new QPixmap(":/icon/icons/Car.png");
    v = 100.0;
    prevLen = totalLen = preX = preY = x = y = 0.0;
    angle = 0.0;
    width = 70;
    height = 40;
}

Car::~Car()
{
    delete carImg;
}

void Car::reset()
{
    endState = false;
    prevLen = totalLen = preX = preY = x = y = 0.0;
    angle = 0.0;
}

