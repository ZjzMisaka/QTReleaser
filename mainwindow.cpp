#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<QList<QString>>("QList<QString>");

    Qt::WindowFlags flag = nullptr;
    setWindowFlags(flag); // 设置禁止最大化
    setFixedSize(this->width(),this->height()); // 禁止改变窗口大小。

    fileSetter = new FileSetter();
    output = new Output();
    help = new Help();
    about = new About();
    setToolNameSelecter();
    listStr = "";
    verboseStr = "";
    isSimpleMode = false;
    needCopyList.append(toolPath + "bin/libgcc_s_seh-1.dll");
    needCopyList.append(toolPath + "bin/libgcc_s_dw2-1.dll");
    needCopyList.append(toolPath + "bin/libstdc++-6.dll");
    needCopyList.append(toolPath + "bin/libwinpthread-1.dll");
    needCopyList.append(toolPath + "bin/Qt5Core.dll");
    needCopyList.append(toolPath + "bin/Qt5Gui.dll");
    needCopyList.append(toolPath + "bin/Qt5Widgets.dll");
    needCopyList.append(toolPath + "plugins/platforms/qoffscreen.dll");
    needCopyList.append(toolPath + "plugins/platforms/qwindows.dll");
    needCopyList.append(toolPath + "plugins/styles/qwindowsvistastyle.dll");

    this->setAcceptDrops(true);

    connect(ui->action_opencfgfile, SIGNAL(triggered()), this, SLOT(openCfgFile()));
    connect(ui->action_opencfgpath, SIGNAL(triggered()), this, SLOT(openCfgPath()));
    connect(ui->action_help, SIGNAL(triggered()), this, SLOT(openHelp()));
    connect(ui->action_about, SIGNAL(triggered()), this, SLOT(openAbout()));
    connect(ui->ac_setter, SIGNAL(triggered()), this, SLOT(openSetter()));
    connect(ui->cb_selecttoolname, &QComboBox::currentTextChanged, this, &MainWindow::setPath);
    connect(ui->cb_selectprojecttypeorpackmode, &QComboBox::currentTextChanged, this, &MainWindow::setProjectTypeParameter);
    connect(ui->pb_setreleasepath, &QPushButton::clicked, this, &MainWindow::selectFile);
    connect(ui->pb_release, &QPushButton::clicked, this, &MainWindow::release);
    connect(ui->cb_cmd_1, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_2, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_3, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_4, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_5, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_6, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_7, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_8, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_9, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_10, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_11, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_12, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_13, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_14, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_15, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_16, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_17, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_18, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->cb_cmd_19, &QCheckBox::clicked, this, &MainWindow::setOtherParameterText);
    connect(ui->pb_dir, &QCheckBox::clicked, this, &MainWindow::selectDirPath);
    connect(ui->pb_libdir, &QCheckBox::clicked, this, &MainWindow::selectLibDirPath);
    connect(ui->pb_plugindir, &QCheckBox::clicked, this, &MainWindow::selectPluginDirPath);
    connect(ui->cb_list, &QComboBox::currentTextChanged, this, &MainWindow::comboBoxChanged);
    connect(ui->cb_verbose, &QComboBox::currentTextChanged, this, &MainWindow::comboBoxChanged);
    connect(ui->cb_simplemode, &QCheckBox::clicked, this, &MainWindow::changeMode);

    ui->cb_simplemode->click();
}

/**
 * @brief MainWindow::dragEnterEvent
 * @param event
 * 拖拽文件至界面上
 */
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

/**
 * @brief MainWindow::dropEvent
 * @param event
 * 在界面上释放文件
 */
void MainWindow::dropEvent(QDropEvent *event)
{
    QString releasePath = event->mimeData()->urls().first().toString().remove("file:///");
    ui->le_releasepath->setText(releasePath);
}

/**
 * @brief MainWindow::setToolNameSelecter
 * 读取配置文件, 并设置编译版本下拉框
 */
void MainWindow::setToolNameSelecter()
{
    datas = fileSetter->getDatasFromCfg();
    QList<QString> *  datasTemp = datas;

    int count = ui->cb_selecttoolname->count();
    for (int i = 1; i < count; --count)
    {
        ui->cb_selecttoolname->removeItem(i);
    }

    for(QString i : *datasTemp)
    {
        i = i.split(',')[0];
        ui->cb_selecttoolname->addItem(i);
    }
}

/**
 * @brief MainWindow::setPath
 * @param text
 * 在编译版本下拉框改变选择后调用, 根据选择的配置名获取编译器和QML路径, 判断是否是简单打包模式并根据选择设置项目类型下拉框
 */
void MainWindow::setPath(const QString & text)
{
    int count = ui->cb_selectprojecttypeorpackmode->count();
    for (int i = 1; i < count; --count)
    {
        ui->cb_selectprojecttypeorpackmode->removeItem(i);
    }
    if (text == "选择编译版本")
    {
        ui->cb_selectprojecttypeorpackmode->setEnabled(false);
        return;
    }
    toolPath = fileSetter->getToolPathByName(text);
    qmlPath = fileSetter->getQmlPathByName(text);

    if (!isSimpleMode)
    {
        ui->cb_selectprojecttypeorpackmode->setEnabled(true);
        if (toolPath != nullptr && toolPath != "")
        {
            ui->cb_selectprojecttypeorpackmode->addItem("Qt Widgets Application");
        }
        if (qmlPath != nullptr && qmlPath != "")
        {
            ui->cb_selectprojecttypeorpackmode->addItem("Qt Quick Application");
        }
    }
}

/**
 * @brief MainWindow::setProjectTypeParameter
 * @param text
 * 根据选择的项目类型初始化参数
 */
void MainWindow::setProjectTypeParameter(const QString & text)
{
    if (text == "Qt Widgets Application")
    {
        projectTypeParameter = "";
    }
    else if (text == "Qt Quick Application")
    {
        projectTypeParameter = "–qmldir " + qmlPath;
    }
    else
    {
        projectTypeParameter = "";
    }
}

/**
 * @brief MainWindow::selectFile
 * 选择待打包文件的路径
 */
void MainWindow::selectFile()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("选择待打包文件的路径"));
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
    ui->le_releasepath->setText(filePath[0]);
}

/**
 * @brief MainWindow::setOtherParameterText
 * 当其他参数的复选框被点击时统一调用, 得到调用者复选框并设置参数
 */
void MainWindow::setOtherParameterText()
{
    QCheckBox * checkBox =  qobject_cast<QCheckBox *>(sender());

    QString tempStr =  ui->le_otherparameter->text();
    tempStr = tempStr.replace(" --" + checkBox->text().replace(' ', "-"), "");
    ui->le_otherparameter->setText(tempStr);

    if (checkBox->isChecked())
    {
        ui->le_otherparameter->setText(ui->le_otherparameter->text() + " --" + checkBox->text().replace(' ', '-'));
    }
}

/**
 * @brief MainWindow::selectDirPath
 * 选择QML安装路径
 */
void MainWindow::selectDirPath()
{
    QString qmlPath = QFileDialog::getExistingDirectory(this, tr("选择QML安装路径"), "./", QFileDialog::ShowDirsOnly);
    if (!qmlPath.isEmpty())
    {
        ui->le_dir->setText(qmlPath);
    }
}

/**
 * @brief MainWindow::selectLibDirPath
 * 选择文件夹
 */
void MainWindow::selectLibDirPath()
{
    QString libDirPath = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), "./", QFileDialog::ShowDirsOnly);
    if (!libDirPath.isEmpty())
    {
        ui->le_libdir->setText(libDirPath);
    }
}

/**
 * @brief MainWindow::selectPluginDirPath
 * 选择文件夹
 */
void MainWindow::selectPluginDirPath()
{
    QString pluginDirPath = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), "./", QFileDialog::ShowDirsOnly);
    if (!pluginDirPath.isEmpty())
    {
        ui->le_plugindir->setText(pluginDirPath);
    }
}

/**
 * @brief MainWindow::comboBoxChanged
 * 当额外参数的下拉框选择发生变化时统一调用, 获取调用者并根据选择设置参数
 */
void MainWindow::comboBoxChanged()
{
    QComboBox * comboBox =  qobject_cast<QComboBox *>(sender());
    if(comboBox->currentIndex() == 0)
    {
        if(comboBox->findText("--list <option>") != -1)
        {
            QString tempStr =  ui->le_otherparameter->text();
            tempStr = tempStr.replace(" " + listStr, "");
            ui->le_otherparameter->setText(tempStr);
            listStr = "";
        }
        else
        {
            QString tempStr =  ui->le_otherparameter->text();
            tempStr = tempStr.replace(" " + verboseStr, "");
            ui->le_otherparameter->setText(tempStr);
            verboseStr = "";
        }
    }
    else
    {
        if(comboBox->findText("--list <option>") != -1)
        {
            QString tempStr =  ui->le_otherparameter->text();
            tempStr = tempStr.replace(listStr, "");
            ui->le_otherparameter->setText(tempStr);
            listStr = "";

            listStr = " --list " + comboBox->currentText();
            ui->le_otherparameter->setText(ui->le_otherparameter->text() + " " + listStr);
        }
        else
        {
            QString tempStr =  ui->le_otherparameter->text();
            tempStr = tempStr.replace(verboseStr, "");
            ui->le_otherparameter->setText(tempStr);
            verboseStr = "";

            verboseStr = " --verbose " + comboBox->currentText();
            ui->le_otherparameter->setText(ui->le_otherparameter->text() + " " + verboseStr);
        }
    }
}

/**
 * @brief MainWindow::openSetter
 * 打开setter窗口
 */
void MainWindow::openSetter()
{
    fileSetter = new FileSetter();
    connect(fileSetter, &FileSetter::refreshCfg, this, &MainWindow::setToolNameSelecter);
    fileSetter->show();
}

/**
 * @brief MainWindow::openCfgFile
 * 打开配置文件查看窗口
 */
void MainWindow::openCfgFile()
{
    reader = new CfgReader();
    connect(reader, &CfgReader::refreshCfg, this, &MainWindow::setToolNameSelecter);
    reader->show();
}

/**
 * @brief MainWindow::openCfgPath
 * 打开配置文件所在路径
 */
void MainWindow::openCfgPath()
{
    if(QDesktopServices::openUrl(QUrl("file:./", QUrl::TolerantMode)))
    {
        return;
    }
    else
    {
        QMessageBox::information(nullptr, "警告", "打开路径失败", QMessageBox::Ok);
    }
}

/**
 * @brief MainWindow::openHelp
 * 打开帮助窗口
 */
void MainWindow::openHelp()
{
    help->show();
}

/**
 * @brief MainWindow::openAbout
 * 打开关于窗口
 */
void MainWindow::openAbout()
{
    about->show();
}

/**
 * @brief MainWindow::changeMode
 * 当点击改变模式的复选框时调用, 改变打包模式并对应地修改界面
 */
void MainWindow::changeMode()
{
    if(ui->cb_simplemode->isChecked())
    {
        isSimpleMode = true;
        ui->cb_selectprojecttypeorpackmode->setEnabled(false);

        int count = ui->cb_selectprojecttypeorpackmode->count();
        for (int i = 1; i < count; --count)
        {
            ui->cb_selectprojecttypeorpackmode->removeItem(i);
        }

        ui->pb_dir->setEnabled(false);
        ui->pb_libdir->setEnabled(false);
        ui->pb_plugindir->setEnabled(false);
        ui->cb_cmd_1->setEnabled(false);
        ui->cb_cmd_2->setEnabled(false);
        ui->cb_cmd_3->setEnabled(false);
        ui->cb_cmd_4->setEnabled(false);
        ui->cb_cmd_5->setEnabled(false);
        ui->cb_cmd_6->setEnabled(false);
        ui->cb_cmd_7->setEnabled(false);
        ui->cb_cmd_8->setEnabled(false);
        ui->cb_cmd_9->setEnabled(false);
        ui->cb_cmd_10->setEnabled(false);
        ui->cb_cmd_11->setEnabled(false);
        ui->cb_cmd_12->setEnabled(false);
        ui->cb_cmd_13->setEnabled(false);
        ui->cb_cmd_14->setEnabled(false);
        ui->cb_cmd_15->setEnabled(false);
        ui->cb_cmd_16->setEnabled(false);
        ui->cb_cmd_17->setEnabled(false);
        ui->cb_cmd_18->setEnabled(false);
        ui->cb_cmd_19->setEnabled(false);
        ui->cb_list->setEnabled(false);
        ui->cb_verbose->setEnabled(false);
        ui->le_dir->setEnabled(false);
        ui->le_libdir->setEnabled(false);
        ui->le_plugindir->setEnabled(false);
        ui->le_otherparameter->setEnabled(false);
        ui->label_3->setEnabled(false);
        ui->label_4->setEnabled(false);
        ui->label_5->setEnabled(false);
        ui->label->setEnabled(false);
    }
    else
    {
        isSimpleMode = false;
        setPath(ui->cb_selecttoolname->currentText());

        ui->pb_dir->setEnabled(true);
        ui->pb_libdir->setEnabled(true);
        ui->pb_plugindir->setEnabled(true);
        ui->cb_cmd_1->setEnabled(true);
        ui->cb_cmd_2->setEnabled(true);
        ui->cb_cmd_3->setEnabled(true);
        ui->cb_cmd_4->setEnabled(true);
        ui->cb_cmd_5->setEnabled(true);
        ui->cb_cmd_6->setEnabled(true);
        ui->cb_cmd_7->setEnabled(true);
        ui->cb_cmd_8->setEnabled(true);
        ui->cb_cmd_9->setEnabled(true);
        ui->cb_cmd_10->setEnabled(true);
        ui->cb_cmd_11->setEnabled(true);
        ui->cb_cmd_12->setEnabled(true);
        ui->cb_cmd_13->setEnabled(true);
        ui->cb_cmd_14->setEnabled(true);
        ui->cb_cmd_15->setEnabled(true);
        ui->cb_cmd_16->setEnabled(true);
        ui->cb_cmd_17->setEnabled(true);
        ui->cb_cmd_18->setEnabled(true);
        ui->cb_cmd_19->setEnabled(true);
        ui->cb_list->setEnabled(true);
        ui->cb_verbose->setEnabled(true);
        ui->le_dir->setEnabled(true);
        ui->le_libdir->setEnabled(true);
        ui->le_plugindir->setEnabled(true);
        ui->le_otherparameter->setEnabled(true);
        ui->label_3->setEnabled(true);
        ui->label_4->setEnabled(true);
        ui->label_5->setEnabled(true);
        ui->label->setEnabled(true);
    }
}

/**
 * @brief MainWindow::release
 * 开始打包
 */
void MainWindow::release()
{
    if ((!isSimpleMode && ui->cb_selectprojecttypeorpackmode->currentIndex() == 0) || ui->cb_selecttoolname->currentIndex() == 0 || ui->le_releasepath->text().trimmed() == "")
    {
        QMessageBox::information(nullptr, "错误", QString("编译版本, 项目类型打包方式或路径不完整, 无法生成. "), QMessageBox::Ok);
        return;
    }

    QString releasePath = ui->le_releasepath->text();
    if (!isSimpleMode && releasePath.contains(" "))
    {
        QMessageBox::information(nullptr, "错误", QString("待打包文件路径错误, 路径中不能带有空格. "), QMessageBox::Ok);
        return;
    }
    if (releasePath.mid(releasePath.length() - 4) != ".exe")
    {
        QMessageBox::information(nullptr, "错误", QString("待打包文件路径错误, 非可执行文件. " + releasePath.mid(releasePath.length() - 4)), QMessageBox::Ok);
        return;
    }

    if(!isSimpleMode)
    {
        QString command = toolPath + "bin/windeployqt.exe  " + projectTypeParameter + " " + ui->le_releasepath->text() + " " + ui->le_otherparameter->text() + " " + ui->le_dir->text() + " " + ui->le_libdir->text() + " " + ui->le_plugindir->text();

        QProcess p;
        p.start("cmd", QStringList()<<"/c"<<command);
        p.waitForStarted();
        p.waitForFinished();
        output->setText(QString::fromLocal8Bit(p.readAllStandardOutput()));
        p.close();
        output->show();
    }
    else
    {
        QString info;
        int count = 0;

        QString fromDirBaseStr = toolPath;
        QString toDirBaseStr = ui->le_releasepath->text().mid(0, ui->le_releasepath->text().lastIndexOf("/") + 1);
        QString fromDirStr;
        QString toDirStr;

        for (QString needCopyStr : needCopyList)
        {
            QDir toDir(toDirBaseStr + needCopyStr.mid(needCopyStr.indexOf("/"), needCopyStr.lastIndexOf("/") - needCopyStr.indexOf("/")));
            if(!toDir.exists())
            {
                if(!toDir.mkdir(toDir.absolutePath()))
                {
                    //目的文件夹不存在且创建失败则跳过
                    continue;
                }
            }

            fromDirStr = fromDirBaseStr + needCopyStr;
            QFileInfo fileInfo(fromDirStr);
            if(!fileInfo.isFile())
            {
                //如果待拷贝文件夹不存在则跳过
                continue;
            }

            QString toDirChildrenStr;
            toDirChildrenStr = needCopyStr.mid(needCopyStr.indexOf("/") + 1);
            toDirStr = toDirBaseStr + toDirChildrenStr;

            QFile::copy(fromDirStr, toDirStr);
            ++count;
            info += "COPY <br />FROM <br /><span style='color:green;'>" + fromDirStr + "</span> <br />TO <br /><span style='color:green;'>" + toDirStr + "</span><br /><br />";
        }
        info += "TOTAL: <span style='color:green;'>" + QString::number(count) + "</span> FILES";
        output->setText(info);
        output->show();
    }
}

MainWindow::~MainWindow()
{
    delete fileSetter;
    delete ui;
}
