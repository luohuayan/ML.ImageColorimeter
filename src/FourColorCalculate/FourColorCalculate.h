#pragma once
#include "fourcolorcalculate_global.h"
#include "FourColorPlugin.h"
#include "FourColorWidget.h"
class FOURCOLORCALCULATE_EXPORT FourColorCalculate : public QObject,public FourColorPlugin {
    Q_OBJECT

    Q_INTERFACES(FourColorPlugin)

    Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "FourColorCalculate.json")
 public:
    FourColorCalculate(QObject* parent = nullptr);

 public:
    QWidget* GetWidget();

 private:
    FourColorWidget* widget;
};

