#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "engine.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    (void) event;
    const int border = 5;
    const int render_width = this->width() - border * 2;
    const int render_height = this->height() - border * 2;

    QPainter painter(this);
    if ((rendered.width() != render_width) ||
            (rendered.height() != render_height)) {
        render_time = clock();
        rendered = engine(render_width, render_height);
        render_time = clock() - render_time;
        render_time /= CLOCKS_PER_SEC;
    }
    painter.drawImage(border, border, rendered);
    painter.drawText(border, border + 10, std::to_string(render_time).c_str());
}

MainWindow::~MainWindow()
{
    delete ui;
}
