#include "filesetter.h"
#include "ui_filesetter.h"
#include "mainwindow.h"

#include "QDebug"

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
    successCount = 0;
    defaultRootPath = QCoreApplication::applicationDirPath().mid(0, 3);
    fileController = new FileController();
    fileController->moveToThread(&fileControllerThread);

    ui->label_autosetrunningnowpath->setText(defaultRootPath);

    connect(ui->pb_add, &QPushButton::clicked, this, &FileSetter::addLine);
    connect(ui->pb_delete, &QPushButton::clicked, this, &FileSetter::deleteLine);
    connect(ui->pb_reset, &QPushButton::clicked, this, &FileSetter::reSet);
    connect(ui->pb_save, &QPushButton::clicked, this, &FileSetter::saveDataToCfg);
    connect(ui->pb_viewtoolpath, &QPushButton::clicked, this, &FileSetter::selectFile);
    connect(ui->pb_viewqmlpath, &QPushButton::clicked, this, &FileSetter::selectPath);
    connect(ui->pb_autoset, &QPushButton::clicked, this, &FileSetter::autoSet);
    connect(this, &FileSetter::findFileInPath, fileController, &FileController::findFileInPath);
    connect(fileController, &FileController::setSchedule, this, &FileSetter::setSchedule);
    connect(fileController, &FileController::setResult, this, &FileSetter::setResult);
    connect(&fileControllerThread, &QThread::finished, fileController, &QObject::deleteLater);

    fileControllerThread.start();

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
        ui->verticalLayout->setContentsMargins(3, 5, 5, 5);
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
    labelList = ui->scrollArea->findChildren<CanClickedQLabel *>();
    for (CanClickedQLabel * i : labelList)
    {
        i->setStyleSheet("background-color: none");
    }
    this->selectedLabel = selectedLabel;
    selectedLabel->setStyleSheet("background-color: #0078d7; color: #F0F0F0;");

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
    labelList = ui->scrollArea->findChildren<CanClickedQLabel *>();
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
    QString newToolPath = ui->le_toolpath->text().trimmed();
    QString newQmlPath = ui->le_qmlpath->text().trimmed();

    for (int i = 0; i < datas->count(); ++i)
    {
        QString currentNameNeedCompare = "";
        QString currentToolPathNeedCompare = "";
        QString currentQmlPathNeedCompare = "";
        if (datas->at(i).split(',').count() >= 1)
        {
            currentNameNeedCompare = datas->at(i).split(',')[0];
        }
        if (datas->at(i).split(',').count() >= 2)
        {
            currentToolPathNeedCompare = datas->at(i).split(',')[1];
        }
        if (datas->at(i).split(',').count() >= 3)
        {
            currentQmlPathNeedCompare = datas->at(i).split(',')[2];
        }

        //重命名选中条
        if(newName != "" && selectedName == currentNameNeedCompare)
        {
            datas->removeAt(i);
            break;
        }
        //新名字重复则忽视
        if(newName == currentNameNeedCompare || (newToolPath == currentToolPathNeedCompare && newQmlPath == currentQmlPathNeedCompare))
        {
            return false;
        }
    }

    if (ui->le_qmlpath->text().trimmed() != "")
    {
        datas->push_back(ui->le_name->text().trimmed() + "," + ui->le_toolpath->text().trimmed() + "," + ui->le_qmlpath->text().trimmed());
        ++successCount;
    }
    else
    {
        datas->push_back(ui->le_name->text().trimmed() + "," + ui->le_toolpath->text().trimmed());
        ++successCount;
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

void FileSetter::autoSet(int step)
{
    if(step == 0)
    {
        QString path = QFileDialog::getExistingDirectory(this, tr("选择包含windeployqt.exe的文件夹"), defaultRootPath, QFileDialog::ShowDirsOnly);
        emit findFileInPath(path, "windeployqt.exe");
    }
    else if (step == 1)
    {
        successCount = 0;

        for (QString fullName : fullNameList)
        {
            QString fullNameTemp = fullName;
            QString typePath = fullNameTemp.remove("/bin/windeployqt.exe");
            QString  type = typePath.mid(typePath.lastIndexOf('/') + 1);

            //qDebug()<< fullName + ", " + type;

            addLine();
            ui->le_name->setText(type);
            ui->le_toolpath->setText(fullName);
            saveDataToCfg();
        }

        if(selectedLabel->text() == "new data")
        {
            deleteLine();
            getDatasFromCfg();
        }

        ui->label_autosetrunningnowpath->setText(QString::fromStdString("自动录入成功: 共" + to_string(successCount) + "条"));
    }
}

void FileSetter::setSchedule(QString schedule)      //最好增加百分比进度
{
    ui->label_autosetrunningnowpath->setText(schedule);
}

void FileSetter::setResult(QList<QString> result)
{
    fullNameList = result;
    autoSet(1);
}

FileSetter::~FileSetter()
{
    fileControllerThread.quit();
    fileControllerThread.wait();
    fileControllerThread.exit();
    //delete &fileController;           //要用这个
    //delete &fileControllerThread;
    delete ui;
}
