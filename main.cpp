#include "mywidget.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyWidget w;
    QFile qss(":/qss/cspline.qss");
    if(qss.open(QFile::ReadOnly))
    {
        QString style = QLatin1String(qss.readAll());
        w.setStyleSheet(style);
        qss.close();
    }else
    {
        qDebug("Failed");
    }
    w.show();

    return a.exec();
}
