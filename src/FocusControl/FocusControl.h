#pragma once

#include "focuscontrol_global.h"
#include "FocusControlPlugin.h"
#include "FocusControlWidget.h"
class FOCUSCONTROL_EXPORT FocusControl : public QObject, public FocusControlPlugin
{
    Q_OBJECT

    Q_INTERFACES(FocusControlPlugin)

    Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "FocusControl.json")
public:
    FocusControl(QObject* parent = nullptr);
public:
    QWidget* GetWidget();
private:
    FocusControlWidget* widget;
};

