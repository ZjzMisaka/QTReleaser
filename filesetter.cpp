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
    successCount = 0;
    failCount = 0;
    defaultRootPath = QCoreApplication::applicationDirPath().mid(0, 3);
    fileController = new FileController();
    fileController->moveToThread(&fileControllerThread);

    ui->label_info->setText(defaultRootPath);

    connect(ui->pb_add, &QPushButton::clicked, this, &FileSetter::addLine);
    connect(ui->pb_delete, &QPushButton::clicked, this, &FileSetter::deleteLine);
    connect(ui->pb_reset, &QPushButton::clicked, this, &FileSetter::reSet);
    connect(ui->pb_save, &QPushButton::clicked, this, &FileSetter::saveDataToCfg);
    connect(ui->pb_viewtoolpath, &QPushButton::clicked, this, &FileSetter::selectToolPath);
    connect(ui->pb_viewqmlpath, &QPushButton::clicked, this, &FileSetter::selectQmlPath);
    connect(ui->pb_autoset, &QPushButton::clicked, this, &FileSetter::setParaAndTransferAutoSet);
    connect(this, &FileSetter::findFileInPath, fileController, &FileController::findFileInPath);
    connect(fileController, &FileController::setSchedule, this, &FileSetter::getSchedule);
    connect(fileController, &FileController::finishAutoSet, this, &FileSetter::OnAutoSetFinished);
    connect(this, &FileSetter::stopFileControllerThread, fileController, &FileController::stopThread, Qt::DirectConnection);

    getDatasFromCfg();
}

/**
 * @brief FileSetter::addLine
 * @return
 * 增加一个新的配置label并选中
 */
CanClickedQLabel * FileSetter::addLine()
{
    if(labelList.count() >= 1)
    {
        selectLabel(labelList.last());
    }
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

/**
 * @brief FileSetter::deleteLine
 * 删除一个配置label, 并在配置文件中删除对应行
 */
void FileSetter::deleteLine()
{
    CanClickedQLabel * temp;

    if(selectedLabel != nullptr && selectedLabel->text() == "new data")
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

        ui->le_name->setText("");
        ui->le_toolpath->setText("");
        ui->le_qmlpath->setText("");

        writeToCfg();

        emit refreshCfg();
    }
}

/**
 * @brief FileSetter::selectLabel
 * @param selectedLabel
 * 选中高亮一个配置label
 */
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

/**
 * @brief FileSetter::checkCfgPath
 * 确认配置文件是否存在, 不存在则新建一个
 */
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
 * 从配置文件获取配置, 并对应更新界面
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

    //从配置文件中获取data
    datas = new QList<QString>();
    while(!fileCfg.atEnd())
    {
        datas->push_back(fileCfg.readLine().trimmed());
    }

    //labelList = *new QList<CanClickedQLabel *>();       //可以优化, 不要删除全部label在增加回来, 而是增加没有的部分
    //获取所有的配置label, 并全部删除
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

    //根据从配置文件获得的data创建新的配置label
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

    //如果没有配置, 也即没有label被新增出来, 则增加一行label
    if(labelList.count() == 0)
    {
        addLine();
    }

    return datas;
}

/**
 * @brief FileSetter::getToolPathByName
 * @param name
 * @return toolPath
 * 根据配置名从配置文件得到编译器路径
 */
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

/**
 * @brief FileSetter::getQmlPathByName
 * @param name
 * @return qmlPath
 * 根据配置名从配置文件得到QML安装目录, 如果不存在则留空
 */
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

/**
 * @brief FileSetter::saveDataToCfg
 * @return boolValue
 * 保存输入的信息至配置文件, 并相应修改界面
 */
bool FileSetter::saveDataToCfg()
{
    QString selectedName = selectedLabel->text().trimmed();
    QString newName = ui->le_name->text().trimmed();
    QString newToolPath = ui->le_toolpath->text().trimmed();
    QString newQmlPath = ui->le_qmlpath->text().trimmed();

    if(selectedLabel == nullptr)
    {
        ui->label_info->setText("保存失败, 请先添加新的配置. ");
        return false;
    }
    if(newName == "" || newToolPath == "" || newName == "new data")
    {
        ui->label_info->setText("保存失败, 信息不完整. ");
        return false;
    }
    QDir dir(newToolPath + "bin/");
    if(!dir.exists())
    {
        ui->label_info->setText("编译器路径不合法. ");
        return false;
    }

    for (int i = 0; i < datas->count(); ++i)
    {
        QString currentNameNeedCompare = "";
        QString currentToolPathNeedCompare = "";
        QString currentQmlPathNeedCompare = "";
        //分割datas
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

        //重命名选中条. 当输入的新配置名不为空, 并遍历到了当前选中label的配置信息, 暂时删除当前配置并在下面重新写入
        if(newName != "" && selectedName == currentNameNeedCompare)
        {
            datas->removeAt(i);
            break;
        }
        //新名字重复则忽视
        if(newName == currentNameNeedCompare || (newToolPath == currentToolPathNeedCompare && newQmlPath == currentQmlPathNeedCompare))
        {
            ui->label_info->setText("保存失败, 配置名重复. ");
            ++failCount;
            return false;
        }
    }

    //准备要写入到配置文件中的信息
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

    //写入配置文件
    if(writeToCfg())
    {
        emit refreshCfg();
    }
    else
    {
        ui->label_info->setText("配置写入失败. ");
        return false;
    }

    //修改选中的label的内容
    if(selectedLabel != nullptr)
    {
        selectedLabel->setText(ui->le_name->text());
    }

    getDatasFromCfg();
    ui->label_info->setText("保存成功. ");
    return true;
}

/**
 * @brief FileSetter::writeToCfg
 * @return boolValue
 * 写入配置文件
 */
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

/**
 * @brief FileSetter::reSet
 * 重置用户输入区
 */
void FileSetter::reSet()
{
    ui->le_name->setText("");
    ui->le_toolpath->setText("");
    ui->le_qmlpath->setText("");
}

/**
 * @brief FileSetter::selectToolPath
 * 选择编译器路径
 */
void FileSetter::selectToolPath()
{
    QString toolPath = QFileDialog::getExistingDirectory(this, tr("选择编译器路径"), "./", QFileDialog::ShowDirsOnly);
    ui->le_toolpath->setText(toolPath);
}

/**
 * @brief FileSetter::selectQmlPath
 * 选择QML安装路径
 */
void FileSetter::selectQmlPath()
{
    QString qmlPath = QFileDialog::getExistingDirectory(this, tr("选择QML安装路径"), "./", QFileDialog::ShowDirsOnly);
    if (!qmlPath.isEmpty())
    {
        ui->le_qmlpath->setText(qmlPath);
    }
}

/**
 * @brief FileSetter::setParaAndTransferAutoSet
 * 判断当点击按钮时值为结束录入还是自动录入, 并据此调用autoSet的步骤
 */
void FileSetter::setParaAndTransferAutoSet()
{
    if(ui->pb_autoset->text() == "结束录入")
    {
        autoSet(1, true);
    }
    else if(ui->pb_autoset->text() == "自动录入")
    {
        autoSet(0);
    }
}

/**
 * @brief FileSetter::autoSet
 * @param step
 * 自动查找并录入新配置
 * 步骤0: 开始录入, 步骤1: 结束录入
 */
void FileSetter::autoSet(int step, bool manualStop)
{
    if(step == 0)
    {
        failCount = 0;
        successCount = 0;
        QString path = QFileDialog::getExistingDirectory(this, tr("选择包含windeployqt.exe的文件夹"), defaultRootPath, QFileDialog::ShowDirsOnly);
        if(path == "")
        {
            ui->label_info->setText("未选择路径");
            return;
        }

        //开始线程
        fileControllerThread.start();
        emit findFileInPath(path, "windeployqt.exe");
        ui->pb_autoset->setText("结束录入");
    }
    else if (step == 1)
    {
        //如果是手动点击按钮停止, 则先停止录入线程
        if(manualStop)
        {
            finishFileControllerThread();
        }
        ui->label_info->setText(QString::fromStdString("录入结束: 共成功" + to_string(successCount) + "条, 失败" + to_string(failCount) + "条."));
        ui->pb_autoset->setText("自动录入");
    }
}

/**
 * @brief FileSetter::getSchedule
 * @param schedule
 * @param isNameSame
 * 得到当前录入进度, 如果该进度是符合要求的路径, 则录入
 */
void FileSetter::getSchedule(QString schedule, bool isNameSame)
{
    ui->label_info->setText(schedule);
    if(isNameSame)
    {
        QString scheduleTemp = schedule;
        QString typePath = scheduleTemp.remove("/bin/windeployqt.exe");
        QString  type = typePath.mid(typePath.lastIndexOf('/') + 1);
        typePath = typePath + "/";
        if (type.length() > ui->le_name->maxLength())
        {
            type = type.remove("_");
        }

        if (selectedLabel != nullptr && selectedLabel->text() != "new data" && addLine() == nullptr)
        {
            ++failCount;
            return;
        }
        ui->le_toolpath->setText(typePath);
        ui->le_name->setText(type);
        saveDataToCfg();

        if(selectedLabel->text() == "new data")
        {
            deleteLine();
            getDatasFromCfg();
        }
    }
}

/**
 * @brief FileSetter::OnAutoSetFinished
 * 录入全部结束, 调用autoSet步骤1
 */
void FileSetter::OnAutoSetFinished()
{
    autoSet(1);
}

/**
 * @brief FileSetter::closeEvent
 * @param event
 * 在关闭窗口前停止录入线程
 */
void FileSetter::closeEvent(QCloseEvent *event)
{
    event->ignore();
    ui->label_info->setText("等待线程被安全关闭");
    finishFileControllerThread();
    event->accept();
}

/**
 * @brief FileSetter::finishFileControllerThread
 * 停止录入线程
 */
void FileSetter::finishFileControllerThread()
{
    emit stopFileControllerThread();
    //获得结果后停止线程以便销毁
    fileControllerThread.quit();
    fileControllerThread.wait();
}

FileSetter::~FileSetter()
{
    delete ui;
}
