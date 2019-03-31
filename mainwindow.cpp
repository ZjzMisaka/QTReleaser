#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fileSetter = new FileSetter();

    Qt::WindowFlags flag = nullptr;
    setWindowFlags(flag); // 设置禁止最大化
    setFixedSize(this->width(),this->height()); // 禁止改变窗口大小。

    connect(ui->ac_setter, SIGNAL(triggered()), this, SLOT(openSetter()));
}

void MainWindow::openSetter()
{
    fileSetter->show();
}

MainWindow::~MainWindow()
{
    delete fileSetter;
    delete ui;
}
