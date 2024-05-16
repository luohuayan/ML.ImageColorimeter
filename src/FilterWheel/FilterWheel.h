#pragma once

#include "filterwheel_global.h"
#include "FilterWheelPlugin.h"
#include "FilterWheelWidget.h"

class FILTERWHEEL_EXPORT FilterWheel : public QObject, public FilterWheelPlugin
{
    Q_OBJECT
        /*
        *Use Q_INTERFACES() macro tells Qt's meta-object system about the interface and tells the meta-object system "I want to use this interface
        */
        Q_INTERFACES(FilterWheelPlugin)
        /*
        * Use Q_ PLUGIN_ METADATA() macro export plug-in
        */
        Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "FilterWheelControl.json")
public:
    FilterWheel(QObject* parent = nullptr);

public:
    QWidget* GetWidget();
    //bool InitConfig();

private:
    FilterWheelWidget* widget;
};
