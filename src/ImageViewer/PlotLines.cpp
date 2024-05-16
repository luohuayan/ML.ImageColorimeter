#include "plotlines.h"

PlotLines::PlotLines(QString title, QString xLable, QString yLable, QWidget* parent) :
    QwtPlot(parent)
{
    QFont font;
    QwtText qtitle(title), qxLable(xLable), qyLable(yLable);
    font.setPointSize(10);
    font.setBold(true);
    qtitle.setFont(font);
    qxLable.setFont(font);
    qyLable.setFont(font);
    setTitle(qtitle);
    

    QwtPlotCanvas* canvas = new QwtPlotCanvas();
    canvas->setPalette(Qt::white);
    canvas->setBorderRadius(10);
    setCanvas(canvas);
    plotLayout()->setAlignCanvasToScales(true);


    QwtPlotGrid* grid = new QwtPlotGrid;
    grid->enableX(true);
    grid->enableY(true);
    grid->setMajorPen(Qt::black, 0, Qt::DotLine);
    grid->attach(this);
    setAxisTitle(QwtPlot::yLeft, qyLable);
    setAxisTitle(QwtPlot::xBottom, qxLable);

 
    QwtLegend* legend = new QwtLegend;
    legend->setDefaultItemMode(QwtLegendData::Checkable); 
    insertLegend(legend, QwtPlot::RightLegend);
    connect(legend, SIGNAL(checked(const QVariant&, bool, int)),SLOT(showItem(const QVariant&, bool)));

    CurvePicker* picker  = new CurvePicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPicker::CrossRubberBand, QwtPicker::AlwaysOn, this->canvas());
    picker->setTrackerMode(QwtPicker::AlwaysOn);
    picker->setStateMachine(new QwtPickerDragPointMachine());
    picker->setRubberBandPen(QPen(Qt::red));
    picker->setTrackerPen(QPen(Qt::blue));

    QwtPlotMagnifier* plot_magnifier = new QwtPlotMagnifier(this->canvas());
    QwtPlotPanner* plot_panner = new QwtPlotPanner(this->canvas());
    setAutoReplot(true);
}

void PlotLines::showItem(const QVariant& itemInfo, bool on)
{
    QwtPlotItem* plotItem = infoToItem(itemInfo);
    if (plotItem)
        plotItem->setVisible(on);
}
void PlotLines::drawChart(double* x,double* y,int size,double maxMTFFreq,QString seriesName,QColor color)
{
    QwtPlotCurve* curve = new QwtPlotCurve(seriesName);
    curve->setPen(color, 2);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true); 
    double max = 0;
    QVector<double> xCopy,yCopy;
    for (int i = 0; i < size; i++)
    {
        if (title().text() == "mtf(lp/degree)" && maxMTFFreq > 0)
        {
            if (x != NULL && x[i] > maxMTFFreq)
            {
                break;
            }
        }
        if (y != NULL && y[i] <= 100)
        {
            xCopy.append(x[i]);
            yCopy.append(y[i]);
            if (x[i] > max)
                max = x[i];
        }
    }
    setAxisScale(QwtPlot::xBottom, 0.0, max);
    QwtPointArrayData<double>* data = new QwtPointArrayData<double>(xCopy, yCopy);
    curve->setData(data);
    curve->attach(this);
    curve->setLegendAttribute(curve->LegendShowLine);

    QwtPlotZoomer* zoomer = new QwtPlotZoomer(this->canvas());
    zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ControlModifier);
    this->replot();

}

void PlotLines::drawChart(QList<QPointF> m_LineDatas, QString seriesName, QColor color)
{
    QwtPlotCurve* curve = new QwtPlotCurve(seriesName);
    curve->setPen(color, 2);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

    QVector<double> xCopy(m_LineDatas.size()), yCopy(m_LineDatas.size());
    for (int i = 0; i < m_LineDatas.size(); ++i) {
        xCopy[i] = m_LineDatas[i].x();
        yCopy[i] = m_LineDatas[i].y();
    }
    double xMin = xCopy.isEmpty() ? 0 : *std::min_element(xCopy.constBegin(), xCopy.constEnd());
    double xMax = xCopy.isEmpty() ? 0 : *std::max_element(xCopy.constBegin(), xCopy.constEnd());
    double yMin = yCopy.isEmpty() ? 0 : *std::min_element(yCopy.constBegin(), yCopy.constEnd());
    double yMax = yCopy.isEmpty() ? 0 : *std::max_element(yCopy.constBegin(), yCopy.constEnd());

    setAxisScale(QwtPlot::xBottom, xMin - 0.1 * (xMax - xMin), xMax + 0.1 * (xMax - xMin));
    setAxisScale(QwtPlot::yLeft, yMin - 0.1 * (yMax - yMin), yMax + 0.1 * (yMax - yMin));

    QwtPointArrayData<double>* data = new QwtPointArrayData<double>(xCopy.data(), yCopy.data(), xCopy.size());
    curve->setData(data);
    curve->attach(this);
    curve->setLegendAttribute(curve->LegendShowLine);
    QwtPlotZoomer* zoomer = new QwtPlotZoomer(this->canvas());
    zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton,Qt::ControlModifier);
    this->replot();
}

void PlotLines::clearChart()
{
    QwtPlotItemList items = itemList(QwtPlotItem::Rtti_PlotCurve);
    for (int i = 0; i < items.size(); i++)
    {
        items[i]->detach();
    }
}
