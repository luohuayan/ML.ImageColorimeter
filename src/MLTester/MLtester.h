#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MLtester.h"
#include <QLabel>
#include <QEvent>
#include <QToolTip>
#include <QStackedWidget>
#include <QMouseEvent>
#include <QApplication>

#include "CollpasePagePlugin.h"
#include "navigationBarPlugin.h"
#include "TabBarPlugin.h"
#include "MenuBarPlugin.h"
class MLtester : public QMainWindow
{
    Q_OBJECT

public:
    MLtester(QWidget *parent = nullptr);
    ~MLtester();

public:
    void InitializationInterface();
    void CreateMenuBar();
    void CreateActions();
    void CreateToolBars();
    void CreateLogBar();
    void InitStatusBar();
    void InitLayout();
    void CreateLeftNavigationBar();
    void CreateTabBar();
public slots:
    void refreshStatus(QString mesg, bool status);
    void onTestModeChaged();
    void onFullScreen();
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
private:
    Ui::MLtesterClass ui;
    QVBoxLayout* m_centerQVBoxLayout;
    QHBoxLayout* m_centerQHBoxLayout;
    QLabel* statusText;

    MenuBarPlugin* menuPlugin;
    CollpasePagePlugin* collpasePagePlugin;
    NavigationBarPlugin* LeftNavigationBarPlugin;
    TabBarPlugin* tabBarPlugin;

};
