#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    int h=20; // размерность ячейки;
    //    test.dat комментарий названия файла. В данной работе используется статичный путь до файла, и статичная размеры сетки;
    int ii = 50;
    int jj = 98;
    //    f.dat комментарий названия файла. В данной работе используется статичный путь до файла, и статичная размеры сетки;
    //    int ii = 22;
    //    int jj =102;
    double x[ii][jj]; // массив координат сетки по оси X;
    double y[ii][jj]; // массив координат сетки по оси Y;
    double F[ii][jj]; // массив значений функции f = доли жидкости в котрольном объёме;
    double Nx[ii][jj]; // массив значений нормали по оси X;
    double Ny[ii][jj]; // массив значений нормали по оси Y;
    double eps = pow(10, -30); //это малая величина, используемая для устранения вычислительной сингулярности;
    double alpha[ii][jj]; // массив значений угла отклонения доли жидкости альфа;
    double SB[ii][jj]; // массив точек находящихся на нижнец границе ячейки;
    double ST[ii][jj]; // массив точек находящихся на верхней границе ячейки;
    double SR[ii][jj]; // массив точек находящихся на правой границе ячейки;
    double SL[ii][jj]; // массив точек находящихся на левой границе ячейки;
    int CS; // хранит целочисленные значения от 1 до 4 - соответствует четырем основным положениям свободной поверхности;

    // считывание файла
    QFile file("C:/Users/oneone/Downloads/test.dat");
    if(file.open(QIODevice::ReadOnly |QIODevice::Text))
    {
        while(!file.atEnd())
        {
            for (int i=0;i<ii; i++){
                for (int j=0; j<jj; j++){
                    QString str = file.readLine();
                    QStringList lst = str.split(" "); //Делим строку на слова разделенные пробелом
                    F[i][j]=lst.at(2).toDouble();
                }
            }
        }
    }


    for(int i=0;i<ii; i++){
        for(int j=0; j<jj; j++){

            x[i][j]=h*i;
            y[i][j]=h*j;

            scene->addRect(QRectF(x[i][j],(-1)*y[i][j],h,h),QPen(Qt::black), QBrush(Qt::cyan));

            //заполнение значений нормалей
            Nx[i][j] = -(F[i + 1][j + 1] + 2 * F[i + 1][j]   + F[i + 1][j - 1] - F[i - 1][j + 1] - 2 * F[i - 1][j] - F[i - 1][j - 1]);
            Ny[i][j] = -(F[i + 1][j + 1] + 2 * F[i][j + 1] + F[i - 1][j + 1] - F[i - 1][j - 1] - 2 * F[i][j - 1] - F[i + 1][j - 1]);

            //заполнение значений альфа
            if((Nx[i][j]>=0 and Ny[i][j]>=0)or(Nx[i][j]<=0 and Ny[i][j]<=0)){
                alpha[i][j]=M_PI_2-fabs(atan((Ny[i][j])/(Nx[i][j]+eps)));
            }
            else if((Nx[i][j]>=0 and Ny[i][j]<=0)or(Nx[i][j]<=0 and Ny[i][j]>=0)){
                alpha[i][j]=fabs(atan((Nx[i][j])/(Ny[i][j]+eps)));
            }

            //F=1 - полностью заполненный контрольный объём
            if(F[i][j]==1) {
                scene->addRect(
                            QRectF(x[i][j],(-1)*y[i][j],h,h),
                            QPen(Qt::black),
                            QBrush(Qt::blue));
            }


            if(alpha[i][j]<M_PI_4){

                if (F[i][j]<0.5*tan(alpha[i][j])) {
                    CS=1;
                }
                else if(F[i][j]<1-0.5*tan(alpha[i][j])) {
                    CS=2;
                }
                else {
                    CS=4;
                }
            }
            else if(alpha[i][j]>M_PI_4) {

                if (F[i][j]<0.5/tan(alpha[i][j])) {
                    CS=1;
                }
                else if(F[i][j]<(1-0.5/tan(alpha[i][j]))) {
                    CS=3;
                }
                else {
                    CS=4;
                }
            }

            switch (CS) {

                case 1:

                if ((Nx[i][j] < 0.0) && (Ny[i][j] >= 0.0)) {
                    SB[i][j] = sqrt(2.0 * F[i][j] * h * h / tan(alpha[i][j]));
                    SR[i][j] = sqrt(2.0 * F[i][j] * h * h * tan(alpha[i][j]));

                    if(SB[i][j] or SR[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(-SB[i][j]+x[i][j]+h,(-1)*y[i][j]+h)
                                          <<QPointF(x[i][j]+h,-SR[i][j]-y[i][j]+h)
                                          <<QPointF(x[i][j]+h,(-1)*y[i][j]+h),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] < 0.0) && (Ny[i][j] < 0.0)) {
                    ST[i][j] = sqrt(2.0 * F[i][j] * h * h / tan(alpha[i][j]));
                    SR[i][j] = sqrt(2.0 * F[i][j] * h * h * tan(alpha[i][j]));

                    if(ST[i][j]>0 or SR[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(-ST[i][j]+h+x[i][j],-y[i][j])
                                          <<QPointF(x[i][j]+h,-y[i][j])
                                          <<QPointF(x[i][j]+h,SR[i][j]-y[i][j]),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] >= 0.0) && (Ny[i][j] < 0.0)) {
                    ST[i][j] = sqrt(2.0 * F[i][j] * h * h / tan(alpha[i][j]));
                    SL[i][j] = sqrt(2.0 * F[i][j] * h * h * tan(alpha[i][j]));

                    if(ST[i][j]>0 or SL[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(ST[i][j]+x[i][j],-y[i][j])
                                          <<QPointF(x[i][j],SL[i][j]-y[i][j])
                                          <<QPointF(x[i][j],-y[i][j]),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] >= 0.0) && (Ny[i][j] >= 0.0)) {
                    SB[i][j] = sqrt(2.0 * F[i][j] * h * h / tan(alpha[i][j]));
                    SL[i][j] = sqrt(2.0 * F[i][j] * h * h * tan(alpha[i][j]));

                    if(SB[i][j]>0 or SL[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(SB[i][j]+x[i][j],(-1)*y[i][j]+h)
                                          <<QPointF(x[i][j],-SL[i][j]-y[i][j]+h)
                                          <<QPointF(x[i][j],(-1)*y[i][j]+h),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                break;

            case 2:

                if ((Nx[i][j] < 0.0) && (Ny[i][j] >= 0.0)) {
                    SL[i][j] = F[i][j] * h - 0.5 * h * tan(alpha[i][j]);
                    SR[i][j] = F[i][j] * h + 0.5 * h * tan(alpha[i][j]);

                    if (SL[i][j]>0 or SR[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(x[i][j], -SL[i][j]-y[i][j]+h)
                                          <<QPointF(x[i][j]+h, -SR[i][j]-y[i][j]+h)
                                          <<QPointF(x[i][j]+h, -y[i][j]+h)
                                          <<QPointF(x[i][j], -y[i][j]+h),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] < 0.0) && (Ny[i][j] < 0.0)) {
                    SL[i][j] = F[i][j] * h - 0.5 * h * tan(alpha[i][j]);
                    SR[i][j] = F[i][j] * h + 0.5 * h * tan(alpha[i][j]);

                    if (SL[i][j]>0 or SR[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(x[i][j], -y[i][j])
                                          <<QPointF(x[i][j], SL[i][j]-y[i][j])
                                          <<QPointF(x[i][j]+h, SR[i][j]-y[i][j])
                                          <<QPointF(x[i][j]+h, -y[i][j]),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] >= 0.0) && (Ny[i][j] < 0.0)) {
                    SL[i][j] = F[i][j] * h + 0.5 * h * tan(alpha[i][j]);
                    SR[i][j] = F[i][j] * h - 0.5 * h * tan(alpha[i][j]);

                    if (SL[i][j]>0 or SR[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(x[i][j], -y[i][j])
                                          <<QPointF(x[i][j], SL[i][j]-y[i][j])
                                          <<QPointF(x[i][j]+h, SR[i][j]-y[i][j])
                                          <<QPointF(x[i][j]+h, -y[i][j]),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] >= 0.0) && (Ny[i][j] >= 0.0)) {
                    SL[i][j] = F[i][j] * h + 0.5 * h * tan(alpha[i][j]);
                    SR[i][j] = F[i][j] * h - 0.5 * h * tan(alpha[i][j]);

                    if (SL[i][j]>0 or SR[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(x[i][j], -SL[i][j]-y[i][j]+h)
                                          <<QPointF(x[i][j]+h, -SR[i][j]-y[i][j]+h)
                                          <<QPointF(x[i][j]+h, -y[i][j]+h)
                                          <<QPointF(x[i][j], -y[i][j]+h),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                break;

            case 3:

                if ((Nx[i][j] < 0.0) && (Ny[i][j] >= 0.0)) {
                    SB[i][j] = F[i][j] * h + 0.5 * h / tan(alpha[i][j]);
                    ST[i][j] = F[i][j] * h - 0.5 * h / tan(alpha[i][j]);

                    if(SB[i][j]>0 or ST[i][j]>0){
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(h - ST[i][j]+x[i][j], -y[i][j])
                                          <<QPointF(h - SB[i][j]+x[i][j], -y[i][j]+h)
                                          <<QPointF(x[i][j]+h, -y[i][j]+h)
                                          <<QPointF(x[i][j]+h, -y[i][j]),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] < 0.0) && (Ny[i][j] < 0.0)) {
                    SB[i][j] = F[i][j] * h - 0.5 * h / tan(alpha[i][j]);
                    ST[i][j] = F[i][j] * h + 0.5 * h / tan(alpha[i][j]);

                    if(SB[i][j]>0 or ST[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(h-ST[i][j]+x[i][j], -y[i][j])
                                          <<QPointF(h - SB[i][j]+x[i][j], -y[i][j]+h)
                                          <<QPointF(x[i][j]+h, -y[i][j]+h)
                                          <<QPointF(x[i][j]+h, -y[i][j]),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] >= 0.0) && (Ny[i][j] < 0.0)) {
                    SB[i][j] = F[i][j] * h - 0.5 * h / tan(alpha[i][j]);
                    ST[i][j] = F[i][j] * h + 0.5 * h / tan(alpha[i][j]);

                    if(SB[i][j]>0 or ST[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(ST[i][j]+x[i][j], -y[i][j])
                                          <<QPointF(SB[i][j]+x[i][j], -y[i][j]+h)
                                          <<QPointF(x[i][j], -y[i][j]+h)
                                          <<QPointF(x[i][j], -y[i][j]),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] >= 0.0) && (Ny[i][j] >= 0.0)) {
                    SB[i][j] = F[i][j] * h + 0.5 * h / tan(alpha[i][j]);
                    ST[i][j] = F[i][j] * h - 0.5 * h / tan(alpha[i][j]);

                    if(SB[i][j]>0 or ST[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(ST[i][j]+x[i][j], -y[i][j])
                                          <<QPointF(SB[i][j]+x[i][j], -y[i][j]+h)
                                          <<QPointF(x[i][j], -y[i][j]+h)
                                          <<QPointF(x[i][j], -y[i][j]),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                break;

            case 4:

                if ((Nx[i][j] < 0.0) && (Ny[i][j] > 0.0)) {
                    ST[i][j] = h - sqrt(2.0 * (1.0 - F[i][j]) * h * h / tan(alpha[i][j]));
                    SL[i][j] = h - sqrt(2.0 * (1.0 - F[i][j]) * h * h * tan(alpha[i][j]));

                    if (h>ST[i][j]>0 || h>SL[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(x[i][j],h-SL[i][j]-y[i][j])
                                          <<QPointF(x[i][j]-ST[i][j]+h,-y[i][j])
                                          <<QPointF(x[i][j]+h, -y[i][j])
                                          <<QPointF(x[i][j]+h,-y[i][j]+h)
                                          <<QPointF(x[i][j], -y[i][j]+h),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] < 0.0) && (Ny[i][j] < 0.0)) {
                    SL[i][j] = h - sqrt(2.0 * (1.0 - F[i][j]) * h * h * tan(alpha[i][j]));
                    SB[i][j] = h - sqrt(2.0 * (1.0 - F[i][j]) * h * h / tan(alpha[i][j]));

                    if(SL[i][j]>0 or SB[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(h-SB[i][j]+x[i][j],(-1)*y[i][j]+h)
                                          <<QPointF(x[i][j],SL[i][j]-y[i][j])
                                          <<QPointF(x[i][j],-y[i][j])
                                          <<QPointF(x[i][j]+h,-y[i][j])
                                          <<QPointF(x[i][j]+h,-y[i][j]+h),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] >= 0.0) && (Ny[i][j] < 0.0)) {
                    SB[i][j] = h - sqrt(2.0 * (1.0 - F[i][j]) * h * h / tan(alpha[i][j]));
                    SR[i][j] = h - sqrt(2.0 * (1.0 - F[i][j]) * h * h * tan(alpha[i][j]));

                    if (SR[i][j]>0 or SB[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(x[i][j],-y[i][j]+h)
                                          <<QPointF(x[i][j],-y[i][j])
                                          <<QPointF(x[i][j]+h,-y[i][j])
                                          <<QPointF(x[i][j]+h,SR[i][j]-y[i][j])
                                          <<QPointF(SB[i][j]+x[i][j],-y[i][j]+h),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }

                if ((Nx[i][j] >= 0.0) && (Ny[i][j] >= 0.0)) {
                    SR[i][j] = h - sqrt(2.0 * (1.0 - F[i][j]) * h * h * tan(alpha[i][j]));
                    ST[i][j] = h - sqrt(2.0 * (1.0 - F[i][j]) * h * h / tan(alpha[i][j]));

                    if(h>SR[i][j]>0 or h>ST[i][j]>0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                                          <<QPointF(x[i][j],-y[i][j]+h)
                                          <<QPointF(x[i][j],-y[i][j])
                                          <<QPointF(x[i][j]+ST[i][j],-y[i][j])
                                          <<QPointF(x[i][j]+h,-y[i][j]+h-SR[i][j])
                                          <<QPointF(x[i][j]+h,-y[i][j]+h),
                                          QPen(Qt::black), QBrush(Qt::blue));
                    }
                }
                break;
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
