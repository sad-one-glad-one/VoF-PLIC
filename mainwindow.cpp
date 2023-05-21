#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this); // Инициализация пользовательского интерфейса;
    scene = new QGraphicsScene(); // Создание графической сцены, для отображения графики в QGraphicsView;
    ui->graphicsView->setScene(scene); // Привязка существующего QGraphicsScene к QGraphicsView;

    // Созание экземпляра валидатора только для целых чисел
    QIntValidator* intValidator = new QIntValidator(this);
    // Применение валидатора для текстовых полей
    ui->input_x->setValidator(intValidator);
    ui->input_y->setValidator(intValidator);
    ui->input_square_size->setValidator(intValidator);
    // Соединение сигналов со слотами
    connect(ui->input_x, SIGNAL(textChanged(QString)), this, SLOT(on_input_x_textChanged(QString)));
    connect(ui->input_y, SIGNAL(textChanged(QString)), this, SLOT(on_input_y_textChanged(QString)));
    connect(ui->input_square_size, SIGNAL(textChanged(QString)), this, SLOT(on_input_square_size_textChanged(QString)));

    // Устанавливается значение по умолчанию для переменной size (из слота on_input_square_size_textChanged)
    ui->input_square_size->setText("20");
}

MainWindow::~MainWindow()
{
    // Деструктор - вызывается автоматически при удалении класса (при закрытии окна MainWindow)
    delete ui;
}

void MainWindow::on_input_x_textChanged(const QString &newValue)
{
    // Изменение значения переменной x из слота on_input_x_textChanged
    x = newValue.toInt();
}

void MainWindow::on_input_y_textChanged(const QString &newValue)
{
    // Изменение значения переменной y из слота on_input_y_textChanged.
    y = newValue.toInt();
}

void MainWindow::on_input_square_size_textChanged(const QString &newValue)
{
    // Изменение значения переменной size из слота on_input_square_size_textChanged.
    size = newValue.toInt();
}

void MainWindow::on_draw_grid_button_clicked()
{
    // Очистка сцены и сбрасывание ее размеров
    scene->clear();
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);

    for (int i = 1; i < (x - 1); i++) {

        for (int j = 1; j < (y - 1); j++) {
            // Заполнение пустой сцены
            scene->addRect(QRectF(i * size, -j * size, size, size), QPen(Qt::black), QBrush(Qt::cyan));
        }
    }
}

void MainWindow::on_load_button_clicked()
{
    // Открытие диалогового окна для выбора файла
    QString fileName = QFileDialog::getOpenFileName(this,
        "Выберите файл", "",
        "Текстовые файлы (*.dat);; Текстовые файлы (*.txt);; Текстовые файлы (*.json);");

    // Проверка, был ли выбран файл
    if (fileName.isEmpty())
        return;

    // Открытие файла для чтения
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;

    // Считывание и вывод содержимого файла
    QTextStream in(&file);

    // Выделение памяти для двухмерного динамического массива
    FList.resize(x);
    for (int i = 0; i < x; i++) {
        FList[i].resize(y);
    }

    // Чтение данных из файла и сохранение их в массиве
    for (int i = 0; i < x; i++) {

        for (int j = 0; j < y; j++) {
            // Записывание значения в соответствующую ячейку двухмерного вектора
            in >> FList[i][j];
        }
    }

    // Закрытие файла
    file.close();

    // Вызов функции для отрисовки свободной поверхности
    drawVoF();
}

void MainWindow::drawVoF()
{
    double Nx; // Нормаль по оси X;
    double Ny; // Нормаль по оси Y;
    double alpha; // Угол отклонения доли жидкости альфа;
    const double eps = pow(10, -30); // Малая величина - для устранения вычислительной сингулярности;
    int CS; // Значения от 1 до 4 - соответствует четырем основным положениям свободной поверхности;
    double ST; // Точка находящихся на верхней границе ячейки;
    double SR; // Точка находящаяся на правой границе ячейки;
    double SB; // Точка находящаяся на нижнец границе ячейки;
    double SL; // Точка находящаяся на левой границе ячейки;

    // Создание переменных для рисования контура и заливки фигуры
    QPen pen(Qt::black);
    QBrush brush(Qt::blue);

    for(int i = 1; i < (x - 1); i++) {

        for(int j = 1; j < (y - 1); j++) {
            const int xPosition = i * size; // Координаты текущей ячейки по оси X;
            const int yPosition = j * size; // Координаты текущей ячейки по оси Y;

            // Вычисление значений нормалей
            Nx = -(FList[i + 1][j + 1] + 2 * FList[i + 1][j] + FList[i + 1][j - 1] -
                    FList[i - 1][j + 1] - 2 * FList[i - 1][j] - FList[i - 1][j - 1]);
            Ny = -(FList[i + 1][j + 1] + 2 * FList[i][j + 1] + FList[i - 1][j + 1] -
                    FList[i - 1][j - 1] - 2 * FList[i][j - 1] - FList[i + 1][j - 1]);

            // Вычисление значений альфа
            if ((Nx >= 0 && Ny >= 0) || (Nx <= 0 && Ny <= 0)) {
                alpha = M_PI_2 - fabs(atan(Ny / (Nx + eps)));
            }
            else {
                alpha = fabs(atan(Nx / (Ny + eps)));
            }

            // F = 1 - полностью заполненный контрольный объём
            if (FList[i][j] == 1) {
                scene->addRect(
                    QRectF(xPosition, -yPosition, size, size),
                    pen, brush);
            }

            // Определение типа положения свободной поверхности
            if (alpha < M_PI_4) {

                if (FList[i][j] < 0.5 * tan(alpha)) {
                    CS=1;
                }
                else if (FList[i][j] < 1 - 0.5 * tan(alpha)) {
                    CS=2;
                }
                else {
                    CS=4;
                }
            }
            else if (alpha > M_PI_4) {

                if (FList[i][j] < 0.5 / tan(alpha)) {
                    CS=1;
                }
                else if (FList[i][j] < (1 - 0.5 / tan(alpha))) {
                    CS=3;
                }
                else {
                    CS=4;
                }
            }


            switch (CS) {

                case 1:

                if ((Nx < 0.0) && (Ny >= 0.0)) {
                    SB = sqrt(2.0 * FList[i][j] * size * size / tan(alpha));
                    SR = sqrt(2.0 * FList[i][j] * size * size * tan(alpha));

                    if (SB || SR > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition -SB + size, -yPosition + size)
                            <<QPointF(xPosition + size, -yPosition - SR + size)
                            <<QPointF(xPosition + size, -yPosition + size),
                            pen, brush);
                    }
                }

                if ((Nx < 0.0) && (Ny < 0.0)) {
                    ST = sqrt(2.0 * FList[i][j] * size * size / tan(alpha));
                    SR = sqrt(2.0 * FList[i][j] * size * size * tan(alpha));

                    if (ST > 0 || SR > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition - ST + size, -yPosition)
                            <<QPointF(xPosition + size, -yPosition)
                            <<QPointF(xPosition + size, -yPosition + SR),
                            pen, brush);
                    }
                }

                if ((Nx >= 0.0) && (Ny < 0.0)) {
                    ST = sqrt(2.0 * FList[i][j] * size * size / tan(alpha));
                    SL = sqrt(2.0 * FList[i][j] * size * size * tan(alpha));

                    if (ST > 0 || SL > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition + ST, -yPosition)
                            <<QPointF(xPosition, -yPosition + SL)
                            <<QPointF(xPosition, -yPosition),
                            pen, brush);
                    }
                }

                if ((Nx >= 0.0) && (Ny >= 0.0)) {
                    SB = sqrt(2.0 * FList[i][j] * size * size / tan(alpha));
                    SL = sqrt(2.0 * FList[i][j] * size * size * tan(alpha));

                    if (SB > 0 || SL > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition + SB, -yPosition + size)
                            <<QPointF(xPosition, -yPosition - SL + size)
                            <<QPointF(xPosition, -yPosition + size),
                            pen, brush);
                    }
                }

                break;

                case 2:

                if ((Nx < 0.0) && (Ny >= 0.0)) {
                    SL = FList[i][j] * size - 0.5 * size * tan(alpha);
                    SR = FList[i][j] * size + 0.5 * size * tan(alpha);

                    if (SL > 0 || SR > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition, -yPosition - SL + size)
                            <<QPointF(xPosition + size, -yPosition - SR + size)
                            <<QPointF(xPosition + size, -yPosition + size)
                            <<QPointF(xPosition, -yPosition + size),
                            pen, brush);
                    }
                }

                if ((Nx < 0.0) && (Ny < 0.0)) {
                    SL = FList[i][j] * size - 0.5 * size * tan(alpha);
                    SR = FList[i][j] * size + 0.5 * size * tan(alpha);

                    if (SL > 0 || SR > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition, -yPosition)
                            <<QPointF(xPosition, -yPosition + SL)
                            <<QPointF(xPosition + size, -yPosition + SR)
                            <<QPointF(xPosition + size, -yPosition),
                            pen, brush);
                    }
                }

                if ((Nx >= 0.0) && (Ny < 0.0)) {
                    SL = FList[i][j] * size + 0.5 * size * tan(alpha);
                    SR = FList[i][j] * size - 0.5 * size * tan(alpha);

                    if (SL > 0 || SR > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition, -yPosition)
                            <<QPointF(xPosition, -yPosition + SL)
                            <<QPointF(xPosition + size, -yPosition + SR)
                            <<QPointF(xPosition + size, -yPosition),
                            pen, brush);
                    }
                }

                if ((Nx >= 0.0) && (Ny >= 0.0)) {
                    SL = FList[i][j] * size + 0.5 * size * tan(alpha);
                    SR = FList[i][j] * size - 0.5 * size * tan(alpha);

                    if (SL > 0 || SR > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition, -yPosition - SL + size)
                            <<QPointF(xPosition + size, -yPosition - SR + size)
                            <<QPointF(xPosition + size, -yPosition + size)
                            <<QPointF(xPosition, -yPosition + size),
                            pen, brush);
                    }
                }

                break;

                case 3:

                if ((Nx < 0.0) && (Ny >= 0.0)) {
                    SB = FList[i][j] * size + 0.5 * size / tan(alpha);
                    ST = FList[i][j] * size - 0.5 * size / tan(alpha);

                    if (SB > 0 || ST > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition - ST + size, -yPosition)
                            <<QPointF(xPosition - SB + size, -yPosition + size)
                            <<QPointF(xPosition + size, -yPosition + size)
                            <<QPointF(xPosition + size, -yPosition),
                            pen, brush);
                    }
                }

                if ((Nx < 0.0) && (Ny < 0.0)) {
                    SB = FList[i][j] * size - 0.5 * size / tan(alpha);
                    ST = FList[i][j] * size + 0.5 * size / tan(alpha);

                    if (SB > 0 || ST > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition -ST + size, -yPosition)
                            <<QPointF(xPosition -SB + size, -yPosition + size)
                            <<QPointF(xPosition + size, -yPosition + size)
                            <<QPointF(xPosition + size, -yPosition),
                            pen, brush);
                    }
                }

                if ((Nx >= 0.0) && (Ny < 0.0)) {
                    SB = FList[i][j] * size - 0.5 * size / tan(alpha);
                    ST = FList[i][j] * size + 0.5 * size / tan(alpha);

                    if (SB > 0 || ST > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition + ST, -yPosition)
                            <<QPointF(xPosition + SB, -yPosition + size)
                            <<QPointF(xPosition, -yPosition + size)
                            <<QPointF(xPosition, -yPosition),
                            pen, brush);
                    }
                }

                if ((Nx >= 0.0) && (Ny >= 0.0)) {
                    SB = FList[i][j] * size + 0.5 * size / tan(alpha);
                    ST = FList[i][j] * size - 0.5 * size / tan(alpha);

                    if (SB > 0 || ST > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition + ST, -yPosition)
                            <<QPointF(xPosition + SB, -yPosition + size)
                            <<QPointF(xPosition, -yPosition + size)
                            <<QPointF(xPosition, -yPosition),
                            pen, brush);
                    }
                }

                break;

                case 4:

                if ((Nx < 0.0) && (Ny  > 0.0)) {
                    ST = size - sqrt(2.0 * (1.0 - FList[i][j]) * size * size / tan(alpha));
                    SL = size - sqrt(2.0 * (1.0 - FList[i][j]) * size * size * tan(alpha));

                    if (((size > ST) && (ST > 0)) || ((size > SL) && (SL > 0))) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition, -yPosition -SL + size)
                            <<QPointF(xPosition - ST + size, -yPosition)
                            <<QPointF(xPosition + size, -yPosition)
                            <<QPointF(xPosition + size, -yPosition + size)
                            <<QPointF(xPosition, -yPosition + size),
                            pen, brush);
                    }
                }

                if ((Nx < 0.0) && (Ny < 0.0)) {
                    SL = size - sqrt(2.0 * (1.0 - FList[i][j]) * size * size * tan(alpha));
                    SB = size - sqrt(2.0 * (1.0 - FList[i][j]) * size * size / tan(alpha));

                    if (SL > 0 || SB > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition -SB + size, -yPosition + size)
                            <<QPointF(xPosition, -yPosition + SL)
                            <<QPointF(xPosition, -yPosition)
                            <<QPointF(xPosition + size, -yPosition)
                            <<QPointF(xPosition + size, -yPosition + size),
                            pen, brush);
                    }
                }

                if ((Nx >= 0.0) && (Ny < 0.0)) {
                    SB = size - sqrt(2.0 * (1.0 - FList[i][j]) * size * size / tan(alpha));
                    SR = size - sqrt(2.0 * (1.0 - FList[i][j]) * size * size * tan(alpha));

                    if (SR > 0 || SB > 0) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition, -yPosition + size)
                            <<QPointF(xPosition, -yPosition)
                            <<QPointF(xPosition + size, -yPosition)
                            <<QPointF(xPosition + size, -yPosition + SR)
                            <<QPointF(xPosition + SB, -yPosition + size),
                            pen, brush);
                    }
                }

                if ((Nx >= 0.0) && (Ny >= 0.0)) {
                    SR = size - sqrt(2.0 * (1.0 - FList[i][j]) * size * size * tan(alpha));
                    ST = size - sqrt(2.0 * (1.0 - FList[i][j]) * size * size / tan(alpha));

                    if (((size > SR) && (SR > 0)) || ((size > ST) && (ST > 0))) {
                        QPolygonF polygon;
                        scene->addPolygon(polygon
                            <<QPointF(xPosition, -yPosition + size)
                            <<QPointF(xPosition, -yPosition)
                            <<QPointF(xPosition + ST, -yPosition)
                            <<QPointF(xPosition + size, -yPosition - SR + size)
                            <<QPointF(xPosition + size, -yPosition + size),
                            pen, brush);
                    }
                }

                break;
            }
        }
    }
}
