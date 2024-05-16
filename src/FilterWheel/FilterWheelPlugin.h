/*************************************************
Author:jason.huang
Date:2023.12.25
Version:0.0.1
Description:filter wheel interface control
**************************************************/

#ifndef CONFIGURATION_PLUGIN
#define CONFIGURATION_PLUGIN
#include "filterwheel_global.h"
#include <QtPlugin>
#include <QObject>
#include <QWidget>

class FILTERWHEEL_EXPORT FilterWheelPlugin
{
public:
    virtual ~FilterWheelPlugin() {}
    //virtual bool InitConfig() = 0;
    virtual QWidget* GetWidget() = 0;
};
//Plug-in identifier
#define AbstractInterface_iid "FilterWheel plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(FilterWheelPlugin, AbstractInterface_iid)

#endif
