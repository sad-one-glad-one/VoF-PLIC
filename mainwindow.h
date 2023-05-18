#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QFile>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
protected:
    double sizex;
    double sizey;
//    test.dat
//    double sizeF=4900;
//    f.dat
    double sizeF=2244;


private slots:
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
