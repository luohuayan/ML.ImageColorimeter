#include "MenuBar.h"

MenuBar::MenuBar(QObject* parent): QObject(parent)
{
	m_menuWidget = new MenuWidget();
}

MenuBar::~MenuBar()
{
}

void MenuBar::SetMenuBar(QMainWindow* window)
{
	window->setMenuBar(m_menuWidget);
}

QWidget* MenuBar::GetWidget()
{
	return m_menuWidget;
}
