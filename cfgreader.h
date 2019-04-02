#ifndef CFGREADER_H
#define CFGREADER_H

#include <QWidget>
#include "QFile"
#include "QTextStream"

namespace Ui {
class CfgReader;
}

class CfgReader : public QWidget
{
    Q_OBJECT

public:
    explicit CfgReader(QWidget *parent = nullptr);
    ~CfgReader();

signals:
    void refreshCfg();

private slots:
    void saveCfg();

private:
    Ui::CfgReader *ui;
    QString cfgContent;
};

#endif // CFGREADER_H
