#include "MenuWidget.h"
#include "aboutWidget.h"
#include <QApplication>

MenuWidget::MenuWidget(QWidget* parent) : QMenuBar(parent) {
    setStyleSheet("QMenuBar {"
        "    background-color: #c0c0c0;"
        "    font-family: Microsoft YaHei;}"
        "QMenuBar::item {"
        "    spacing: 3px; "
        "    padding: 5px 10px; "
        "    background-color: transparent; "
        "    color: black; }"
        "QMenuBar::item:selected {"
        "    background-color: #515151;"
        "    color: white;}"
        "QMenuBar::item:!enabled {"
        "    color: #808080; }");

    mode = new QLabel();
    mode->setStyleSheet("font-family: Microsoft YaHei; font-size: 12px; font-weight: bold;");
    mode->setText("Engineer Mode ");
    mode->adjustSize();
    QPushButton* switchButton = new QPushButton(QIcon(":/image/images/routine/switch.png"), "", this);
    connect(switchButton, &QPushButton::clicked, [=]() {emit sigTestModeChaged(); });
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 7, 0);
    layout->addWidget(mode);
    layout->addWidget(switchButton);
    QWidget* container = new QWidget(this);
    container->setLayout(layout);
    setCornerWidget(container, Qt::TopRightCorner);

    QMenu* fileMenu = addMenu("File");
    QAction* exitAction = new QAction("Exit", this);
    fileMenu->addAction(exitAction);
    connect(exitAction, &QAction::triggered, [=]() {
        QApplication* app;
        app->exit(0);
    });
    //fileMenu->addSeparator();
    //QMenu* subMenu = new QMenu("Locating", this);
    //openAction->setMenu(subMenu);
    //QAction* subAction1 = new QAction("Subaction 1", this);
    //subMenu->addAction(subAction1);
    //QAction* subAction2 = new QAction("Subaction 2", this);
    //subMenu->addAction(subAction2);
    //QAction* saveAction = new QAction("Save", this);
    //fileMenu->addAction(saveAction);

    QMenu* settingMenu = addMenu("Setting");
    settingMenu->setEnabled(false);

    //QAction* calibrationAction = new QAction("ImageCalibration Setting", this);
    //settingMenu->addAction(calibrationAction);
    //ImageCalibrationWidget* imageCalibrationWidget = new ImageCalibrationWidget();
    //connect(calibrationAction, &QAction::triggered, [=]() {imageCalibrationWidget->show(); });

    QMenu* viewMenu = addMenu("View");
    QAction* rightBar = new QAction("Zoom Right SideBar", this);
    QAction* measureMode = new QAction("Switch Measure Mode", this);
    QAction* selectorMode = new QAction("Selctor Style", this);
    QAction* tabMode = new QAction("Tab Mode", this);
    rightBar->setIcon(QIcon(":/image/images/routine/sidebar.png"));
    measureMode->setIcon(QIcon(":/image/images/routine/switch.png"));
    viewMenu->addAction(rightBar);
    viewMenu->addAction(measureMode);
    viewMenu->addAction(selectorMode);
    viewMenu->addAction(tabMode);
    QMenu* styleMenu = new QMenu();
    selectorMode->setMenu(styleMenu);

    QAction* style1Action = styleMenu->addAction("Icons and text");
    QAction* style2Action = styleMenu->addAction("Icons only");
    QAction* style3Action = styleMenu->addAction("Hidden");
    style1Action->setCheckable(true);
    style2Action->setCheckable(true);
    style3Action->setCheckable(true);
    QActionGroup* Actiongroup = new QActionGroup(this);
    Actiongroup->addAction(style1Action);
    Actiongroup->addAction(style2Action);
    Actiongroup->addAction(style3Action); 
    Actiongroup->setExclusive(true);

    QMenu* tabMenu = new QMenu();
    //tabMenu->setHideOnTrigger(false);
    
    tabMode->setMenu(tabMenu);
    tab1Action = tabMenu->addAction("FilterWheel");
    tab2Action = tabMenu->addAction("FoucusControl");
    tab3Action = tabMenu->addAction("CameraControl");
    tab4Action = tabMenu->addAction("FourColor");
    tab1Action->setCheckable(true);
    tab2Action->setCheckable(true);
    tab3Action->setCheckable(true);
    tab4Action->setCheckable(true);

    tab1Action->setChecked(true);
    tab2Action->setChecked(true);
    tab3Action->setChecked(true);
    tab4Action->setChecked(true);

    connect(measureMode, &QAction::triggered, [=]() {emit sigTestModeChaged(); });
    connect(rightBar, &QAction::triggered, [=]() {emit sigRightBarZoom(); });
    connect(style1Action, &QAction::triggered, [=]() { emit sigModeStyle(0); });
    connect(style2Action, &QAction::triggered, [=]() { emit sigModeStyle(1); });
    connect(style3Action, &QAction::triggered, [=]() { emit sigModeStyle(2);});

    connect(tab1Action, &QAction::toggled, [=](bool status) { emit sigTabModeStyle(1, status); });
    connect(tab2Action, &QAction::toggled, [=](bool status) { emit sigTabModeStyle(2, status); });
    connect(tab3Action, &QAction::toggled, [=](bool status) { emit sigTabModeStyle(3, status); });
    connect(tab4Action, &QAction::toggled, [=](bool status) { emit sigTabModeStyle(4, status); });

    QMenu* toolMenu = addMenu("Tools");
    toolMenu->setDisabled(true);
    QMenu* winMenu = addMenu("Window");
    QAction* fullScreen = new QAction("Full Screen", this);
    fullScreen->setIcon(QIcon(":/image/images/routine/spread.png"));
    winMenu->addAction(fullScreen);
    connect(fullScreen, &QAction::triggered, [=]() {emit sigFullScreen(); });
    QMenu* helpMenu = addMenu("Help");
    QAction* aboutAction = new QAction("About MLColorimeter", this);
    helpMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered,[=]() { (new aboutClass())->show(); });

}
void MenuWidget::updateTestMode(QString strmode)
{
    mode->setText(strmode);
}
void MenuWidget::onUpdateTabMode(QWidget* widget) 
{
    QString title = widget->windowTitle(); 
    if (title == "FilterWheel") 
    {
        tab1Action->setChecked(false);
    } 
    else if (title == "FocusControl") 
    {
        tab2Action->setChecked(false);
    } 
    else if (title == "CameraControl") 
    {
        tab3Action->setChecked(false);
    } 
    else if (title == "FourColor") 
    {
        tab4Action->setChecked(false);
    }
}
MenuWidget::~MenuWidget() {
}
