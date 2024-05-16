#include "MLtester.h"

#include <QHash>
#include <QVariant>
#include "PluginCore.h"
#include "LogPlugin.h"

#include "MeasureCameraPlugin.h"
//#include "IQMeasurePlugin.h"
#include "RecipePlugin.h"
#include "service/ml_motion2D.h"
#include "LoggingWrapper.h"
#include "FilterWheelPlugin.h"
#include "FocusControlPlugin.h"
#include "CameraControlPlugin.h"
#include "mtfmodelconfig.h"
#include <FourColorPlugin.h>

MLtester::MLtester(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/image/images/logo/mlico1.ico"));
    setWindowTitle("MLtester");
    setMouseTracking(true);
    ui.centralWidget->setMouseTracking(true);
    InitializationInterface();
    connect(PluginCore::getinstance()->GetPlugin<MeasureCameraPlugin>("MeasureCameraPlugin")->GetWidget(), SIGNAL(sigConnectStatus(QString,bool)),this,SLOT(refreshStatus(QString,bool)));
    connect(menuPlugin->GetWidget(),SIGNAL(sigTestModeChaged()),this,SLOT(onTestModeChaged()));
    connect(menuPlugin->GetWidget(), SIGNAL(sigFullScreen()), this, SLOT(onFullScreen()));
    connect(menuPlugin->GetWidget(), SIGNAL(sigRightBarZoom()),tabBarPlugin->getWidget(),SLOT(onZoomClicked()));
    connect(menuPlugin->GetWidget(), SIGNAL(sigTabModeStyle(int,bool)),tabBarPlugin->getWidget(), SLOT(onTabModeChanged(int,bool)));


    connect(menuPlugin->GetWidget(), SIGNAL(sigModeStyle(int)), LeftNavigationBarPlugin->GetNavigationBar(), SLOT(onModeStyle(int)));
    
    connect(LeftNavigationBarPlugin->GetNavigationBar(), SIGNAL(sigUpdateMode(QString)), menuPlugin->GetWidget(), SLOT(updateTestMode(QString)));
}

MLtester::~MLtester()
{
    delete PluginCore::getinstance();
}

void MLtester::InitializationInterface()
{
    InitLayout();
    CreateMenuBar();
    CreateLeftNavigationBar();
    CreateLogBar();
    //InitStatusBar();
    CreateTabBar();
    
}

void MLtester::CreateMenuBar()
{
    menuPlugin = PluginCore::getinstance()->GetPlugin<MenuBarPlugin>("MenuBarPlugin");
    if (menuPlugin)
        menuPlugin->SetMenuBar(this);
}

void MLtester::CreateActions()
{
}

void MLtester::CreateToolBars()
{
}


void MLtester::CreateLogBar()
{
    collpasePagePlugin = PluginCore::getinstance()->GetPlugin<CollpasePagePlugin>("CollpasePagePlugin");
    if (collpasePagePlugin)
    {
        collpasePagePlugin->GetWidget()->setParent(ui.centralWidget);
        collpasePagePlugin->SetLayout(m_centerQVBoxLayout, -1);
    }
    
    LoggingWrapper::getinstance()->SetParent(collpasePagePlugin->GetWidget());
    collpasePagePlugin->addWidget(LoggingWrapper::getinstance()->GetLogWidget(), "Log", QIcon(":/image/images/routine/log.png")); 
}

void MLtester::InitStatusBar()
{
    statusBar()->setMaximumHeight(35);
    statusBar()->setStyleSheet(
        "QStatusBar{color:rgb(255,255,255);}"
        "QStatusBar{font-size: 12px;}"
        "QStatusBar{border:2px groove gray;}"
        "QStatusBar{background-color:rgba(246,248,244);}"
        "QStatusBar::item{border:0px}"
    );
    statusText = new QLabel("");
    statusText->setStyleSheet("QLabel{font-size:14px;font-family: Microsoft YaHei; font-weight: bold;color:#146a12}");
    statusBar()->addWidget(statusText);
}
void MLtester::refreshStatus(QString mesg,bool status)
{
    statusBar()->clearMessage();
    if (status)
    {
        statusText->setStyleSheet("QLabel{font-size:14px; font-weight: bold;color:#146a12}");
    }
    else
    {
        statusText->setStyleSheet("QLabel{font-size:14px; font-weight: bold;color:#ca342d}");

    }
    statusText->setText(mesg);
}
void MLtester::InitLayout()
{
    m_centerQVBoxLayout = new QVBoxLayout();
    m_centerQHBoxLayout = new QHBoxLayout(this);
    ui.centralWidget->setLayout(m_centerQHBoxLayout);
    m_centerQHBoxLayout->setContentsMargins(0, 0, 0, 0);
    ui.centralWidget->layout()->setSpacing(0);
}


void MLtester::CreateLeftNavigationBar()
{
    LeftNavigationBarPlugin = PluginCore::getinstance()->GetPlugin<NavigationBarPlugin>("NavigationBarPlugin");
    MeasureCameraPlugin* MeasurePlugin = PluginCore::getinstance()->GetPlugin<MeasureCameraPlugin>("MeasureCameraPlugin");
    RecipePlugin* recipePlugin = PluginCore::getinstance()->GetPlugin<RecipePlugin>("RecipePlugin");
    //connect(LeftNavigationBarPlugin->GetNavigationBar(), SIGNAL(sigStackedWidgetChanged(QString)), MeasurePlugin->GetWidget(), SLOT(removeOrAddImageView(QString)));
    //connect(LeftNavigationBarPlugin->GetNavigationBar(), SIGNAL(sigStackedWidgetChanged(QString)), iqPlugin->GetWidget(), SLOT(removeOrAddImageView(QString)));

    if (LeftNavigationBarPlugin)
    {
        ui.centralWidget->layout()->addWidget(LeftNavigationBarPlugin->GetWidget());
        ui.centralWidget->layout()->addItem(m_centerQVBoxLayout);

        LeftNavigationBarPlugin->SetParent(ui.centralWidget, m_centerQVBoxLayout);
        int num = 0;

        if (MeasurePlugin) {
            LeftNavigationBarPlugin->addMainMenu("Measure", QIcon(":/image/images/routine/MTF.png"));
            LeftNavigationBarPlugin->addMenuWidget("Measure", num, MeasurePlugin->GetWidget());
            num++;
        }

        //if (iqPlugin)
        //{
        //    LeftNavigationBarPlugin->addMainMenu("IQMeasure", QIcon(":/image/images/routine/IQ.png"));
        //    LeftNavigationBarPlugin->addMenuWidget("IQMeasure", num, iqPlugin->GetWidget());
        //    num++;
        //}
        if (recipePlugin)
        {
            LeftNavigationBarPlugin->addMainMenu("Recipe", QIcon(":/image/images/routine/recipe.png"));
            LeftNavigationBarPlugin->addMenuWidget("Recipe",num, recipePlugin->GetWidget());
            num++;
        }

    }
    
}

void MLtester::CreateTabBar()
{
    tabBarPlugin = PluginCore::getinstance()->GetPlugin<TabBarPlugin>("TabBarPlugin");
    tabBarPlugin->setSplitter();
    if (tabBarPlugin)
        ui.centralWidget->layout()->addWidget(tabBarPlugin->getWidget());

    FilterWheelPlugin* filterWheelPlugin = PluginCore::getinstance()->GetPlugin<FilterWheelPlugin>("FilterWheelPlugin");
    if (filterWheelPlugin && tabBarPlugin)
        tabBarPlugin->addWidget(filterWheelPlugin->GetWidget(), "FilterWheel", QIcon());

    FocusControlPlugin* focusControlPlugin = PluginCore::getinstance()->GetPlugin<FocusControlPlugin>("FocusControlPlugin");
    if (focusControlPlugin && tabBarPlugin)
        tabBarPlugin->addWidget(focusControlPlugin->GetWidget(), "FocusControl", QIcon());

    CameraControlPlugin* cameraControlPlugin = PluginCore::getinstance()->GetPlugin<CameraControlPlugin>("CameraControlPlugin");
    if (cameraControlPlugin && tabBarPlugin)
        tabBarPlugin->addWidget(cameraControlPlugin->GetWidget(), "CameraControl", QIcon());

     FourColorPlugin* fourColorPlugin =
        PluginCore::getinstance()->GetPlugin<FourColorPlugin>("FourColorPlugin");
    if (cameraControlPlugin && tabBarPlugin)
         tabBarPlugin->addWidget(fourColorPlugin->GetWidget(),"FourColor", QIcon());
}

void MLtester::onTestModeChaged()
{
    LeftNavigationBarPlugin->selectiveDisplay();
    tabBarPlugin->selectiveDisplay();
}

void MLtester::onFullScreen()
{
    if (isFullScreen()) {
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    }
    else {
        setWindowState(windowState() | Qt::WindowFullScreen);
    }
}

void MLtester::mousePressEvent(QMouseEvent* event)
{
   /* if (!menuNavigationBar->menuDialog->isHidden() && ((QCursor::pos().x() < menuNavigationBar->menuDialog->mapToGlobal(QPoint(0, 0)).x() || QCursor::pos().x() > menuNavigationBar->menuDialog->mapToGlobal(QPoint(0, 0)).x() + menuNavigationBar->menuDialog->rect().width()) || (QCursor::pos().y() < menuNavigationBar->menuDialog->mapToGlobal(QPoint(0, 0)).y() || QCursor::pos().y() > menuNavigationBar->menuDialog->mapToGlobal(QPoint(0, 0)).y() + menuNavigationBar->menuDialog->rect().height())))
        menuNavigationBar->hideAndRefresh();*/
}

void MLtester::mouseMoveEvent(QMouseEvent* event)
{
    NavigationBarPlugin* LeftNavigationBarPlugin = PluginCore::getinstance()->GetPlugin<NavigationBarPlugin>("NavigationBarPlugin");
   // if (LeftNavigationBarPlugin) LeftNavigationBarPlugin->hideAndRefresh();
    //menuNavigationBar->hideAndRefresh();
    //ui.centralWidget->setCursor(Qt::SizeVerCursor);
  
}

void MLtester::paintEvent(QPaintEvent* event)
{
}

void MLtester::resizeEvent(QResizeEvent* event)
{
    
}

void MLtester::mouseReleaseEvent(QMouseEvent* event)
{

}