#include "filesetter.h"
#include "ui_filesetter.h"
#include "mainwindow.h"

#include "iostream"

using namespace std;

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
    connect(ui->pb_viewtoolpath, &QPushButton::clicked, this, &FileSetter::selectFile);
    connect(ui->pb_viewqmlpath, &QPushButton::clicked, this, &FileSetter::selectPath);

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
    }

    ui->le_name->setText("");
    ui->le_toolpath->setText("");
    ui->le_qmlpath->setText("");

    writeToCfg();

    emit refreshCfg();
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

    QString toolPath = getToolPathByName(selectedLabel->text());
    QString qmlPath = getQmlPathByName(selectedLabel->text());
    if(selectedLabel->text() != "new data")
    {
        ui->le_name->setText(selectedLabel->text());
    }
    else
    {
        ui->le_name->setText("");
    }
    ui->le_toolpath->setText(toolPath);
    ui->le_qmlpath->setText(qmlPath);
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

/**
 * @brief FileSetter::getDatasFromCfg
 * @return
 * delete等内存操作全部在此进行
 */
QList<QString> *  FileSetter::getDatasFromCfg()
{
    QString cfgContent;
    QFile fileCfg(cfgPath);
    if (!fileCfg.exists())
    {
        checkCfgPath();
    }
    if(!fileCfg.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return nullptr;
    }

    datas = new QList<QString>();
    while(!fileCfg.atEnd())
    {
        datas->push_back(fileCfg.readLine().trimmed());
    }

    //labelList = *new QList<CanClickedQLabel *>();       //可以优化, 不要删除全部label在增加回来, 而是增加没有的部分
    foreach(CanClickedQLabel * label, labelList)
    {
        if(label)
        {
            labelList.removeOne(label);
            delete label;
            label=nullptr;
        }
    }
    selectedLabel = nullptr;

    for(QString data : *datas)
    {
        QString name = data.split(",")[0];
        CanClickedQLabel * newLabel = addLine();
        if(newLabel != nullptr)
        {
            newLabel->setText(name);
            selectLabel(newLabel);
        }
    }

    if(labelList.count() == 0)
    {
        addLine();
    }

    return datas;
}

QString FileSetter::getToolPathByName(QString name)
{
    QString toolPath;
    for(QString data : *datas)
    {
        QString thisDataName = data.split(',')[0];
        if(thisDataName == name)
        {
            QString dataTemp = data;
            toolPath = dataTemp.split(',')[1];
            return toolPath;
        }
    }
    return nullptr;
}

QString FileSetter::getQmlPathByName(QString name)
{
    QString qmlPath;
    for(QString data : *datas)
    {
        QString thisDataName = data.split(',')[0];
        if(thisDataName == name)
        {
            QString dataTemp = data;
            if(dataTemp.split(',').count() == 3)
            {
                qmlPath = dataTemp.split(',')[2];
            }
            else
            {
                qmlPath = "";
            }
            return qmlPath;
        }
    }
    return nullptr;
}

bool FileSetter::saveDataToCfg()
{
    if(ui->le_name->text() == "" || ui->le_toolpath->text() == "" || ui->le_name->text() == "new data" || selectedLabel == nullptr)
    {
        return false;
    }
    QString selectedName = selectedLabel->text().trimmed();
    QString newName = ui->le_name->text().trimmed();

    for (int i = 0; i < datas->count(); ++i)
    {
        if(newName != "" && selectedName == datas->at(i).split(',')[0])
        {
            datas->removeAt(i);
            break;
        }
        if(newName == datas->at(i).split(',')[0])
        {
            return false;
        }
    }

    if (ui->le_qmlpath->text().trimmed() != "")
    {
        datas->push_back(ui->le_name->text().trimmed() + "," + ui->le_toolpath->text().trimmed() + "," + ui->le_qmlpath->text().trimmed());
    }
    else
    {
        datas->push_back(ui->le_name->text().trimmed() + "," + ui->le_toolpath->text().trimmed());
    }

    if(writeToCfg())
    {
        emit refreshCfg();
    }
    else
    {
        return false;
    }
    if(selectedLabel != nullptr)
    {
        selectedLabel->setText(ui->le_name->text());
    }

    getDatasFromCfg();
    return true;
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

    return true;
}

void FileSetter::reSet()
{
    ui->le_name->setText("");
    ui->le_toolpath->setText("");
    ui->le_qmlpath->setText("");
}

void FileSetter::selectFile()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("选择windeployqt.exe的路径"));
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
        ui->le_toolpath->setText(filePath[0]);
    }
    else
    {
        return;
    }
}

void FileSetter::selectPath()
{
    QString qmlPath = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), "./", QFileDialog::ShowDirsOnly);
    if (!qmlPath.isEmpty())
    {
        ui->le_qmlpath->setText(qmlPath);
    }
}

FileSetter::~FileSetter()
{
    delete ui;
}
