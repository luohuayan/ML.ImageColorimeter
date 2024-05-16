/*************************************************
Author:jason.huang
Date:2023.1.24
Version:0.0.1
Description:Display the navigation bar on the left side of the frame
**************************************************/

#ifndef NAVIGATION_BAR_PLUGIN
#define NAVIGATION_BAR_PLUGIN
#include "navigationbar_global.h"
#include "VerticalMenuNavigationBar.h"
#include <QtPlugin>
#include <QWidget>
#include <QLayout>

class NAVIGATIONBAR_EXPORT NavigationBarPlugin
{
public:
    virtual ~NavigationBarPlugin() {}

    virtual void SetParent(QWidget* parent, QLayout* layout) = 0; //Incoming parent interface and layout in parent interface
    virtual void addMainMenu(QString title, QIcon icon) = 0;
    virtual void addMenuWidget(QString widgetTitle, int mainMenuIndex, QWidget* widget) = 0;
    virtual void selectiveDisplay() = 0;
    virtual QWidget* GetWidget() = 0; //Return to the left navigation bar widget
    virtual VerticalMenuNavigationBar* GetNavigationBar() = 0;
    virtual void hideAndRefresh() = 0; //Refresh navigation bar display
};
//Plug-in identifier
#define AbstractInterface_iid "navigation bar plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(NavigationBarPlugin, AbstractInterface_iid)


#endif