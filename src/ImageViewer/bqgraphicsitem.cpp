#include "bqgraphicsitem.h"
#include <QVector>
#include <QDebug>
#include <QMenu>
#include <QSpinBox>
#include <QWidgetAction>
#include <QtMath>

void BGraphicsItem::delself() 
{
    emit deleteOne();
}
void BGraphicsItem::delpart()
{
    emit deletePart();
}

void BGraphicsItem::delall()
{
    emit deleteAll();
}

BGraphicsItem::BGraphicsItem(QPointF center, QPointF edge, QColor color, ItemType type)
    : m_center(center), m_edge(edge), m_type(type)
{
    m_pen_noSelected.setColor(color);
    m_pen_noSelected.setWidth(2);
    m_pen_isSelected.setColor(QColor(255, 0, 255));
    m_pen_isSelected.setWidth(4);

    this->setPen(m_pen_noSelected);
    this->setFlags(QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);
}
void BGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    updateRegion();
    QAbstractGraphicsShapeItem::mouseMoveEvent(event);
}

void BGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    this->setPen(m_pen_isSelected);
    m_pointList.at(0)->show();
    m_pointList.at(1)->show();
}

void BGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    this->setPen(m_pen_noSelected);
    m_pointList.at(0)->hide();
    m_pointList.at(1)->hide();

}

QPointF BGraphicsItem::getTopleft()
{
    qreal half_width = abs(m_center.x() - m_edge.x());
    qreal half_height = abs(m_center.y() - m_edge.y());
    qreal topLeftX = m_center.x() - half_width;
    qreal topLeftY = m_center.y() - half_height;
    return QPointF(topLeftX, topLeftY);
}

void BGraphicsItem::updateRegion()
{
    QPointF pt = boundingRect().topLeft();
    int w = boundingRect().width();
    int h = boundingRect().height();
    QPointF LTgen = mapToScene(pt);
    int Xgen = static_cast<int>(LTgen.x());
    int Ygen = static_cast<int>(LTgen.y());
    emit ROIRegionChange(Xgen, Ygen, w, h);
}
void BGraphicsItem::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    this->setPen(m_pen_isSelected);
    emit chooseItem(getName());
}

void BGraphicsItem::focusOutEvent(QFocusEvent* event)
{
    Q_UNUSED(event);
    this->setPen(m_pen_noSelected);
}

void BGraphicsItem::addDelMenu(QGraphicsSceneContextMenuEvent* event)
{
    QMenu menu;
    QAction* removeAction = new QAction("Delete this item.", &menu);
    QAction* removePartAction = new QAction("Delete this type of item.", &menu);
    QAction* removeAllAction = new QAction("Delete all item.", &menu);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(delself()));
    connect(removePartAction, SIGNAL(triggered()), this, SLOT(delpart()));
    connect(removeAllAction, SIGNAL(triggered()), this, SLOT(delall()));
    menu.addAction(removeAction);
    menu.addAction(removePartAction);
    menu.addAction(removeAllAction);
    menu.exec(event->screenPos());
}

//------------------------------------------------------------------------------

BEllipse::BEllipse(qreal x, qreal y, qreal width, qreal height,QColor color, ItemType type)
    : BGraphicsItem(QPointF(x,y), QPointF(x+width/2,y+height/2), color, type)
{
    BPointItem *point = new BPointItem(this, m_edge, BPointItem::Edge);
    point->setParentItem(this);
    m_pointList.append(point);
    m_pointList.append(new BPointItem(this, m_center, BPointItem::Center));
    m_pointList.setRandColor();
    m_pointList.at(0)->hide();
    m_pointList.at(1)->hide();
}

QRectF BEllipse::boundingRect() const
{
    qreal half_width = abs(m_center.x() - m_edge.x());
    qreal half_height = abs(m_center.y() - m_edge.y());

    qreal topLeftX = m_center.x() - half_width;
    qreal topLeftY = m_center.y() - half_height;

    return  QRectF(topLeftX, topLeftY, half_width * 2, half_height * 2);
}

void BEllipse::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    //QRectF ret(m_center.x() - abs(m_edge.x()), m_center.y() - abs(m_edge.y()), abs(m_edge.x()) * 2, abs(m_edge.y()) * 2);
    painter->drawEllipse(boundingRect());
}

void BEllipse::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (this->isSelected())
    {
        QMenu* menu = new QMenu();
        menu->setStyleSheet("QMenu { background-color:transparent; border: 5px solid rgb(235,110,36); border-radius: 10px; }");

        QSpinBox* width_spinBox = new QSpinBox(menu);
        width_spinBox->setStyleSheet("QSpinBox{ width:120px; height:30px; font-size:16px; font-weight:bold; }");
        width_spinBox->setRange(0, 1000);
        width_spinBox->setPrefix("w: ");
        width_spinBox->setSuffix(" mm");
        width_spinBox->setSingleStep(1);
        width_spinBox->setValue(2 * abs(m_center.x() - m_edge.x()));
        connect(width_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int v) {
            //if (m_edge.x() < 0) {
            //    m_edge.setX((-1) * v / 2);
            //}
            //else {
            //    m_edge.setX(v / 2);
            //}
            m_edge.setX(m_center.x() + v / 2);
            m_pointList.at(0)->setPoint(m_edge);
            this->hide();
            this->update();
            this->show();
            updateRegion();
            });

        QSpinBox* height__spinBox = new QSpinBox(menu);
        height__spinBox->setStyleSheet("QSpinBox{ width:120px; height:30px; font-size:16px; font-weight:bold; }");
        height__spinBox->setRange(0, 1000);
        height__spinBox->setPrefix("h: ");
        height__spinBox->setSuffix(" mm");
        height__spinBox->setSingleStep(1);
        height__spinBox->setValue(2 * abs(m_center.y() - m_edge.y()));
        connect(height__spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int v) {
            //if (m_edge.y() < 0) {
            //    m_edge.setY((-1) * v / 2);
            //}
            //else {
            //    m_edge.setY(v / 2);
            //}
            m_edge.setY(m_center.y() + v / 2);
            m_pointList.at(0)->setPoint(m_edge);
            this->hide();
            this->update();
            this->show();
            updateRegion();
            });

        QWidgetAction* width_widgetAction = new QWidgetAction(menu);
        width_widgetAction->setDefaultWidget(width_spinBox);
        menu->addAction(width_widgetAction);

        QWidgetAction* height_widgetAction = new QWidgetAction(menu);
        height_widgetAction->setDefaultWidget(height__spinBox);
        menu->addAction(height_widgetAction);

        menu->exec(QCursor::pos());
        delete menu;
    }
    else
    {
        addDelMenu(event);
    }
    QGraphicsItem::contextMenuEvent(event);
}

//------------------------------------------------------------------------------

BCircle::BCircle(qreal x, qreal y, qreal radius, QColor color, ItemType type)
    : BEllipse(x, y, radius*sqrt(2), radius*sqrt(2), color, type)
{
    updateRadius();
}

void BCircle::updateRadius()
{
    m_radius = round(sqrt(pow(m_center.x() - m_edge.x(), 2) + pow(m_center.y() - m_edge.y(), 2)));
}

QRectF BCircle::boundingRect() const
{
    return QRectF(m_center.x() - m_radius, m_center.y() - m_radius, m_radius * 2, m_radius * 2);
}

void BCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    QRectF ret(m_center.x() - m_radius, m_center.y() - m_radius, m_radius * 2, m_radius * 2);
    painter->drawEllipse(ret);
}

void BCircle::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (this->isSelected())
    {
        QMenu* menu = new QMenu();
        menu->setStyleSheet("QMenu { background-color:transparent; border: 5px solid rgb(235,110,36); border-radius: 10px; }");

        QSpinBox* radius_spinBox = new QSpinBox(menu);
        radius_spinBox->setStyleSheet("QSpinBox{ width:120px; height:30px; font-size:16px; font-weight:bold; }");
        radius_spinBox->setRange(0, 1000);
        radius_spinBox->setPrefix("r: ");
        radius_spinBox->setSuffix(" mm");
        radius_spinBox->setSingleStep(1);
        radius_spinBox->setValue(m_radius);
        connect(radius_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int v) {
            m_radius = v;

            m_edge.setX(m_center.x() + m_radius * sqrt(2) / 2);

            m_edge.setY(m_center.y() + m_radius * sqrt(2) / 2);

            m_pointList.at(0)->setPoint(m_edge);
            this->hide();
            this->update();
            this->show();
            updateRegion();
            });

        QWidgetAction* radius_widgetAction = new QWidgetAction(menu);
        radius_widgetAction->setDefaultWidget(radius_spinBox);
        menu->addAction(radius_widgetAction);

        menu->exec(QCursor::pos());
        delete menu;
    }
    else
    {
        addDelMenu(event);
    }
    QGraphicsItem::contextMenuEvent(event);
}


//------------------------------------------------------------------------------

BRectangle::BRectangle(qreal x, qreal y, qreal width, qreal height, QColor color, ItemType type)
    : BGraphicsItem(QPointF(x,y), QPointF(x + width/2,y + height/2), color, type)
{
    BPointItem *point = new BPointItem(this, m_edge, BPointItem::Edge);
    point->setParentItem(this);
    m_pointList.append(point);
    m_pointList.append(new BPointItem(this, m_center, BPointItem::Center));
    m_pointList.setRandColor();
    m_pointList.at(0)->hide();
    m_pointList.at(1)->hide();
}
//获取矩形的长边和短边尺寸
qreal BRectangle::GetRectLength(QString type)
{
    qreal Rect_Length1,Rect_Length2;
    Rect_Length1 = abs(m_edge.x()-m_center.x()) * 2;
    Rect_Length2 = abs(m_edge.y()-m_center.y()) * 2;

    if(type == "Length1") return Rect_Length1;
    else if(type == "Length2") return Rect_Length2;
}

void BRectangle::SetRect(qreal x, qreal y, qreal width, qreal height)
{

    m_center.setX(x);
    m_center.setY(y);
    m_edge.setX(width/2);
    m_edge.setY(height/2);
    m_pointList.at(0)->setPoint(m_edge);
    this->hide();
    this->update();
    this->show();
    qDebug()<<m_center<<m_edge<<endl;
}
QRectF BRectangle::boundingRect() const
{
    qreal half_width = abs(m_center.x() - m_edge.x());
    qreal half_height = abs(m_center.y() - m_edge.y());

    qreal topLeftX = m_center.x() - half_width;
    qreal topLeftY = m_center.y() - half_height;

    return  QRectF(topLeftX, topLeftY, half_width * 2, half_height * 2);
}

void BRectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(this->pen());
    painter->setBrush(this->brush());
    //QPen mPen = QPen(Qt::blue,4);
    //painter->setPen(mPen);

   // QRectF ret(m_center.x() - abs(m_edge.x()), m_center.y() - abs(m_edge.y()), abs(m_edge.x()) * 2, abs(m_edge.y()) * 2);
    painter->drawRect(boundingRect());
}

void BRectangle::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (this->isSelected())
    {
        QMenu* menu = new QMenu();
        menu->setStyleSheet("QMenu { background-color:transparent; border: 5px solid rgb(235,110,36); border-radius: 10px; }");

        QSpinBox* width_spinBox = new QSpinBox(menu);
        width_spinBox->setStyleSheet("QSpinBox{ width:120px; height:30px; font-size:16px; font-weight:bold; }");
        width_spinBox->setRange(0, 1000);
        width_spinBox->setPrefix("w: ");
        width_spinBox->setSuffix(" mm");
        width_spinBox->setSingleStep(1);
        width_spinBox->setValue(2 * abs(m_center.x() - m_edge.x()));
        connect(width_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int v) {
            //if (m_edge.x() < 0) {
            //    m_edge.setX((-1) * v / 2);
            //}
            //else {
            //    m_edge.setX(v / 2);
            //}
            m_edge.setX(m_center.x() + v / 2);
            m_pointList.at(0)->setPoint(m_edge);
            this->hide();
            this->update();
            this->show();
            updateRegion();
            });

        QSpinBox* height__spinBox = new QSpinBox(menu);
        height__spinBox->setStyleSheet("QSpinBox{ width:120px; height:30px; font-size:16px; font-weight:bold; }");
        height__spinBox->setRange(0, 1000);
        height__spinBox->setPrefix("h: ");
        height__spinBox->setSuffix(" mm");
        height__spinBox->setSingleStep(1);
        height__spinBox->setValue(2 * abs(m_center.y() - m_edge.y()));
        connect(height__spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int v) {
            //if (m_edge.y() < 0) {
            //    m_edge.setY((-1) * v / 2);
            //}
            //else {
            //    m_edge.setY(v / 2);
            //}
            m_edge.setY(m_center.y() + v / 2);
            m_pointList.at(0)->setPoint(m_edge);
            this->hide();
            this->update();
            this->show();
            updateRegion();
            });

        QWidgetAction* width_widgetAction = new QWidgetAction(menu);
        width_widgetAction->setDefaultWidget(width_spinBox);
        menu->addAction(width_widgetAction);

        QWidgetAction* height_widgetAction = new QWidgetAction(menu);
        height_widgetAction->setDefaultWidget(height__spinBox);
        menu->addAction(height_widgetAction);

        menu->exec(QCursor::pos());
        delete menu;
    }
    else
    {
        addDelMenu(event);
    }
    QGraphicsItem::contextMenuEvent(event);
}

BCrossPoint::BCrossPoint(qreal x, qreal y, qreal angle)
{
    center.setX(x);
    center.setY(y);
    QPointF p_oldLU = QPointF(x - 3*cos(1.0472),y-3*sin(1.0472));
    QPointF p_oldRU = QPointF(x + 3*cos(1.0472),y-3*sin(1.0472));
    QPointF p_oldRD = QPointF(x + 3*cos(1.0472),y+3*sin(1.0472));
    QPointF p_oldLD = QPointF(x - 3*cos(1.0472),y+3*sin(1.0472));
    QVector<QPointF> vpt;
    Point_LU = getRotatePoint(center,p_oldLU,angle);
    vpt.append(Point_LU);
    Point_RU = getRotatePoint(center,p_oldRU,angle);
    vpt.append(Point_RU);
    Point_RD = getRotatePoint(center,p_oldRD,angle);
    vpt.append(Point_RD);
    Point_LD = getRotatePoint(center,p_oldLD,angle);
    vpt.append(Point_LD);
    vpt.append(Point_LU);
    m_polyRect = QPolygonF(vpt);
}

QRectF BCrossPoint::boundingRect() const
{
    QRectF boundingRectF = m_polyRect.boundingRect();
    return QRectF(boundingRectF.x(), boundingRectF.y(), boundingRectF.width(),boundingRectF.height());
}

void BCrossPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPen mPen = QPen(Qt::green,1);
    painter->setPen(mPen);

    //绘制十字线
    painter->drawLine(Point_LU,Point_RD);
    painter->drawLine(Point_RU,Point_LD);
}

QPointF BCrossPoint::getRotatePoint(QPointF ptCenter, QPointF ptIn, qreal angle)
{
    double dx = ptCenter.x();
    double dy = ptCenter.y();
    double x = ptIn.x();
    double y = ptIn.y();
    double xx,yy;
    xx = (x-dx)*cos(angle*M_PI/180)-(y-dy)*sin(angle*M_PI/180)+dx;
    yy = (x-dx)*sin(angle*M_PI/180)+(y-dy)*cos(angle*M_PI/180)+dy;

    return QPointF(xx,yy);
}

//---------------------------------------------------------------------------------

BArrow::BArrow()
{

}

BArrow::BArrow(QPointF startPoint, QPointF endPoint)
{

    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);

    m_Color = Qt::green;
    setLineItem(startPoint,endPoint);

}

void BArrow::setLineItem(QPointF startP, QPointF endP)
{
    m_EndP = endP - startP;

    CreatePointNodes();
}

void BArrow::setColor(QColor color)
{
   m_Color = color;
}

void BArrow::CreatePointNodes()
{
    //箭头直线与水平方向的夹角再加pi
    float angle = atan2(m_EndP.y(), m_EndP.x()) + 3.1415926;
    //这两个值需要根据实际场景的坐标大小进行调整，
    float ExtRefArrowLenght = 4;//箭头末端大小的长度，
    float ExtRefArrowDegrees = 1.047;//箭头末端顶角的一半

    m_points[0] = m_EndP;
     //求得箭头点1坐标
    m_points[1].setX(m_EndP.x() + ExtRefArrowLenght * cos(angle - ExtRefArrowDegrees));
    m_points[1].setY(m_EndP.y() + ExtRefArrowLenght * sin(angle - ExtRefArrowDegrees));
    //求得箭头点2坐标
    m_points[2].setX(m_EndP.x() + ExtRefArrowLenght * cos(angle + ExtRefArrowDegrees));
    m_points[2].setY(m_EndP.y() + ExtRefArrowLenght * sin(angle + ExtRefArrowDegrees));


}

QRectF BArrow::boundingRect() const
{
    return QRectF(0, 0, m_EndP.x(), m_EndP.y());
}


void BArrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);                   //设置反走样，防锯齿
    QPen pen(m_Color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush brush(m_Color, Qt::SolidPattern);

    painter->setPen(pen);
    painter->setBrush(brush);

    QLineF line(0, 0, m_EndP.x(), m_EndP.y());
    painter->drawLine(line);

    painter->drawPolygon(m_points, 3);
}
