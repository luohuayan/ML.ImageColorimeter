#ifndef BPOINTITEM_H
#define BPOINTITEM_H

#include <QObject>
#include <QAbstractGraphicsShapeItem>
#include <QPointF>
#include <QPen>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QCursor>
#include <QKeyEvent>
#include <QList>

class BPointItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT

public:
    enum PointType {
        Center = 0,
        Edge,      
        Special    
    };

    BPointItem(QAbstractGraphicsShapeItem* parent, QPointF p, PointType type);

    QPointF getPoint() { return m_point; }
    void setPoint(QPointF p) { m_point = p; }

protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPointF m_point;
    PointType m_type;
};

//------------------------------------------------------------------------------

class BPointItemList: public QList<BPointItem *>
{
public:
    void setRandColor();
    void setColor(const QColor color);
    void setVisible(bool visible);
};

#endif // BPOINTITEM_H
