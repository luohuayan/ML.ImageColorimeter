#pragma once
#include <QObject>
#include <QWidget>
#include <QtPlugin>

#include "fourcolorcalculate_global.h"

class FOURCOLORCALCULATE_EXPORT FourColorPlugin {
 public:
    virtual ~FourColorPlugin() {}
    virtual QWidget* GetWidget() = 0;
};
// Plug-in identifier
#define AbstractInterface_iid "FourColor plugin"
// Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(FourColorPlugin, AbstractInterface_iid)
