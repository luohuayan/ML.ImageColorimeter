#pragma once

#include <QMainWindow>
#include "ui_LineCutWindow.h"
#include "PlotLines.h"
class LineCutWindow : public QMainWindow
{
	Q_OBJECT

public:
	LineCutWindow(QWidget *parent = nullptr);
	~LineCutWindow();

	void initChartView();

public slots:
	void onUpdateCutLine(QList<float>, QList<QPointF>, QList<QPointF>, QList<QPointF>, QList<QPointF>);

private:
	Ui::LineCutWindowClass ui;
	PlotLines* m_chart = nullptr;

};
