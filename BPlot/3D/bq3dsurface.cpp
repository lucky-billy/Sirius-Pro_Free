#include "bq3dsurface.h"

BQMouseEvent::BQMouseEvent(Type type, const QPointF &localPos, Qt::MouseButton button,
                           Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers)
    : QMouseEvent(type, localPos, button, buttons, modifiers)
{

}

void BQMouseEvent::changeButtonType(int type)
{
    if ( type == 1 ) {
        this->b = Qt::LeftButton;
    } else if ( type == 2 ) {
        this->b = Qt::RightButton;
    }
}

BQ3DSurface::BQ3DSurface(const QSurfaceFormat *format, QWindow *parent) : Q3DSurface(format, parent)
{

}

void BQ3DSurface::mousePressEvent(QMouseEvent *event)
{
    BQMouseEvent *e = static_cast<BQMouseEvent *>(event);
    if ( e->buttons() == Qt::LeftButton ) {
        e->changeButtonType(2);
    } else if ( e->buttons() == Qt::RightButton ) {
        e->changeButtonType(1);
        emit mousePressed();
    }

    Q3DSurface::mousePressEvent(e);
}
