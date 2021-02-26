#include "btablewidget.h"

BTableWidget::BTableWidget(QWidget *parent) : QTableWidget(parent)
{

}

void BTableWidget::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Enter ) {
        event->ignore();
    }

    if ( event->modifiers() == Qt::ControlModifier &&  event->key() == Qt::Key_X ) {
        event->ignore();
    }

    QTableWidget::keyPressEvent(event);
}
