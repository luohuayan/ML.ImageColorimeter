#pragma once
#include "focuscontrol_global.h"
#include <QtPlugin>
#include <QObject>
#include <QWidget>

class FOCUSCONTROL_EXPORT FocusControlPlugin
{
public:
    virtual ~FocusControlPlugin() {}
    virtual QWidget* GetWidget() = 0;
};
//Plug-in identifier
#define AbstractInterface_iid "FocusControl plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(FocusControlPlugin, AbstractInterface_iid)


