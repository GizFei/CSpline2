
#include "cspline.h"
#include <iostream>
#include <cmath>
using namespace std;

/**
 * @brief CSpline::CSpline Cardinal样条构造函数
 * @param x 控制点X坐标集
 * @param y 控制点Y坐标集
 * @param n 控制点数量
 * @param grain 平滑插值点的个数
 * @param tension 曲线平滑度 [0, 1]
 */
CSpline::CSpline(int x[], int y[], int n, int grain, float tension)
{
    // 增加头尾两个虚拟点
    Point *knots = new Point[n + 2];
    orgPn = n;

    knots[0].x = x[0];
    knots[0].y = y[0];
    for(int i = 1; i <= n; i++)
    {
        knots[i].x = x[i-1];
        knots[i].y = y[i-1];
    }
    knots[n+1].x = x[n-1];
    knots[n+1].y = y[n-1];
    orgPoints = knots;

    // 调用三维样条
    CubicSpline(n+2, knots, grain, tension);
}

/**
 * @brief CSpline::CubicSpline 创建3D Cardinal样条
 * @param n 控制点个数
 * @param knots 控制点集
 * @param grain 插值点数
 * @param tension 曲线平滑度 [0, 1]
 */
void CSpline::CubicSpline(int n, Point *knots, int grain, float tension)
{
    // ! 这里的n>=4
    int pn = n + grain*(n-3); // 所有点的个数
    this->pn = pn;

    GetCardinalMatrix(tension);
    initArgs();

    points = new Point[pn];
    points[0] = knots[0];
    points[1] = knots[1];
    int index = 2;
    float u[grain];
    for(int i = 0; i < grain; i++)
        u[i] = float(i+1) / (grain+1);
    for(int i = 1; i <= n-3; i++)
    {
        Point *ks = &knots[i-1];
        Point *k1 = &knots[i];
        Point *k2 = &knots[i+1];
        Point *ke = &knots[i+2];
        for(int j = 1; j <= grain; j++)
        {
            points[index].x = GetAxisValue(ks->x, k1->x, k2->x, ke->x, u[j-1]);
            points[index].y = GetAxisValue(ks->y, k1->y, k2->y, ke->y, u[j-1]);
            points[index].z = GetAxisValue(ks->z, k1->z, k2->z, ke->z, u[j-1]);
            //points[index].print();
            index++;
        }
        points[index] = knots[i+1];
        //points[index].print();
        index++;
    }
    points[index] = knots[n-1];
}

Point* CSpline::getPoints()
{
    return this->points;
}

Point *CSpline::getPointByS(float s)
{
    int i = getSegment(s);
    //std::cout << i << " ";
    float u = getU(s);
    //std::cout << u << std::endl;
    Point *ks = orgPoints + i;
    Point *k1 = orgPoints + i+1;
    Point *k2 = orgPoints + i+2;
    Point *ke = orgPoints + i+3;
    Point *p = new Point;
    p->x = GetAxisValue(ks->x, k1->x, k2->x, ke->x, u);
    p->y = GetAxisValue(ks->y, k1->y, k2->y, ke->y, u);
    p->z = GetAxisValue(ks->z, k1->z, k2->z, ke->z, u);
    return p;
}

float CSpline::getTotalLen()
{
    float sum = 0;
    for(int i = 0; i < orgPn - 1; i++)
    {
        sum += arcLens[i];
    }
    return sum;
}

CSpline::~CSpline()
{   
}

// 获得Cardinal矩阵（即方程式里的M）
void CSpline::GetCardinalMatrix(float a)
{
    M[0][0] = -a;  M[0][1] = 2.0 - a;  M[0][2] = -2.0 + a;  M[0][3] = a;
    M[1][0] = 2*a; M[1][1] = -3.0 + a; M[1][2] = 3.0 - 2*a; M[1][3] = -a;
    M[2][0] = -a;  M[2][1] = 0;        M[2][2] = a;         M[2][3] = 0;
    M[3][0] = 0;   M[3][1] = 1.0;      M[3][2] = 0;         M[3][3] = 0;
}

/**
 * @brief CSpline::GetAxisValue 计算P(u)中的某个坐标轴的值
 * 先计算MB，再乘以U的转置。
 *                [p0]
 * [u^3 u^2 u 1]  [p1]
 *                [p2]
 *                [p3]
 * axis: x-0,y-1,z-2
 */
float CSpline::GetAxisValue(float a, float b, float c, float d, float u)
{
    float p0, p1, p2, p3;
    p0 = M[0][0] * a + M[0][1] * b + M[0][2] * c + M[0][3] * d;
    p1 = M[1][0] * a + M[1][1] * b + M[1][2] * c + M[1][3] * d;
    p2 = M[2][0] * a + M[2][1] * b + M[2][2] * c + M[2][3] * d;
    p3 = M[3][0] * a + M[3][1] * b + M[3][2] * c + M[3][3] * d;
    return (p3 + u*(p2 + u*(p1 + u*p0)));
}

/**
 * @brief CSpline::f 计算Simpson函数中f(ui)的值
 * @param i  曲线第几段，从0开始
 * @param ui 未知数ui
 * @return
 */
float CSpline::f(int i, float ui)
{
    float sum = (((A[i]*ui + B[i])*ui + C[i])*ui + D[i])*ui + E[i];
    return sqrt(sum);
}

/**
 * @brief CSpline::Simpson 扩展的Simpson方法计算[a,b]间的弧长
 * @param a
 * @param b
 * @return 弧长
 */
float CSpline::Simpson(int segment, float a, float b)
{
    if(segment > orgPn - 2 || orgPn == 0 || a > b)
    {
        return 0;
    }
    int n = 10; // 分成10个子区间

    float sum = 0;
    float h = (b-a) / n;
    sum += f(segment, a);
    for(int i = 1; i <= n-1; i++)
    {
        float ui = a + i*h;
        if(i%2 == 0)
            sum += 2 * f(segment, ui);
        else
            sum += 4 * f(segment, ui);
    }
    sum += f(segment, b);
    return (h * sum / 3.0);
}

/**
 * @brief CSpline::initArgs 初始化f(u)的五个参数
 */
void CSpline::initArgs()
{
    A = new float[orgPn-1];
    B = new float[orgPn-1];
    C = new float[orgPn-1];
    D = new float[orgPn-1];
    E = new float[orgPn-1];

    // 初始化
    a[0] = new float[orgPn-1]; a[1] = new float[orgPn-1]; a[2] = new float[orgPn-1];
    b[0] = new float[orgPn-1]; b[1] = new float[orgPn-1]; b[2] = new float[orgPn-1];
    c[0] = new float[orgPn-1]; c[1] = new float[orgPn-1]; c[2] = new float[orgPn-1];

    for(int i = 0; i < orgPn-1; i++)
    {
        Point *ks = orgPoints + i;
        Point *k1 = orgPoints + i+1;
        Point *k2 = orgPoints + i+2;
        Point *ke = orgPoints + i+3;
        a[0][i] = M[0][0] * ks->x + M[0][1] * k1->x + M[0][2] * k2->x + M[0][3] * ke->x;
        a[1][i] = M[0][0] * ks->y + M[0][1] * k1->y + M[0][2] * k2->y + M[0][3] * ke->y;
        a[2][i] = M[0][0] * ks->z + M[0][1] * k1->z + M[0][2] * k2->z + M[0][3] * ke->z;

        b[0][i] = M[1][0] * ks->x + M[1][1] * k1->x + M[1][2] * k2->x + M[1][3] * ke->x;
        b[1][i] = M[1][0] * ks->y + M[1][1] * k1->y + M[1][2] * k2->y + M[1][3] * ke->y;
        b[2][i] = M[1][0] * ks->z + M[1][1] * k1->z + M[1][2] * k2->z + M[1][3] * ke->z;

        c[0][i] = M[2][0] * ks->x + M[2][1] * k1->x + M[2][2] * k2->x + M[2][3] * ke->x;
        c[1][i] = M[2][0] * ks->y + M[2][1] * k1->y + M[2][2] * k2->y + M[2][3] * ke->y;
        c[2][i] = M[2][0] * ks->z + M[2][1] * k1->z + M[2][2] * k2->z + M[2][3] * ke->z;
    }
    for(int i = 0; i < orgPn-1; i++)
    {
        A[i] = 9 * (a[0][i]*a[0][i] + a[1][i]*a[1][i] + a[2][i]*a[2][i]);
        B[i] = 12 * (a[0][i]*b[0][i] + a[1][i]*b[1][i] + a[2][i]*b[2][i]);
        C[i] = 6 * (a[0][i]*c[0][i] + a[1][i]*c[1][i] + a[2][i]*c[2][i])
                + 4 * (b[0][i]*b[0][i] + b[1][i]*b[1][i] + b[2][i]*b[2][i]);
        D[i] = 4 * (b[0][i]*c[0][i] + b[1][i]*c[1][i] + b[2][i]*c[2][i]);
        E[i] = c[0][i]*c[0][i] + c[1][i]*c[1][i] + c[2][i]*c[2][i];
    }

    initArcLens();
}

/**
 * @brief CSpline::initArcLens 初始化各段弧长用于查询
 */
void CSpline::initArcLens()
{
    arcLens = new float[orgPn-1];
    for(int i = 0; i < orgPn-1; i++)
    {
        arcLens[i] = Simpson(i, 0, 1);
        //std::cout << arcLens[i] << endl;
    }
}

int CSpline::getSegment(float s)
{
    if(s < 0)
        return -1;

    float sum = 0;
    for(int i = 0; i < orgPn - 1; i++)
    {
        sum += arcLens[i];
        if(s <= sum)
            return i;
    }
    return -1;
}

float CSpline::getU(float s)
{
    int segment = getSegment(s);
    float uLen = getULen(segment, s);

    return getU2(segment, 0, 1, uLen, 0.01);
}

/**
 * @brief CSpline::getU2 getU(float s)的辅助递归函数
 * @param u1
 * @param u2
 * @param uLen
 * @return
 */
float CSpline::getU2(int segment, float u1, float u2, float uLen, float prec)
{
//    float u3 = (u1+u2) / 2; // 中点值
//    float tmp = Simpson(segment, 0, u3);

//    if(fabs(tmp-uLen) <= prec)
//    {
//        return u3;
//    }
//    if(uLen < tmp)
//    {
//        return getU2(segment, u1, u3, uLen, prec);
//    }else
//    {
//        return getU2(segment, u3, u2, uLen, prec);
//    }
    float us = u1;
    float ue = u2;
    float ua, tmp;
    do{
        ua = (us+ue)/2;
        tmp = Simpson(segment, 0, ua);
        if(uLen < tmp)
            ue = ua;
        else
            us = ua;
    }while(fabs(tmp-uLen) > prec);
    return ua;
}

/**
 * @brief CSpline::getULen 在当前段上的弧长
 * @param i 曲线第几段
 * @param s 总弧长（从起始点开始）
 * @return
 */
float CSpline::getULen(int i, float s)
{
    if(i > orgPn-2 || s < 0)
        return 0;

    float preL = 0;
    for(int k = 0; k < i; k++)
    {
        preL += arcLens[k];
    }
    return s - preL;
}

void Point::print()
{
    cout << "(" << this->x << "," << this->y << "," << this->z << ")" << endl;
}

