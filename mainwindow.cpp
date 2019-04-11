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

    connect(ui->action_opencfgfile, SIGNAL(triggered()), this, SLOT(openCfgFile()));
    connect(ui->action_opencfgpath, SIGNAL(triggered()), this, SLOT(openCfgPath()));
    connect(ui->action_help, SIGNAL(triggered()), this, SLOT(openHelp()));
    connect(ui->action_about, SIGNAL(triggered()), this, SLOT(openAbout()));
    connect(ui->ac_setter, SIGNAL(triggered()), this, SLOT(openSetter()));
    connect(ui->cb_selecttoolname, &QComboBox::currentTextChanged, this, &MainWindow::setPath);
    connect(ui->cb_selectprojecttype, &QComboBox::currentTextChanged, this, &MainWindow::setProjectTypeParameter);
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
}

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

void MainWindow::setPath(const QString & text)
{
    ui->cb_selectprojecttype->setEnabled(true);

    int count = ui->cb_selectprojecttype->count();
    for (int i = 1; i < count; --count)
    {
        ui->cb_selectprojecttype->removeItem(i);
    }
    if (text == "选择windeployqt版本")
    {
        ui->cb_selectprojecttype->setEnabled(false);
        return;
    }
    toolPath = fileSetter->getToolPathByName(text);
    qmlPath = fileSetter->getQmlPathByName(text);
    if (toolPath != nullptr && toolPath != "")
    {
        ui->cb_selectprojecttype->addItem("Qt Widgets Application");
    }
    if (qmlPath != nullptr && qmlPath != "")
    {
        ui->cb_selectprojecttype->addItem("Qt Quick Application");
    }
}

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

void MainWindow::selectFile()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("选择release文件的路径"));
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

void MainWindow::setOtherParameterText()
{
    QCheckBox * checkBox =  (QCheckBox *)sender();

    QString tempStr =  ui->le_otherparameter->text();
    tempStr = tempStr.replace(" --" + checkBox->text().replace(' ', "-"), "");
    ui->le_otherparameter->setText(tempStr);

    if (checkBox->isChecked())
    {
        ui->le_otherparameter->setText(ui->le_otherparameter->text() + " --" + checkBox->text().replace(' ', '-'));
    }
}

void MainWindow::selectDirPath()
{
    QString qmlPath = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), "./", QFileDialog::ShowDirsOnly);
    if (!qmlPath.isEmpty())
    {
        ui->le_dir->setText(qmlPath);
    }
}

void MainWindow::selectLibDirPath()
{
    QString qmlPath = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), "./", QFileDialog::ShowDirsOnly);
    if (!qmlPath.isEmpty())
    {
        ui->le_libdir->setText(qmlPath);
    }
}

void MainWindow::selectPluginDirPath()
{
    QString qmlPath = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), "./", QFileDialog::ShowDirsOnly);
    if (!qmlPath.isEmpty())
    {
        ui->le_plugindir->setText(qmlPath);
    }
}

void MainWindow::comboBoxChanged()
{
    QComboBox * comboBox =  (QComboBox *)sender();
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

void MainWindow::openSetter()
{
    fileSetter = new FileSetter();
    connect(fileSetter, &FileSetter::refreshCfg, this, &MainWindow::setToolNameSelecter);
    fileSetter->show();
}

void MainWindow::openCfgFile()
{
    reader = new CfgReader();
    connect(reader, &CfgReader::refreshCfg, this, &MainWindow::setToolNameSelecter);
    reader->show();
}

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

void MainWindow::openHelp()
{
    help->show();
}

void MainWindow::openAbout()
{
    about->show();
}

void MainWindow::release()
{
    if (ui->cb_selecttoolname->currentIndex() == 0 || ui->cb_selectprojecttype->currentIndex() == 0 || ui->le_releasepath->text().trimmed() == "")
    {
        QMessageBox::information(nullptr, "错误", QString("生成工具版本, 项目类型或路径不完整, 无法生成. "), QMessageBox::Ok);
        return;
    }

    QString command = toolPath + " " + projectTypeParameter + " " + ui->le_releasepath->text() + " " + ui->le_otherparameter->text() + " " + ui->le_dir->text() + " " + ui->le_libdir->text() + " " + ui->le_plugindir->text();

    QProcess p;
    p.start("cmd", QStringList()<<"/c"<<command);
    p.waitForStarted();
    p.waitForFinished();
    output->setText(QString::fromLocal8Bit(p.readAllStandardOutput()));
    p.close();
    output->show();
}

MainWindow::~MainWindow()
{
    delete fileSetter;
    delete ui;
}
