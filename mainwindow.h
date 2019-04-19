#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMimeData"
#include "filesetter.h"
#include "cfgreader.h"
#include "QProcess"
#include "QMessageBox"
#include "QComboBox"
#include "QDesktopServices"
#include "output.h"
#include "help.h"
#include "about.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSetter();
    void setPath(const QString & text);
    void setProjectTypeParameter(const QString & text);
    void selectFile();
    void release();
    void setOtherParameterText();
    void selectDirPath();
    void selectLibDirPath();
    void selectPluginDirPath();
    void comboBoxChanged();
    void openCfgPath();
    void openCfgFile();
    void openHelp();
    void openAbout();
    void changeMode();

private:
    Ui::MainWindow *ui;
    FileSetter *fileSetter;
    CfgReader *reader;
    Output *output;
    Help *help;
    About *about;

    QList<QString> * datas;
    QString toolPath;
    QString qmlPath;
    QString releasePath;
    QString projectTypeParameter;
    QString otherParameter;
    QString listStr;
    QString verboseStr;
    bool isSimpleMode;
    QList<QString> needCopyList;

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void setToolNameSelecter();
};

#endif // MAINWINDOW_H
