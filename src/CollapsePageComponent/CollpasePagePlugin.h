/*************************************************
Author:jason.huang
Date:2023.1.20
Version:0.0.1
Description:Add a folding function to the interface to fold it
**************************************************/

#ifndef COLLPASEPAGE_PLUGIN_H
#define COLLPASEPAGE_PLUGIN_H
#include "collapsepagecomponent_global.h"
#include <QObject>
#include <QWidget>
#include <QtPlugin>
#include <QVBoxLayout>
#include <QIcon>

class COLLAPSEPAGECOMPONENT_EXPORT CollpasePagePlugin
{
public:
    virtual ~CollpasePagePlugin() {}
    virtual void setItemExpand(int index, bool expand = true) = 0;
    virtual void SetLayout(QVBoxLayout* qVBoxLayout, int index, int stretch = 0) = 0;
    virtual void setStyle(QString color) = 0;
    virtual void addWidget(QWidget* widget, const QString& label, const QIcon& icon = QIcon()) = 0;
    virtual QWidget* GetWidget() = 0;
};
//Plug-in identifier
#define AbstractInterface_iid "Collpase page plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(CollpasePagePlugin, AbstractInterface_iid)

#endif
