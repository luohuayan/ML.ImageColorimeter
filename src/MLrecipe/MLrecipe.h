#pragma once
#include "mlrecipe_global.h"
#include "RecipePlugin.h"
#include "RecipeEngineWidget.h"
#include <QObject>
class MLRECIPE_EXPORT MLrecipe : public QObject, public RecipePlugin
{
public:
    Q_OBJECT
        /*
        *Use Q_INTERFACES() macro tells Qt's meta-object system about the interface and tells the meta-object system "I want to use this interface
        */
        Q_INTERFACES(RecipePlugin)
        /*
        * Use Q_ PLUGIN_ METADATA() macro export plug-in
        */
        Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "MLrecipe.json")
public:
    MLrecipe(QObject* parent = nullptr);
    QWidget* GetWidget();
private:
    QWidget* m_recipeWidget;
};
