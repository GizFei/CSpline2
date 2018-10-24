#ifndef CSPINE_H
#define CSPINE_H

class Point
{
public:
    float x;
    float y;
    float z;
    Point() {x = 0.0; y = 0.0; z = 0.0;}
    void print();
};

class CSpline
{
public:
    int orgPn; // 原始点个数
    int pn; // 所有点的个数
    CSpline(int x[], int y[], int n, int grain, float tension);
    void CubicSpline(int n, Point *knots, int grain, float tension);
    Point* getPoints();
    Point* getPointByS(float s);
    float getTotalLen();
    ~CSpline();
private:
    Point *orgPoints; // 插值前的点（包含扩充后的头尾两个虚拟点）
    Point *points; // 插值后所有的点
    float M[4][4];
    void GetCardinalMatrix(float a);
    float GetAxisValue(float a, float b, float c, float d, float u);
    float Simpson(int segment, float a, float b);
    float f(int i, float ui);
    void initArgs();
    void initArcLens();
    int getSegment(float s);
    float getU(float s);
    float getU2(int segment, float u1, float u2, float uLen, float prec);
    float getULen(int i, float s);
    float *A, *B, *C, *D, *E;
    float *a[3], *b[3], *c[3];
    float *arcLens;
};

#endif // CSPINE_H
