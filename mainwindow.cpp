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
    QPainter painter(this);

    QImage img = engine();

    painter.drawImage(5, 5, img);
}

MainWindow::~MainWindow()
{
    delete ui;
}
