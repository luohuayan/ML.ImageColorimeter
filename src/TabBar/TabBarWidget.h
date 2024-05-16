#pragma once

#include <QWidget>
#include <QPushButton>
#include <QSplitter>
#include "ui_TabBarWidget.h"
#include <QTabWidget>
#include "CustomTabWidget.h"
class TabBarWidget : public QWidget
{
	Q_OBJECT

public:
	TabBarWidget(QWidget *parent = nullptr);
	~TabBarWidget();

public:
	void addWidget(QWidget* widget, QString& title, QIcon& icon);
	void selectiveDisplay();
	void setSplitter();
private:
	void setupTabUI();
signals:
	void updateTabWidgetPage(QTabWidget*);
	//menuBarPlugin send signal to this function
	void updateTabWidgetShow(std::map<QWidget*, bool>);
public slots:
	void onZoomClicked();
   void onTabModeChanged(int index,bool status);

protected:
	virtual bool eventFilter(QObject* watched, QEvent* Event);

private:
	Ui::TabBarWidgetClass ui;
	QSplitter* m_mainSplitter;
	QPushButton* zoomButton;
	bool flag;
	bool test_mode = 1;
	CustomTabWidget* m_CustomTabWidget;
};
