#include "TabBarWidget.h"
#include <QTabBar>
#include <PluginCore.h>
#include <FilterWheelPlugin.h>
#include <FocusControlPlugin.h>
#include <CameraControlPlugin.h>
#include <FourColorPlugin.h>
#include <MenuBarPlugin.h>

TabBarWidget::TabBarWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setupTabUI();
}

TabBarWidget::~TabBarWidget()
{
}

void TabBarWidget::addWidget(QWidget * widget, QString & title, QIcon & icon)
{
	/*m_tabWidget->insertTab(-1, widget, title);
	int index = m_tabWidget->indexOf(widget);
	m_tabWidget->setTabIcon(index, icon);*/
	m_CustomTabWidget->addTab(widget, icon, title);
}

void TabBarWidget::selectiveDisplay()
{
	test_mode = !test_mode;
	zoomButton->setVisible(test_mode);
	this->setVisible(test_mode);
}

void TabBarWidget::setupTabUI()
{
	m_CustomTabWidget = new CustomTabWidget();
	connect(m_CustomTabWidget, SIGNAL(updateTabWidgetPage(QTabWidget*)), this, SIGNAL(updateTabWidgetPage(QTabWidget*)));
	//menuBarPlugin send signal to this function
	connect(this, SIGNAL(updateTabWidgetShow(std::map<QWidget*, bool>)), m_CustomTabWidget, SLOT(updateTabWidgetShow(std::map<QWidget*, bool>)));
    connect(m_CustomTabWidget, SIGNAL(updateTabMode(QWidget*)),
            PluginCore::getinstance()
                ->GetPlugin<MenuBarPlugin>("MenuBarPlugin")
                ->GetWidget(),
            SLOT(onUpdateTabMode(QWidget*)));
	m_CustomTabWidget->tabBar()->setStyleSheet("QTabBar{background-color:rgba(180,180,180,100%);\
                                            color: black;   border-radius: 15px; font-family: Microsoft YaHei; border: 2px groove gray; border-style: outset;}"
		"QTabBar:hover{background-color:rgb(250, 200, 200); color: black;}"
		"QTabBar:pressed{background-color:rgb(85, 170, 255); border-style: inset; }"
		"QTabBar::tab:selected{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0.278409 rgba(173, 206, 218, 255), stop:1 rgba(255, 255, 255, 255));}"
	);
	this->layout()->addWidget(m_CustomTabWidget);
	this->setMaximumWidth(700);
	this->setMinimumWidth(700);
}

void TabBarWidget::setSplitter()
{
	flag = true;
	m_mainSplitter = new QSplitter(Qt::Horizontal, this);
	m_mainSplitter->setFrameStyle(QFrame::NoFrame);
	m_mainSplitter->setHandleWidth(0);

	zoomButton = new QPushButton(this);
	zoomButton->setStyleSheet(
		"QPushButton{ background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0.2 rgba(238, 238,238, 255),stop:0.5 #C3E5E7 ,stop:0.8 rgba(238, 238, 238, 255)); border:none; padding:0px;}"
		"QPushButton:hover{background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0.2 rgba(238, 238,238, 255),stop:0.5 #C3E5FF ,stop:0.8 rgba(238, 238, 238, 255)); border:none; padding:0px;border-style: outset;}"
	);
	zoomButton->installEventFilter(this);
	zoomButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	zoomButton->setIcon(QIcon(":/image/images/routine/right_arrow_disabled.png"));
	zoomButton->setFixedWidth(8);
	zoomButton->setFocusPolicy(Qt::NoFocus);
	this->layout()->removeWidget(m_CustomTabWidget);
	m_mainSplitter->addWidget(m_CustomTabWidget);
	//this->layout()->addItem(new QSpacerItem(0, 160, QSizePolicy::Expanding, QSizePolicy::Fixed));
	this->layout()->addWidget(zoomButton);
	this->layout()->addWidget(m_mainSplitter);

	connect(zoomButton, SIGNAL(clicked()), this, SLOT(onZoomClicked()));
}

bool TabBarWidget::eventFilter(QObject* watched, QEvent* Event)
{
	if (watched == zoomButton && Event->type() == QEvent::HoverMove)
	{
		this->setCursor(Qt::PointingHandCursor);
	}
	else
	{
		this->setCursor(Qt::ArrowCursor);
	}
	return QWidget::eventFilter(watched, Event);
}

void TabBarWidget::onZoomClicked()
{
	if (flag)  
	{
		flag = false;
		zoomButton->setIcon(QIcon(":/image/images/routine/left_arrow_disabled.png"));
		//m_tabWidget->setVisible(false);
		m_CustomTabWidget->setVisible(false);
		this->setMaximumWidth(18);
		this->setMinimumWidth(18);
	}
	else
	{
		flag = true;
		zoomButton->setIcon(QIcon(":/image/images/routine/right_arrow_disabled.png"));
	    //m_tabWidget->setVisible(true);
		m_CustomTabWidget->setVisible(true);
		this->setMinimumWidth(700);
		this->setMaximumWidth(700);
	}
}
void TabBarWidget::onTabModeChanged(int index,bool status) 
{
    QWidget* curWidget = nullptr;
    switch (index) 
	{
        case 1:
            curWidget = PluginCore::getinstance()->GetPlugin<FilterWheelPlugin>("FilterWheelPlugin")->GetWidget();
            break;
        case 2:
            curWidget =
                PluginCore::getinstance()->GetPlugin<FocusControlPlugin>("FocusControlPlugin")->GetWidget();
            break;
        case 3:
            curWidget =
                PluginCore::getinstance()->GetPlugin<CameraControlPlugin>("CameraControlPlugin")->GetWidget();
            break;
        case 4:
            curWidget = PluginCore::getinstance()->GetPlugin<FourColorPlugin>("FourColorPlugin")->GetWidget();
            break;
        default:
            break;
    }
    if (!status) {
        m_CustomTabWidget->removeTab(curWidget);
    } else {
        m_CustomTabWidget->addTab(curWidget);
    }
}