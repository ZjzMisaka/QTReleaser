#include "canclickedqlabel.h"

CanClickedQLabel::CanClickedQLabel()
{
}

CanClickedQLabel::CanClickedQLabel(const QString &text, QWidget * parent) : QLabel(text, parent)
{

}

/**
 * @brief CanClickedQLabel::mouseReleaseEvent
 * @param ev
 * 重写事件, 并发送一个信号, 参数为自己
 */
void CanClickedQLabel::mouseReleaseEvent(QMouseEvent * ev)
{
  Q_UNUSED(ev)
  emit clicked(this);
}
