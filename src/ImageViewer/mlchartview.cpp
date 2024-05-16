#include "mlchartview.h"

#include <iostream>
#include <QGraphicsScene>

MLChartView::MLChartView(QWidget *parent) : QChartView(parent), m_mouseRelease(true)
{
    m_Xline = new QGraphicsLineItem;
    m_Yline = new QGraphicsLineItem;
    m_text = new QGraphicsTextItem;
    m_closetXLine = new QGraphicsLineItem;
    m_closetYLine = new QGraphicsLineItem;
    m_closestPos = new QGraphicsEllipseItem;
    m_closestPos->setRect(-2, -2, 4, 4);
}

MLChartView::~MLChartView()
{
}

void MLChartView::initSightBead()
{
    QGraphicsScene *scene = this->scene();
    
    scene->addItem(m_Xline);
    scene->addItem(m_Yline);
    scene->addItem(m_text);

    scene->addItem(m_closestPos);
    scene->addItem(m_closetXLine);
    scene->addItem(m_closetYLine);
    m_closestPos->setVisible(false);

    QPen crosshairsPen;
    crosshairsPen.setColor(Qt::gray);
    m_Xline->setPen(crosshairsPen);
    m_Yline->setPen(crosshairsPen);

    QPen pen;
    pen.setColor(Qt::black);
    m_closestPos->setPen(pen);
    m_closetXLine->setPen(pen);
    m_closetYLine->setPen(pen);
}

void MLChartView::showSightBead(bool show)
{
    m_showSightBead = show;
}

void MLChartView::setSelectPosChanged(QPoint selectPos)
{
    m_closestPos->setPos(selectPos);
    m_closestPos->setVisible(true);

    m_closetXLine->setLine(selectPos.x(), 0, selectPos.x(), this->height());
    m_closetYLine->setLine(0, selectPos.y(), this->width(), selectPos.y());
}

void MLChartView::mousePressEvent(QMouseEvent *event)
{
    QPoint pressPos = event->pos();
    setSelectPosChanged(pressPos);

    m_mouseRelease = false;
    m_mousePressedPoint = event->pos();
}

void MLChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mouseRelease)
    {
        QChartView::mouseMoveEvent(event);
        m_Xline->setLine(event->x(), 0, event->x(), this->height());
        m_Yline->setLine(0, event->y(), this->width(), event->y());

        QPointF seriesPos;
        emit getSeriesPos(event->pos(), seriesPos);
        QString showText = QString("[%1, %2]").arg(seriesPos.x()).arg(seriesPos.y());

        m_text->setPlainText(showText);

        QPoint textPos = event->pos();
        if (event->x() + m_text->boundingRect().width() > this->width())
        {
            textPos.setX(textPos.x() - m_text->boundingRect().width());
        }
        m_text->setPos(textPos);
    }
    else
    {
        
        int delatX = event->pos().x() - m_mousePressedPoint.x();
        int delatY = event->pos().y() - m_mousePressedPoint.y();
        m_mousePressedPoint = event->pos();
        mouseMovePos(QPoint(delatX, delatY));
    }

}

void MLChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_mousePressedPoint == event->pos())
    {
        QPoint closestSeriesPos;
        emit getClosestSeriesPos(event->pos(), closestSeriesPos);

        setSelectPosChanged(closestSeriesPos);
    }
    else 
    {
        m_mouseRelease = true;
    }

    m_mouseRelease = true;
}

void MLChartView::wheelEvent(QWheelEvent *event)
{
    //throw std::logic_error("The method or operation is not implemented.");
}

void MLChartView::enterEvent(QEvent *event)
{
    if (m_showSightBead)
    {
        m_Xline->setVisible(true);
        m_Yline->setVisible(true);
        m_text->setVisible(true);
    }
}

void MLChartView::leaveEvent(QEvent *event)
{
    m_Xline->setVisible(false);
    m_Yline->setVisible(false);
    m_text->setVisible(false);

    m_mouseRelease = true;
}
