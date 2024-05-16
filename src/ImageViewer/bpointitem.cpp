#include "bpointitem.h"
#include "bqgraphicsitem.h"
#include <QDebug>
#include <QtMath>

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
    return QRectF(-6, -6, 12, 12);
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
        painter->drawRect(QRectF(-6, -6, 12, 12));
        break;
    case Special:
        painter->drawRect(QRectF(-4, -4, 8, 8));
        break;
    default: break;
    }
}

void BPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->buttons() == Qt::LeftButton ) {
        qreal dx = event->scenePos().x() - event->lastScenePos().x();
        qreal dy = event->scenePos().y() - event->lastScenePos().y();

        BGraphicsItem* item = static_cast<BGraphicsItem *>(this->parentItem());
        BGraphicsItem::ItemType itemType = item->getType();

        switch (m_type) {
        case Center: {
                item->moveBy(dx, dy);
                this->scene()->update();
                QPointF pt = item->boundingRect().topLeft();
                int w = item->boundingRect().width();
                int h = item->boundingRect().height();
                QPointF LTgen = item->mapToScene(pt);
                int Xgen = static_cast<int>(LTgen.x());
                int Ygen = static_cast<int>(LTgen.y());
                emit item->ROIRegionChange(Xgen, Ygen, w, h);
        } break;
        case Edge: {
            if (itemType != BGraphicsItem::ItemType::Rounded_Rectangle) {
                m_point = this->mapToParent( event->pos() );
                this->setPos(m_point);
                this->scene()->update();
            }

            switch (itemType) {
            case BGraphicsItem::ItemType::Ellipse: {
                BEllipse *ellipse = dynamic_cast<BEllipse *>(item);
                ellipse->setEdge(m_point);
            } break;
            case BGraphicsItem::ItemType::Circle: {
                BCircle *circle = dynamic_cast<BCircle *>(item);
                circle->setEdge(m_point);
                circle->updateRadius();
            } break;
            case BGraphicsItem::ItemType::Rectangle: {
                BRectangle *rectangle = dynamic_cast<BRectangle *>(item);
                rectangle->setEdge(m_point);

            } break;
            default: break;
            }
            QPointF pt = item->boundingRect().topLeft();
            int w = item->boundingRect().width();
            int h = item->boundingRect().height();
            QPointF LTgen = item->mapToScene(pt);
            int Xgen = static_cast<int>(LTgen.x());
            int Ygen = static_cast<int>(LTgen.y());
            emit item->ROIRegionChange(Xgen, Ygen, w, h);
        } break;
        default: break;
        }
    }
}

//------------------------------------------------------------------------------

void BPointItemList::setRandColor()
{
    this->setColor(QColor(255,0 ,136/*qrand()%256, qrand()%256, qrand()%256*/));
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
