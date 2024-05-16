#pragma once

#include <QTabWidget>
//#include "ui_CustomTabWidget.h"
#include "CustomTabBar.h"

/**
 * @brief 自定义TabWidget
 * @details setTabBar为protected接口，要修改tabbar就得继承下
 *  如果标签页关闭后会释放
 *  因此只需要删除tab不关闭窗口时需要调用 remove，clear
 *  因此需要删除tab同时关闭窗口时需要调用 close
 * 
 *  保存弹出的widget指针，因为弹出的窗口有时候不会关闭，可能会导致重复添加
 *  在窗口弹出的时候添加到m_QLPopedPage，在窗口恢复以及关闭的时候从m_QLPopedPage移除
 *  当删除标签页的时候，判断是否为弹出的，弹出的不删除
 *  当添加标签页的时候，判断是否为弹出的，弹出的不添加
 *
 *  If the tab is closed, it will be released
 *  You only need to delete the tab without closing the window. You need to call remove, clear
 *  When you need to delete the tab and close the window, you need to call close
 *
 *  Save the pop-up widget pointer, because the pop-up window will not be closed sometimes, which may lead to repeated addition
 *  Add to m_QLPopedPage when the window pops up, and remove from m_QLPopedPage when the window restores and closes
 *  When deleting a tab, judge whether it is pop-up, and the pop-up will not be deleted
 *  When adding a tab, judge whether it is pop-up, and do not add the pop-up
 * 
 */

class CustomTabWidget : public QTabWidget
{
	Q_OBJECT

public:
	CustomTabWidget(QWidget *parent = nullptr);
	~CustomTabWidget();

public:
    // add a page  
    int addTab(QWidget* page);
    int addTab(QWidget* page, const QString&);
    int addTab(QWidget* page, const QIcon& icon, const QString& windowTitle);

    //Add a page that cannot be closed and dragged out
    //appendNormalPage+setFixedPage
    void addFixedTab(QWidget* page);
    //void addFixedTab(QWidget* page, const QString&);

    //Set that a page cannot be closed and dragged out
    void setFixedTabPage(QWidget* page);
    void setFixedTabPage(int index);

    // Remove a page and do not close it
    void removeTab(QWidget* page);
    void removeTab(int index);

    // Remove current page
    void removeCurrentTab();

    //Clear all pages (no include fixed pages), only the tabs on the tabbar will be cleared, and the pop-up will not be cleared
    void clearTab();

    //Clear all pages (include fixed pages), only the tabs on the tabbar will be cleared, and the pop-up will not be cleared
    void clearAllTab();

    // tab is valid
    bool isTabPageValid(int index) const;
    bool isTabPageValid(QWidget* page) const;

    // Check whether a page has popped up
    //bool isTabPagePoped(int index) const;
    bool isTabPagePoped(QWidget* page) const;

    // Move a page from the tab and record the widget pointer to the pop-up list
    void popTab(QWidget* page);
    // Pop up a page and create a new window to place the widget
    void popPage(QWidget* page);

    // Set whether the label can be closed, and connect the signal and slot when it can be closed
    void setTabsClosable(bool flag);

signals:
    void updateTabWidgetPage(QTabWidget*);
    void updateTabMode(QWidget*);
    //close dialog widget
    void closeDialogWidget(QWidget*);
private slots:
    void isDialogIntoTabBar(QPoint point, QDialog* dialog);
    //menuBarPlugin send signal to this function
    void updateTabWidgetShow(std::map<QWidget*, bool> tabWidgetState);
    //click tabBar close button, can send current tabBar index
    void getCloseRequested(int value);
protected:
    void showEvent(QShowEvent* event);
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    CustomTabBar* m_pBar = nullptr;
    void initTabBar();

private:
	//Ui::CustomTabWidgetClass ui;

     // Save tab pages that cannot be dragged out (such as home page)
    QList<QWidget*> m_QLfixedPage;
    // Save the dragged tab
    QList<QWidget*> m_QLPopedPage;
};
