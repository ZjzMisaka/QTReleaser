#include "filesetter.h"
#include "ui_filesetter.h"

FileSetter::FileSetter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileSetter)
{
    ui->setupUi(this);

    Qt::WindowFlags flag = nullptr;
    setWindowFlags(flag); // 设置禁止最大化
    setFixedSize(this->width(),this->height()); // 禁止改变窗口大小。

    labelList = *new QList<CanClickedQLabel *>();
    selectedLabel = nullptr;

    connect(ui->pb_add, &QPushButton::clicked, this, &FileSetter::addLine);
    connect(ui->pb_delete, &QPushButton::clicked, this, &FileSetter::deleteLine);
    connect(ui->pb_reset, &QPushButton::clicked, this, &FileSetter::reSet);
    connect(ui->pb_save, &QPushButton::clicked, this, &FileSetter::saveDataToCfg);
    connect(ui->pb_viewpath, &QPushButton::clicked, this, &FileSetter::selectFile);

    getDatasFromCfg();
}

CanClickedQLabel * FileSetter::addLine()
{
    if(selectedLabel != nullptr && selectedLabel->text() == "new data")
    {
        return nullptr;
    }
    labelList = ui->scrollArea->findChildren<CanClickedQLabel *>();
    if(labelList.count() <= 9)
    {
        reSet();
        label = new CanClickedQLabel("new data");
        connect(label, &CanClickedQLabel::clicked, this, &FileSetter::selectLabel);
        ui->verticalLayout->setMargin(5);
        ui->verticalLayout->setSpacing(0);
        ui->verticalLayout->insertWidget(0, label);
        ui->verticalLayout->addStretch();
        labelList.push_back(label);
        selectLabel(label);
        return label;
    }
    return nullptr;
}

void FileSetter::deleteLine()
{
    CanClickedQLabel * temp;

    if(selectedLabel->text() == "new data")
    {
        temp = selectedLabel;
        delete temp;
        selectedLabel = nullptr;
        return;
    }

    if(selectedLabel != nullptr)
    {
        QString name = selectedLabel->text();
        for(int i = 0; i < datas->count(); ++i)
        {
            if(name == datas->at(i).split(',')[0])
            {
                datas->removeAt(i);
                break;
            }
        }

        temp = selectedLabel;
        delete temp;
        selectedLabel = nullptr;
    }

    ui->le_name->setText("");
    ui->le_path->setText("");

    writeToCfg();
}

void FileSetter::selectLabel(CanClickedQLabel *selectedLabel)
{
    QList<CanClickedQLabel *> labelList = ui->scrollArea->findChildren<CanClickedQLabel *>();
    for (CanClickedQLabel * i : labelList)
    {
        i->setStyleSheet("background-color: none");
    }
    this->selectedLabel = selectedLabel;
    selectedLabel->setStyleSheet("background-color: #BBFFFF");

    QString path = getPathByName(selectedLabel->text());
    if(selectedLabel->text() != "new data")
    {
        ui->le_name->setText(selectedLabel->text());
    }
    else
    {
        ui->le_name->setText("");
    }
    ui->le_path->setText(path);
}

void FileSetter::checkCfgPath()
{
    QFile fileCfg(cfgPath);
    if (!fileCfg.exists())
    {
        fileCfg.open(QIODevice::WriteOnly);
        fileCfg.close();
    }
}

void FileSetter::getDatasFromCfg()
{
    QString cfgContent;
    QFile fileCfg(cfgPath);
    if (!fileCfg.exists())
    {
        checkCfgPath();
    }
    if(!fileCfg.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    datas = new QList<QString>();
    while(!fileCfg.atEnd())
    {
        datas->push_back(fileCfg.readLine());
    }

    labelList = *new QList<CanClickedQLabel *>();       //可以优化, 不要删除全部label在增加回来, 而是增加没有的部分

    for(QString data : *datas)
    {
        QString name = data.split(",")[0];
        QString path = data.split(",")[1];
        CanClickedQLabel * newLabel = addLine();
        newLabel->setText(name);
        selectLabel(newLabel);
    }

    if(labelList.count() == 0)
    {
        addLine();
    }
}

QString FileSetter::getPathByName(QString name)
{
    QString path;
    for(QString data : *datas)
    {
        QString thisDataName = data.split(',')[0];
        if(thisDataName == name)
        {
            QString dataTemp = data;
            int index = dataTemp.indexOf(name) + name.length() + 1;
            path = dataTemp.remove(0, index);
            return path;
        }
    }
    return nullptr;
}

bool FileSetter::saveDataToCfg()
{
    if(ui->le_name->text() == "" || ui->le_path->text() == "" || ui->le_name->text() == "new data" || selectedLabel == nullptr)
    {
        return false;
    }
    QString name = ui->le_name->text();
    for(QString data : *datas)
    {
        if(name != data.split(',')[0])
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    datas->push_back(name + "," + ui->le_path->text());

    if(writeToCfg())
        return true;
    else
        return false;
}

bool FileSetter::writeToCfg()
{
    QFile fileCfg(cfgPath);
    if (!fileCfg.exists())
    {
        checkCfgPath();
    }
    else
    {
        if(!fileCfg.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            return false;
        }

        QTextStream txtOutput(&fileCfg);

        for(QString data : *datas)
        {
            txtOutput << data << endl;
        }

        fileCfg.close();
    }

    if(selectedLabel != nullptr)
    {
        selectedLabel->setText(ui->le_name->text());
    }

    return true;
}

void FileSetter::reSet()
{
    ui->le_name->setText("");
    ui->le_path->setText("");
}

void FileSetter::selectFile()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("选择SS路径"));
    //设置默认文件路径
    fileDialog->setDirectory("./");
    //设置文件过滤器
    fileDialog->setNameFilter(tr("exe(*.exe)"));
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    QStringList filePath;
    if(!fileDialog->exec())
    {
        return;
    }
    filePath = fileDialog->selectedFiles();
    if(filePath[0].contains("windeployqt.exe"))
    {
        ui->le_path->setText(filePath[0]);
    }
    else
    {
        return;
    }
}

FileSetter::~FileSetter()
{
    delete ui;
}
