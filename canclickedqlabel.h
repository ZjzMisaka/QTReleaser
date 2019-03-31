#ifndef CANCLICKEDQLABEL_H
#define CANCLICKEDQLABEL_H

#include "QLabel"

class CanClickedQLabel : public QLabel
{
    Q_OBJECT
    public:
      CanClickedQLabel();
      CanClickedQLabel(const QString &text, QWidget * parent = nullptr);
    private:
    protected:
      virtual void mouseReleaseEvent(QMouseEvent * ev);
    signals:
      void clicked(CanClickedQLabel *selectedLabel);
};

#endif // CANCLICKEDQLABEL_H
