#ifndef BQGRAPHICSITEM_H
#define BQGRAPHICSITEM_H

#include <QObject>
#include "bpointitem.h"

#define PI 3.1415926

class BGraphicsItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT

public:
    enum ItemType {
        Circle = 0,         
        Ellipse,            
        Concentric_Circle,  
        Pie,                
        Chord,              
        Rectangle,          
        Square,             
        Polygon,            
        Round_End_Rectangle,
        Rounded_Rectangle   
    };

    QPointF getCenter() { return m_center; }
    void setCenter(QPointF p) { m_center = p; }

    QPointF getEdge() { return m_edge; }
    void setEdge(QPointF p) { m_edge = p; }

    QString getName() { return m_name; }
    void setName(QString name) { m_name = name; }

    ItemType getType() { return m_type; }

    void addDelMenu(QGraphicsSceneContextMenuEvent* event);
    QPointF getTopleft();
    void updateRegion();
    int Index = -1;
signals:
    void ROIRegionChange(int x, int y, int width, int height);
    void chooseItem(QString);
    void deleteOne();
    void deletePart();
    void deleteAll();
public slots:
    void delself();
    void delpart();
    void delall();
protected:
    BGraphicsItem(QPointF center, QPointF edge, QColor color,ItemType type);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    virtual void focusInEvent(QFocusEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);

protected:
    QPointF m_center;
    QPointF m_edge;
    ItemType m_type;
    QString m_name;
    BPointItemList m_pointList;
    QGraphicsSimpleTextItem* m_txtitem;
    QPen m_pen_isSelected;
    QPen m_pen_noSelected;
};

//------------------------------------------------------------------------------

class BEllipse : public BGraphicsItem
{
    Q_OBJECT

public:
    BEllipse(qreal x, qreal y, qreal width, qreal height, QColor color, ItemType type);

    enum { Type = 20};
    int type() const
    {
        return Type;
    }

protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

//------------------------------------------------------------------------------

class BCircle : public BEllipse
{
public:
    BCircle(qreal x, qreal y, qreal radius, QColor color, ItemType type);
    enum { Type = 21};
    int type() const
    {
        return Type;
    }
    void updateRadius();

protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

public:
    qreal m_radius;
};


//------------------------------------------------------------------------------
class BRectangle : public BGraphicsItem
{
public:
    BRectangle(qreal x, qreal y, qreal width, qreal height, QColor color, ItemType type);
    qreal GetRectLength(QString type);
    void SetRect(qreal x, qreal y, qreal width, qreal height);
    enum { Type = 25};
    int type() const
    {
        return Type;
    }
protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

//------------------------------------------------------------------------------

enum STATE_FLAG{
    DEFAULT_FLAG=0,
    MOV_LEFT_LINE,
    MOV_TOP_LINE,
    MOV_RIGHT_LINE,
    MOV_BOTTOM_LINE,
    MOV_RIGHTBOTTOM_RECT,
    MOV_RECT,
    ROTATE
};


//------------------------------------------------------------------------------

class BTargetPoint : public QGraphicsItem
{
public:
    BTargetPoint();
    BTargetPoint(qreal x, qreal y, qreal radius);
    enum { Type = 31};
    int type() const
    {
        return Type;
    }

protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
//    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
private:
    QRectF m_oldRect;
    qreal m_radius;
    QPointF HorizonLine_Start,HorizonLine_End;
    QPointF VerticalLine_Start,VerticalLine_End;
    QPointF m_startPos;
};

//---------------------------------------------------------------------------------

class BCrossPoint : public QGraphicsItem
{
public:
    BCrossPoint();
    BCrossPoint(qreal x, qreal y, qreal angle);
    enum { Type = 32};
    int type() const
    {
        return Type;
    }
        QPointF getRotatePoint(QPointF ptCenter, QPointF ptIn, qreal angle);

protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

private:
QPointF center;
QPointF Point_LU;
QPointF Point_RU;
QPointF Point_RD;
QPointF Point_LD;
QPolygonF m_polyRect;
};


//------------------------------------------------------------------------------------------

class BArrow : public QGraphicsItem
{
  public:
    BArrow();
    BArrow(QPointF startPoint,QPointF endPoint);
    void setLineItem(QPointF startP, QPointF endP);
    void setColor(QColor color);
        enum { Type = 33};
    int type() const
    {
        return Type;
    }
protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
private:
    void CreatePointNodes(void);
private:
    QPointF m_EndP;
    QPointF m_points[3];
    QColor  m_Color;
};
#endif // BQGRAPHICSITEM_H

