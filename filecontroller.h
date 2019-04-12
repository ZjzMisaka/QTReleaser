#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H

#include "QObject "
#include <QWidget>
#include "canclickedqlabel.h"
#include "QFileDialog"
#include "QTextStream"


class FileController : public QObject
{
    Q_OBJECT
public:
    FileController();

public slots:
    void findFileInPath(QString path, QString fileName);
    void stopThread();

signals:
    void setSchedule(QString schedule, bool isNameSame);
    void finishAutoSet();
    void resultReady(const QString &result);

private:
    bool needStopThread;
};

#endif // FILECONTROLLER_H
