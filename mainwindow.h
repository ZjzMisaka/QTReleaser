#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filesetter.h"
#include "QProcess"
#include "QMessageBox"
#include "QComboBox"

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

private:
    Ui::MainWindow *ui;
    FileSetter *fileSetter;
    QList<QString> * datas;
    QString toolPath;
    QString qmlPath;
    QString releasePath;
    QString projectTypeParameter;
    QString otherParameter;
    QString listStr = "";
    QString verboseStr = "";

    void setToolNameSelecter();
};

#endif // MAINWINDOW_H
