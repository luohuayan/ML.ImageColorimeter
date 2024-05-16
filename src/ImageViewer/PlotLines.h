#ifndef PLOTLINES_H 
#define PLOTLINES_H 
#include<qwt_plot.h> 
#include <qwt_plot_layout.h> 
#include <qwt_plot_canvas.h> 
#include <qwt_plot_renderer.h> 
#include <qwt_plot_grid.h> 
#include <qwt_plot_histogram.h> 
#include <qwt_plot_curve.h> 
#include <qwt_plot_zoomer.h> 
#include <qwt_plot_panner.h> 
#include <qwt_plot_magnifier.h> 
#include <qwt_point_data.h>
#include <qwt_legend.h> 
#include <qwt_legend_label.h> 
#include <qwt_column_symbol.h> 
#include <qwt_series_data.h> 
#include <qpen.h> 
#include <qwt_symbol.h> 
#include <qwt_picker_machine.h> 
#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>
class Q_DECL_EXPORT PlotLines : public QwtPlot
{
    Q_OBJECT

public:
    explicit PlotLines(QString title,QString xLable,QString yLable,QWidget* parent = 0);
    void drawChart(double* x, double* y, int size, double maxMTFFreq,QString seriesName,QColor color);
    void drawChart(QList<QPointF> m_LineDatas, QString seriesName, QColor color);
    void clearChart();
private Q_SLOTS:
    void showItem(const QVariant& itemInfo, bool on);
};
class CurvePicker : public QwtPlotPicker
{
  public:
    CurvePicker(int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QWidget* canvas)
        : QwtPlotPicker(xAxis, yAxis, rubberBand, trackerMode, canvas)
    {
    }

  protected:
    virtual QwtText trackerTextF(const QPointF& pos) const
    {
        QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(plot()->itemList().back());
        if (curve)
        {
            return QString("x: %1\ny: %2").arg(pos.x()).arg(pos.y());
        }

        return QwtText();
    }
};
#endif // PLOTLINES_H 
