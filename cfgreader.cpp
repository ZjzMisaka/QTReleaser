#include "cfgreader.h"
#include "ui_cfgreader.h"

CfgReader::CfgReader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CfgReader)
{
    ui->setupUi(this);

    Qt::WindowFlags flag = nullptr;
    setWindowFlags(flag); // 设置禁止最大化
    setFixedSize(this->width(),this->height()); // 禁止改变窗

    QFile file("./config.txt");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    cfgContent = file.readAll();

    ui->te_cfg->setText(cfgContent);

    connect(ui->pb_save, &QPushButton::clicked, this, &CfgReader::saveCfg);
}

void CfgReader::saveCfg()
{
    QFile cfgFile("./config.txt");
    if (!cfgFile.exists())
    {
        return;
    }
    else
    {
        if(!cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            return;
        }

        QTextStream txtOutput(&cfgFile);

        txtOutput << ui->te_cfg->toPlainText();

        cfgFile.close();

        emit refreshCfg();
    }

    return;
}

CfgReader::~CfgReader()
{
    delete ui;
}
