#include "LineCutWindow.h"
#include <QDateTime>
#include <QDebug>

LineCutWindow::LineCutWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	initChartView();
	setWindowIcon(QIcon(":/image/images/logo/mlico1.ico"));
	setWindowFlag(Qt::WindowStaysOnTopHint, true);
}

LineCutWindow::~LineCutWindow()
{}

void LineCutWindow::initChartView()
{
	m_chart = new PlotLines("", "Distance(pixels)", "Gray Value");
	QHBoxLayout* qHBoxLayout = new QHBoxLayout(this);
	ui.m_chartView->setLayout(qHBoxLayout);
	qHBoxLayout->setContentsMargins(0, 0, 0, 0);
	qHBoxLayout->setSpacing(0);
	qHBoxLayout->addWidget(m_chart);
}

void LineCutWindow::onUpdateCutLine(QList<float> m_CutLineAxisXDatas, \
	QList<QPointF> m_CutLineAxisYGrayDatas, QList<QPointF> m_CutLineAxisYRedDatas,\
	QList<QPointF> m_CutLineAxisYGreenDatas, QList<QPointF> m_CutLineAxisYBlueDatas)
{
	QDateTime startTime = QDateTime::currentDateTime();
	qDebug() << __FUNCTION__ << "..............size:" << m_CutLineAxisYBlueDatas.size();
	qDebug() << __FUNCTION__ << "startTime=" << startTime.toString("hh:mm:ss.zzz");
	this->show();

	m_chart->clearChart();
	m_chart->drawChart(m_CutLineAxisYGrayDatas, "grayLine", QColor(0, 0, 255));

	QDateTime endTime = QDateTime::currentDateTime();
	qDebug() << __FUNCTION__ << "endTime="<< endTime.toString("hh:mm:ss.zzz");
	qDebug() << __FUNCTION__  << startTime.msecsTo(endTime);
}
