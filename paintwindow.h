#ifndef PAINTWINDOW
#define PAINTWINDOW

#include <QWidget>
#include <vector>
#include <QPixmap>
#include <QTimer>
#include "cspline.h"

class Car
{
public:
    Car();
    ~Car();
    float preX, preY, x, y;
    float v;
    float prevLen, totalLen;
    int width, height;
    float angle;
    bool endState;
    QPixmap *carImg;
    void reset();
};

class PaintWindow : public QWidget
{
    Q_OBJECT
public:
    PaintWindow(QWidget *parent = 0);
    ~PaintWindow();
    void drawSpline(int grain, float tension);
    void setIsDrawLine(bool isDraw);
    void setSpeed(int v);
    void startCar();
    void clear();
    void setShowGrains(bool b){ showGrain = b; repaint();}
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
private:
    QTimer *timer;
    Car *car;
    bool drawLine;
    bool drawCar;
    std::vector<Point> orgPoints;
    CSpline *cspline;
    bool showGrain;
private slots:
    void moveCar();
};

#endif // PAINTWINDOW

