#pragma once

#include "navigationbar_global.h"
#include <QWidget>
#include <QIcon>
#include "VerticalMenuNavigationBar.h"
#include "navigationBarPlugin.h"

class NAVIGATIONBAR_EXPORT NavigationBar : public QObject, public NavigationBarPlugin
{
    Q_OBJECT
        /*
        *Use Q_INTERFACES() macro tells Qt's meta-object system about the interface and tells the meta-object system "I want to use this interface
        */
        Q_INTERFACES(NavigationBarPlugin)
        /*
        * Use Q_ PLUGIN_ METADATA() macro export plug-in
        */
        Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "NavigationBar.json")
public:
    NavigationBar(QObject* parent = nullptr);

public:
    void SetParent(QWidget* parent, QLayout* layout);
    void addMainMenu(QString title, QIcon icon);
    void addMenuWidget(QString widgetTitle, int mainMenuIndex, QWidget* widget);
    void selectiveDisplay();
    QWidget* GetWidget();
    VerticalMenuNavigationBar* GetNavigationBar();
    void hideAndRefresh();
private:
    VerticalMenuNavigationBar* m_verticalMenuNavigationBar;
    QStackedWidget* m_stackedWidget;
    QWidget* m_navigationBarShow;
};
