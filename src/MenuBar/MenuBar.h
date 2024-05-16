#pragma once
#include <QObject>
#include <QMenuBar>
#include <QMainWindow>
#include "menubar_global.h"
#include "MenuBarPlugin.h"
#include "MenuWidget.h"
class MENUBAR_EXPORT MenuBar : public QObject, public MenuBarPlugin
{
    Q_OBJECT
        /*
        *Use Q_INTERFACES() macro tells Qt's meta-object system about the interface and tells the meta-object system "I want to use this interface
        */
        Q_INTERFACES(MenuBarPlugin)
        /*
        * Use Q_ PLUGIN_ METADATA() macro export plug-in
        */
        Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "MenuBar.json")
public:
    MenuBar(QObject* parent = nullptr);
    ~MenuBar();
    void SetMenuBar(QMainWindow* window);
    QWidget* GetWidget();
private:
    MenuWidget* m_menuWidget;
};
