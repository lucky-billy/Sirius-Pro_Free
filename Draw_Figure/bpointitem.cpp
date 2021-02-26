#include "bpointitem.h"
#include "bqgraphicsitem.h"

BPointItem::BPointItem(QAbstractGraphicsShapeItem* parent, QPointF p, PointType type)
    : QAbstractGraphicsShapeItem(parent)
    , m_point(p)
    , m_type(type)
{
    this->setPos(m_point);
    this->setFlags(QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsFocusable);

    switch (type) {
    case Center:
        this->setCursor(Qt::OpenHandCursor);
        break;
    case Edge:
        this->setCursor(Qt::PointingHandCursor);
        break;
    case Special:
        this->setCursor(Qt::PointingHandCursor);
        break;
    default: break;
    }
}

QRectF BPointItem::boundingRect() const
{
    return QRectF(-4, -4, 8, 8);
}

void BPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(this->pen());
    painter->setBrush(this->brush());
    this->setPos(m_point);

    switch (m_type) {
    case Center:
        painter->drawEllipse(-4, -4, 8, 8);
        break;
    case Edge:
        painter->drawRect(QRectF(-4, -4, 8, 8));
        break;
    case Special:
        painter->drawRect(QRectF(-4, -4, 8, 8));
        break;
    default: break;
    }
}

void BPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    BSyncytia *item = static_cast<BSyncytia *>(this->parentItem());
    BGraphicsItem::ItemType itemType = item->getType();

    if ( event->buttons() == Qt::LeftButton && !item->getIfLocked() )
    {
        switch (m_type) {
        case Center: {
            qreal dx = event->scenePos().x() - event->lastScenePos().x();
            qreal dy = event->scenePos().y() - event->lastScenePos().y();
            item->moveBy(dx, dy);
            this->scene()->update();
        } break;
        case Edge: {
            switch (itemType) {
            case BGraphicsItem::ItemType::Circle:
            case BGraphicsItem::ItemType::Ellipse:
            case BGraphicsItem::ItemType::Concentric_Circle:
            case BGraphicsItem::ItemType::Rectangle: {
                m_point = this->mapToParent( event->pos() );
                item->updateItemSize(m_point, false);
                this->setPos(m_point);
                this->scene()->update();
            } break;
            case BGraphicsItem::ItemType::Square: {
                m_point = this->mapToParent( event->pos() );
                qreal ret = m_point.x() > m_point.y() ? m_point.x() : m_point.y();
                m_point.setX(ret);
                m_point.setY(ret);
                item->updateItemSize(m_point, false);
                this->setPos(m_point);
                this->scene()->update();
            } break;
            case BGraphicsItem::ItemType::Pill: {
                m_point = this->mapToParent( event->pos() );
                qreal ret_y = m_point.y() > 5 ? ( m_point.y() > m_point.x() ? ( m_point.x() > 5 ? m_point.x() : 5 ) : m_point.y() ) : 5;
                qreal ret_x = m_point.x() > ret_y ? m_point.x() : ret_y;
                m_point.setX(ret_x);
                m_point.setY(ret_y);
                item->updateItemSize(m_point, false);
                this->setPos(m_point);
                this->scene()->update();
            } break;
            case BGraphicsItem::ItemType::Chamfer: {
                m_point = this->mapToParent( event->pos() );
                qreal radius = item->getRadius();
                qreal ret_x = m_point.x() > radius ? m_point.x() : radius;
                qreal ret_y = m_point.y() > radius ? m_point.y() : radius;
                m_point.setX(ret_x);
                m_point.setY(ret_y);
                item->updateItemSize(m_point, false);
                this->setPos(m_point);
                this->scene()->update();
            } break;
            case BGraphicsItem::ItemType::Polygon: {
                m_point = this->mapToParent( event->pos() );
                item->updateEdgeInPolygon(QPointF( event->lastScenePos().x(), event->lastScenePos().y() ),
                                          QPointF( event->scenePos().x(), event->scenePos().y() ));
                this->setPos(m_point);
                this->scene()->update();
            } break;
            default: break;
            }
        } break;
        case Special: {
            switch (itemType) {
            case BGraphicsItem::ItemType::Concentric_Circle: {
                m_point = this->mapToParent( event->pos() );
                item->updateItemSize(m_point, true);
                this->setPos(m_point);
                this->scene()->update();
            } break;
            case BGraphicsItem::ItemType::Chamfer: {
                m_point = this->mapToParent( event->pos() );
                qreal width = item->getWidth();
                qreal height = item->getHeight();
                qreal dx = width/2 - m_point.x();
                qreal dy = height/2 + m_point.y();

                if ( width >= height ) {    // 横
                    if ( dx >= dy ) {   // X轴上移动
                        qreal ret_x = m_point.x() < (width/2 - height/2) ? (width/2 - height/2) :
                                                                           (m_point.x() > width/2 ? width/2 : m_point.x());
                        m_point.setX(ret_x);
                        m_point.setY(-height/2);
                    } else {    // y轴上移动
                        qreal ret_y = m_point.y() < 0 ? (m_point.y() > -height/2 ? m_point.y() : -height/2) : 0;
                        m_point.setX(width/2);
                        m_point.setY(ret_y);
                    }
                } else {    // 竖
                    if ( dx >= dy ) {   // X轴上移动
                        qreal ret_x = m_point.x() > 0 ? (m_point.x() > width/2 ? width/2 : m_point.x()) : 0;
                        m_point.setX(ret_x);
                        m_point.setY(-height/2);
                    } else {    // y轴上移动
                        qreal ret_y = m_point.y() > (-height/2 + width/2) ? (-height/2 + width/2) :
                                                                            (m_point.y() > -height/2 ? m_point.y() : -height/2);
                        m_point.setX(width/2);
                        m_point.setY(ret_y);
                    }
                }
                item->updateItemSize(m_point, true);
                this->setPos(m_point);
                this->scene()->update();
            } break;
            default: break;
            }
        } break;
        default: break;
        }
    }
}

//------------------------------------------------------------------------------

void BPointItemList::setRandColor()
{
    this->setColor(QColor(qrand()%256, qrand()%256, qrand()%256));
}

void BPointItemList::setColor(const QColor color)
{
    for (auto &temp : *this)
    {
        temp->setBrush(QBrush(color));
    }
}

void BPointItemList::setVisible(bool visible)
{
    for (auto &temp : *this)
    {
        temp->setVisible(visible);
    }
}
