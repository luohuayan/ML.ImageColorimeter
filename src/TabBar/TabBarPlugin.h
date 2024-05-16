/*************************************************
Author:jason.huang
Date:2023.1.25
Version:0.0.1
Description:Show label bar on the right side of the frame
**************************************************/

#ifndef TAB_BAR_PLUGIN
#define TAB_BAR_PLUGIN
#include "tabbar_global.h"
#include <QtPlugin>
#include <QWidget>
#include <QTabWidget>
#include <QObject>

class TABBAR_EXPORT TabBarPlugin
{
    //Q_OBJECT
public:
    virtual ~TabBarPlugin() {}
    virtual void addWidget(QWidget* widget, QString title, QIcon icon) = 0;
    virtual void selectiveDisplay() = 0;
    virtual void setSplitter() = 0;
    virtual QWidget* getWidget() = 0; //show widget
//signals:
    //this is signal
    virtual void updateTabWidgetPage(QTabWidget*) = 0;
    virtual void updateTabWidgetShowSignal(std::map<QWidget*, bool>) = 0; ////menuBarPlugin send signal to this function
};
//Plug-in identifier
#define AbstractInterface_iid "tab bar plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(TabBarPlugin, AbstractInterface_iid)

#endif
