#include "VerticalMenuNavigationBar.h"
#include "LoggingWrapper.h"
VerticalMenuNavigationBar::VerticalMenuNavigationBar(QStackedWidget* stackedWidget, QWidget* parent) : QWidget(parent)
{
    m_stackedWidget = stackedWidget;
    vLayout = new QVBoxLayout(this);
    vLayout->setAlignment(Qt::AlignTop);
    vLayout->setContentsMargins(0, 0, 0, 0);

    menuDialog = new QDialog();
    menuDialog->setStyleSheet("background-color:rgb(0,0,64);");
    menuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    childDialog = new QDialog();
    childDialog->setStyleSheet("background-color:rgb(0,0,64);");
    childDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    titleButton = new QPushButton();
    titleButton->setMinimumHeight(33);
    titleButton->setMinimumWidth(90);
    titleButton->setMaximumHeight(33);
    titleButton->setMaximumWidth(90);
    titleButton->setContentsMargins(0, 0, 0, 0);
    titleButton->setStyleSheet("QPushButton{background-color:rgb(212,201,201);"
        "color:rgb(255,255,255);"
        "font:bold 14px;"
        "text-align:left;"
        "font-family:'Microsoft YaHei';}"
    );

    vLayoutDialog = new QVBoxLayout();
    vLayoutDialog->setContentsMargins(0, 0, 0, 0);
    vLayoutDialog->addWidget(titleButton);
    vLayoutDialog->addWidget(childDialog);
    vLayoutDialog->setSpacing(0);
    menuDialog->setLayout(vLayoutDialog);
    connect(this, SIGNAL(switchWidgetSignal(int, QString)), this, SLOT(switchWidget(int, QString)));

    setMouseTracking(true);
    this->parentWidget()->setMouseTracking(true);
  
}

void VerticalMenuNavigationBar::hideAndRefresh()
{
    for (int i = 0; i < menuBarList.length(); i++)
    {
        menuBarList.at(i)->pushbutton->setStyleSheet(
            "QToolButton{color:rgb(255,255,255);  font-size: 12px; border-radius: 0px; padding-top: 10px; }"
        );
    }

    menuDialog->hide();
}

void VerticalMenuNavigationBar::addMainMenu(QString title, QIcon icon)
{
    QToolButton* button = new QToolButton();
    button->setCheckable(true);
    button->setStyleSheet(
        "QToolButton{color:rgb(255,255,255);font-size: 12px;font-family: Microsoft YaHei; border-radius: 0px; padding-top: 10px;}"
        "QToolButton:checked{border:1px solid #1e3943;background-color:#1e3943;}"
        "QToolButton:hover{border:2px groove gray; border-style:outset;}"
        "QToolButton:pressed{border:1px solid #adceda;color:#adceda;background - color:#adceda;}"
    );
    //button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button->setIcon(icon);
    button->setIconSize(QSize(40, 40));
    button->setText(title);

    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    /*
    *Note: Setting a fixed width and height can easily cause the components in the layout to overlap,
    *or the components between the layout and the layout to overlap
    */
    //button->setFixedSize(80, 80);
    button->setFixedWidth(100);
    vLayout->setSpacing(0);
    vLayout->addWidget(button, Qt::AlignLeft);

    button->installEventFilter(this);

    QMenu* menu = new QMenu();
    menu->setMinimumWidth(90);
    menu->setStyleSheet(
        "QMenu{"
        "background-color:rgb(0,0,64);"
        "margin:0px;"
        "}"
        "QMenu::item "
        "{"
        "font-size:12px;"
        "color:rgb(255,225,255);"
        "font-family:Microsoft YaHei;"
        "background-color:rgb(52,73,94,20);"
        "padding:5px 5px; "
        "margin:0px 0px;"
        "border-bottom:1px solid rgb(52,73,94,180);"
        "}"


        "QMenu::item:selected "
        "{"
        "background-color: rgb(255,189,155);"
        "font-size:12px;"
        "color:rgb(0,0,0);"
        "font-family:Microsoft YaHei;"
        "}"

        "QMenu::item:hover "
        "{"
        "font-size:12px;"
        "color:rgb(0,0,0);"
        "background-color: rgb(155,189,155);"
        "font-family:Microsoft YaHei;"
        "}");
    menu->setParent(childDialog);

    MenuBarListNode* menuBarListNode = new MenuBarListNode();
    menuBarListNode->pushbutton = button;
    menuBarListNode->titleStr = "";
    menuBarListNode->menu = menu;
    menuBarListNode->secondCounts = 0;
    menuBarList.push_back(menuBarListNode);
}

void VerticalMenuNavigationBar::addMenuWidget(QString widgetTitle, int mainMenuIndex, QWidget* widget)
{
    if (menuBarList.size() > mainMenuIndex)
    {
        menuBarList[mainMenuIndex]->widgetTitle.push_back(widgetTitle);
        menuBarList[mainMenuIndex]->secondCounts++;
        static int stackedWidgetNum = -1;
        m_stackedWidget->addWidget(widget);
        stackedWidgetNum++;
        m_stackedWidget->widget(stackedWidgetNum)->setMouseTracking(true);

        if (menuBarList[mainMenuIndex]->secondCounts > 1)
        {
            for (int i = 0; i < menuBarList[mainMenuIndex]->secondCounts; i++)
            {
                
                QAction* action = menuBarList[mainMenuIndex]->menu->addAction(menuBarList[mainMenuIndex]->widgetTitle[i]);
                connect(action, &QAction::triggered, [=] {
                    emit switchWidgetSignal(mainMenuIndex, menuBarList[mainMenuIndex]->widgetTitle[i]);
                    hideAndRefresh();
                    });
            }
            
        }
        else
        {
            connect(menuBarList[mainMenuIndex]->pushbutton, &QToolButton::clicked, [=]{
                menuBarList[m_lastWidgetNum]->pushbutton->setChecked(false);
                QWidget* currentWidget = m_stackedWidget->widget(mainMenuIndex);
                m_stackedWidget->setCurrentIndex(mainMenuIndex);
                m_lastWidgetNum = mainMenuIndex;
                emit sigStackedWidgetChanged(menuBarList[mainMenuIndex]->widgetTitle[0]);

                });
        }
    }
}

void VerticalMenuNavigationBar::selectiveDisplay()
{
    if (test_mode)//oper
    {
        for (size_t i = 0; i < menuBarList.size()-1; i++)
        {
            menuBarList[i]->pushbutton->hide();
        }
        update();
        m_stackedWidget->setCurrentIndex(menuBarList.size()-1);
        test_mode = 0;
        emit sigUpdateMode("Operator Mode ");
    }
    else//eng
    {
        for (size_t i = 0; i < menuBarList.size()-1; i++)
        {
            menuBarList[i]->pushbutton->show();
        }
        update();
        m_stackedWidget->setCurrentIndex(m_lastWidgetNum);
        test_mode = 1;
        emit sigUpdateMode("Engineer Mode ");
    }
}

void VerticalMenuNavigationBar::onModeStyle(int style)
{
    Qt::ToolButtonStyle tb_style;
    for (int i = 0; i < menuBarList.size(); i++)
    {
        menuBarList[i]->pushbutton->show();
        parentWidget()->show();
        if (0 == style) 
        {
            menuBarList[i]->pushbutton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        }
        else if (1 == style)
        {
            menuBarList[i]->pushbutton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        }
        else if (2 == style)
        {
            menuBarList[i]->pushbutton->hide();
            parentWidget()->hide();
        }
     
    }
}

void VerticalMenuNavigationBar::switchWidget(int mainMenuIndex, QString widgetTitle)
{
    int beforeWidgetNum = 0;
    for (int i = -1; i < mainMenuIndex; i++)
    {
        for (int j = 0; j < menuBarList[i + 1]->widgetTitle.size(); j++)
        {
            if (i + 1 == mainMenuIndex && menuBarList[i + 1]->widgetTitle[j] == widgetTitle) break;
            beforeWidgetNum++;
        }
        
    }
    m_stackedWidget->setCurrentIndex(beforeWidgetNum);
}

void VerticalMenuNavigationBar::mouseReleaseEvent(QMouseEvent* event)
{
    
}

void VerticalMenuNavigationBar::mouseMoveEvent(QMouseEvent* event)
{
    int a = 0;
}


