#include "TabBar.h"

TabBar::TabBar(QObject* parent) : QObject(parent)
{
	m_TabBarWidget = new TabBarWidget();
	//QObject::connect(m_TabBarWidget, &TabBarWidget::updateTabWidgetPage, this, &TabBarPlugin::updateTabWidgetPage);
	QObject::connect(m_TabBarWidget, SIGNAL(updateTabWidgetPage(QTabWidget*)), this, SLOT(updateTabWidgetPageSlot(QTabWidget*)));

	QObject::connect(this, SIGNAL(updateTabWidgetShowSignal(std::map<QWidget*, bool>)), m_TabBarWidget, SIGNAL(updateTabWidgetShow(std::map<QWidget*, bool>)));
}

void TabBar::updateTabWidgetPageSlot(QTabWidget* tabWidget)
{
	emit updateTabWidgetPage(tabWidget);
}

void TabBar::addWidget(QWidget* widget, QString title, QIcon icon)
{
	m_TabBarWidget->addWidget(widget, title, icon);
}

void TabBar::selectiveDisplay()
{
	m_TabBarWidget->selectiveDisplay();
}
void TabBar::setSplitter()
{
	m_TabBarWidget->setSplitter();
}
QWidget* TabBar::getWidget()
{
	return m_TabBarWidget;
}