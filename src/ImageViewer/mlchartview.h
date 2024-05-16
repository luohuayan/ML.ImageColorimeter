#pragma once

#include <QtCharts/QChartView>

#include <QtCharts/QChartGlobal>
QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
class QValueAxis;
class QSplineSeries;
class QAreaSeries;
QT_CHARTS_END_NAMESPACE

class MLChartView : public QtCharts::QChartView
{
	Q_OBJECT

public:
	MLChartView(QWidget *parent = nullptr);
	~MLChartView();

    void initSightBead();
    void showSightBead(bool show = true);

    void setSelectPosChanged(QPoint selectPos);

  protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;


 void enterEvent(QEvent *event) override;


 void leaveEvent(QEvent *event) override;

 signals:
    void getClosestSeriesPos(QPoint, QPoint &);
    void getSeriesPos(QPoint, QPointF &);
    void mouseMovePos(QPoint);

    private:
    QGraphicsLineItem *m_Xline = nullptr;
    QGraphicsLineItem *m_Yline = nullptr;
    QGraphicsTextItem *m_text = nullptr;
    QGraphicsEllipseItem *m_closestPos = nullptr;
    QGraphicsLineItem *m_closetXLine = nullptr;
    QGraphicsLineItem *m_closetYLine = nullptr;
    bool m_showSightBead;

    QPoint m_mousePressedPoint;
    bool m_mouseRelease;
};
