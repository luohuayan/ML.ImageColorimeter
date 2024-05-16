#pragma once

#include "measure_global.h"
#include <QObject>
#include "MeasureCameraPlugin.h"
#include "MeasureCameraWidget.h"

class MEASURE_EXPORT MeasureCamera : public QObject, public MeasureCameraPlugin
{
    Q_OBJECT
        /*
        *Use Q_INTERFACES() macro tells Qt's meta-object system about the interface and tells the meta-object system "I want to use this interface
        */
        Q_INTERFACES(MeasureCameraPlugin)
        /*
        * Use Q_ PLUGIN_ METADATA() macro export plug-in
        */
        Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "MeasureCamera.json")
public:
    MeasureCamera(QObject* parent = nullptr);

public:
    QWidget* GetWidget();
private:
    MeasureCameraWidget* m_MeasureWidget;
};
