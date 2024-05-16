#pragma once
#include "mlrecipe_global.h"
#include <QtPlugin>
#include <QWidget>

class MLRECIPE_EXPORT RecipePlugin
{
public:
    virtual ~RecipePlugin() {}

    virtual QWidget* GetWidget() = 0;
};
#define AbstractInterface_iid "recipe plugin"
Q_DECLARE_INTERFACE(RecipePlugin, AbstractInterface_iid)
