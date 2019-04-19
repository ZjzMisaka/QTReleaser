#include "output.h"
#include "ui_output.h"

Output::Output(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Output)
{
    ui->setupUi(this);

    Qt::WindowFlags flag = nullptr;
    setWindowFlags(flag); // 设置禁止最大化
    setFixedSize(this->width(),this->height()); // 禁止改变窗口大小。

}

/**
 * @brief Output::setText
 * @param text
 * 设置输出窗口里的值
 */
void Output::setText(QString text)
{
    ui->te_output->setText(text);
}

Output::~Output()
{
    delete ui;
}
