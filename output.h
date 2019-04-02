#ifndef OUTPUT_H
#define OUTPUT_H

#include <QWidget>

namespace Ui {
class Output;
}

class Output : public QWidget
{
    Q_OBJECT

public:
    explicit Output(QWidget *parent = nullptr);
    void setText(QString text);
    ~Output();

private:
    Ui::Output *ui;
};

#endif // OUTPUT_H
