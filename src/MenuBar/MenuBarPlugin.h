/*************************************************
Author:jason.huang
Date:2023.2.8
Version:0.0.1
Description:Show menu bar
**************************************************/

#ifndef MENU_BAR_PLUGIN
#define MENU_BAR_PLUGIN
#include "menubar_global.h"
#include <QtPlugin>
#include <QLayout>
#include <QMainWindow>

class MENUBAR_EXPORT MenuBarPlugin
{
public:
    virtual ~MenuBarPlugin() {}
    virtual void SetMenuBar(QMainWindow* window) = 0;
    virtual QWidget* GetWidget() = 0;
};
//Plug-in identifier
#define AbstractInterface_iid "menu bar plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(MenuBarPlugin, AbstractInterface_iid)

#endif