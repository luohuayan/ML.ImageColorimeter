#ifndef VERTICALMENUNAVIGATIONBAR_H
#define VERTICALMENUNAVIGATIONBAR_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QDialog>
#include <QPushButton>
#include <QMenu>
#include <QEvent>
#include <QMouseEvent>
#include <QLabel>
#include <QToolButton>
#include <QStackedWidget>
//#include "MTFMeasureWidget.h"
//#include "IQMeasureWidget.h"
////class MTFMeasureWidget;
////class IQMeasureWidget;
class  VerticalMenuNavigationBar : public QWidget
{
    Q_OBJECT
public:
    VerticalMenuNavigationBar(QStackedWidget* stackedWidget, QWidget* parent = nullptr);
    void hideAndRefresh();
    void addMainMenu(QString title, QIcon icon);
    void addMenuWidget(QString widgetTitle, int mainMenuIndex, QWidget* widget);
    void selectiveDisplay();
private slots:
    void switchWidget(int mainMenuIndex, QString widgetTitle);
    void onModeStyle(int style);
signals:
    void sigStackedWidgetChanged(QString widgetTitle);
    void sigUpdateMode(QString mode);
protected:
    //bool eventFilter(QObject* obj, QEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
private:
    QDialog* menuDialog;
    QDialog* childDialog;

    QVBoxLayout* vLayout;
    QVBoxLayout* vLayoutDialog;
    
    QPushButton* titleButton;

    struct MenuBarListNode
    {
        QToolButton* pushbutton;
        QMenu* menu;
        QString titleStr;
        std::vector<QString> widgetTitle;
        int secondCounts;
    };

    QList<MenuBarListNode*> menuBarList;

signals:
    void SendIndexToMain(QString index);
    void switchWidgetSignal(int mainMenuIndex, QString widgetTitle);
private:
    QStackedWidget* m_stackedWidget;
    int m_lastWidgetNum = 0;
    bool test_mode = 1;
};

#endif // VERTICALMENUNAVIGATIONBAR_H
