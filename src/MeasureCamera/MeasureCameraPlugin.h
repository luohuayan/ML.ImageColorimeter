#ifndef MEASURECAMERA_PLUGIN
#define MEASURECAMERA_PLUGIN
#include "measure_global.h"
#include <QtPlugin>
#include <QWidget>

class MEASURE_EXPORT MeasureCameraPlugin
{
public:
    virtual ~MeasureCameraPlugin() {}

    virtual QWidget* GetWidget() = 0;
};
//Plug-in identifier
#define AbstractInterface_iid "measure plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(MeasureCameraPlugin, AbstractInterface_iid)


#endif