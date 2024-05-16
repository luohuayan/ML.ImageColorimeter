#include "resizer.hpp"

Resizer::Resizer(QGraphicsItem* crop, Position pos, QRgb rgba) : position(pos),
anchored(false), pressed(false)
{
    ColorBase.setRgba(rgba);
    ColorAnchored.setRgba(qRgba(0, 0, 255, 255));
    ColorMousePressed.setRgba(qRgba(255,0,0,255));
    setParentItem(crop);
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);
    setCursor(Qt::SizeAllCursor);
    updatePosition();
}

void Resizer::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    QRectF rect = boundingRect();
    QBrush brush(ColorBase);
    QPen pen(QColor(0, 0, 0, 0));

    if (pressed)
        brush.setColor(ColorMousePressed);
    else
    {
        if (anchored)
            brush.setColor(ColorAnchored);
        else
            brush.setColor(ColorBase);
    }

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(pen);
    painter->fillRect(rect, brush);
    painter->drawRect(rect);
}

QRectF Resizer::boundingRect() const
{
    return QRectF(0, 0, width, height);
}

void Resizer::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    pressed = true;
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Resizer::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    pressed = false;
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void Resizer::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    qreal dx = event->pos().x() - event->lastPos().x();
    qreal dy = event->pos().y() - event->lastPos().y();

    if (position == LEFT || position == RIGHT)
        moveBy(dx, 0);
    else
        moveBy(0, dy);

    update();
    emit sizeChanged(dx, dy, position);
    QGraphicsItem::mouseMoveEvent(event);
}

void Resizer::updatePosition()
{
    QGraphicsItem* crop = parentItem();
    qreal x, y;
    switch (position)
    {
        parentItem();
    case RIGHT:
        width = 3;
        height = crop->boundingRect().height() +2 * width;
        x = crop->boundingRect().width() / 2;
        y = -width -crop->boundingRect().height() / 2;
        break;

    case LEFT:
        width = 3;
        height = crop->boundingRect().height() + 2 * width;
        x = -width - crop->boundingRect().width() / 2;
        y = -width -crop->boundingRect().height() / 2;
        break;

    case UPPER:
        width = crop->boundingRect().width();
        height = 3;
        x = -crop->boundingRect().width() / 2;
        y = -height - crop->boundingRect().height() / 2;
        break;

    case BOTTOM:
        width = crop->boundingRect().width();
        height = 3;
        x = -crop->boundingRect().width() / 2;
        y = crop->boundingRect().height() / 2;
    }
    setPos(x, y);
}

