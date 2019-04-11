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

signals:
    void setSchedule(QString schedule);
    void setResult(QList<QString> result);
    void resultReady(const QString &result);

};

#endif // FILECONTROLLER_H
