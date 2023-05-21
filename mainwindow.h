#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QGraphicsScene>
#include <QPainter>
#include <QValidator>
#include <QIntValidator>

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
    int x;
    int y;
    int size;
    QVector<QVector<double>> FList;
    void drawVoF();

protected:

private slots:
    void on_input_x_textChanged(const QString &newValue);
    void on_input_y_textChanged(const QString &newValue);
    void on_input_square_size_textChanged(const QString &newValue);
    void on_draw_grid_button_clicked();
    void on_load_button_clicked();
};
#endif // MAINWINDOW_H
