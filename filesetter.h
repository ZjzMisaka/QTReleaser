#ifndef FILESETTER_H
#define FILESETTER_H

#include <QWidget>
#include "canclickedqlabel.h"
#include "QFileDialog"
#include "QTextStream"

using namespace std;

namespace Ui {
class FileSetter;
}

class FileSetter : public QWidget
{
    Q_OBJECT

public:
    explicit FileSetter(QWidget *parent = nullptr);
    ~FileSetter();

private slots:
    CanClickedQLabel * addLine();
    void deleteLine();
    void selectLabel(CanClickedQLabel *selectedLabel);
    void reSet();
    void selectFile();

private:
    Ui::FileSetter *ui;
    CanClickedQLabel *label;
    CanClickedQLabel *selectedLabel;
    QList<QString> * datas;
    QList<CanClickedQLabel *> labelList;
    const QString cfgPath = "./config.txt";

    void checkCfgPath();
    void getDatasFromCfg();
    QString getPathByName(QString name);
    bool saveDataToCfg();
    bool writeToCfg();
};

#endif // FILESETTER_H
