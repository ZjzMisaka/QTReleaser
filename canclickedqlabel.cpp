#include "canclickedqlabel.h"

CanClickedQLabel::CanClickedQLabel()
{
}

CanClickedQLabel::CanClickedQLabel(const QString &text, QWidget * parent) : QLabel(text, parent)
{

}

void CanClickedQLabel::mouseReleaseEvent(QMouseEvent * ev)
{
  Q_UNUSED(ev)
  emit clicked(this);
}
