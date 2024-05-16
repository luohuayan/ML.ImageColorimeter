#pragma once

#include "tabbar_global.h"
#include <QWidget>
#include "TabBarPlugin.h"
#include "TabBarWidget.h"

class TABBAR_EXPORT TabBar : public QObject, public TabBarPlugin
{
    Q_OBJECT
        /*
        *Use Q_INTERFACES() macro tells Qt's meta-object system about the interface and tells the meta-object system "I want to use this interface
        */
        Q_INTERFACES(TabBarPlugin)
        /*
        * Use Q_ PLUGIN_ METADATA() macro export plug-in
        */
        Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "TabBar.json")

public:
    TabBar(QObject* parent = nullptr);
    void addWidget(QWidget* widget, QString title, QIcon icon);
    void selectiveDisplay();
    void setSplitter();
    QWidget* getWidget();

signals:
    void updateTabWidgetPage(QTabWidget*) override;
    //menuBarPlugin send signal to this function
    void updateTabWidgetShowSignal(std::map<QWidget*, bool>) override;
private slots:
    void updateTabWidgetPageSlot(QTabWidget* tabWidget);
    
private:
    TabBarWidget* m_TabBarWidget;
};
