#include "NavigationBar.h"

NavigationBar::NavigationBar(QObject* parent) : QObject(parent)
{
	m_stackedWidget = new QStackedWidget();
	m_navigationBarShow = new QWidget();
	m_verticalMenuNavigationBar = new VerticalMenuNavigationBar(m_stackedWidget, m_navigationBarShow);
	m_navigationBarShow->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
	m_navigationBarShow->setStyleSheet("QWidget{background-color:#383838;}");
	m_navigationBarShow->setMinimumWidth(100);
	m_navigationBarShow->setMaximumWidth(100);
	m_stackedWidget->setMouseTracking(true);
	
}

void NavigationBar::SetParent(QWidget* parent, QLayout* layout)
{
	m_stackedWidget->setParent(parent);
	m_stackedWidget->parentWidget()->setMouseTracking(true);
	m_navigationBarShow->setParent(parent);
	layout->addWidget(m_stackedWidget);
}

void NavigationBar::addMainMenu(QString title, QIcon icon)
{
	m_verticalMenuNavigationBar->addMainMenu(title, icon);
}

void NavigationBar::addMenuWidget(QString widgetTitle, int mainMenuIndex, QWidget* widget)
{
	m_verticalMenuNavigationBar->addMenuWidget(widgetTitle, mainMenuIndex, widget);
}

QWidget* NavigationBar::GetWidget()
{
	return m_navigationBarShow;
}
VerticalMenuNavigationBar* NavigationBar::GetNavigationBar()
{
	return m_verticalMenuNavigationBar;
}

void NavigationBar::hideAndRefresh()
{
	m_verticalMenuNavigationBar->hideAndRefresh();
}
void NavigationBar::selectiveDisplay()
{
	m_verticalMenuNavigationBar->selectiveDisplay();
}