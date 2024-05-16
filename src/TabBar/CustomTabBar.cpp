#include "CustomTabBar.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>


CustomTabBar::CustomTabBar(QWidget *parent)
	: QTabBar(parent)
{
    //Stylesheet does not work Solution
    setAttribute(Qt::WA_StyledBackground);
    //The tab can be dragged to exchange positions
    setMovable(true);
    //The tab displays the Close button
    setTabsClosable(true);

    setMouseTracking(true);
}

CustomTabBar::~CustomTabBar()
{}

// Rewrite the mouse event of the tab  
void CustomTabBar::mousePressEvent(QMouseEvent* event)
{
    QTabBar::mousePressEvent(event);
    if (event->button() == Qt::LeftButton && currentIndex() >= 0)
    {
        //pressPos=event->pos();
        m_bDragPress = true;
    }
}

// Rewrite the mouse event of the tab 
void CustomTabBar::mouseMoveEvent(QMouseEvent* event)
{
    QTabBar::mouseMoveEvent(event);

    //event->buttons() returns the status of the button, include Qt::LeftButton,Qt::RightButton,Qt::MidButton
    if (m_bDragPress && event->buttons())
    {
        //Is it out of the scope of tabbar
        if (!m_bDragOut && !contentsRect().contains(event->pos())) {
            m_bDragOut = true;
            emit beginDragOut(this->currentIndex());

            //The release will not be triggered after QDrag.exec, and it will be triggered manually
            //However, it seems that he still performs animation after the mouse pops up, which needs to be solved

            /*
            * <-Simulate mouse click events through coordinates.->
            * Here, mouse release is simulated according to global coordinates
            *new QMouseEvent(QEvent::MouseButtonRelease, this->mapFromGlobal(QCursor::pos()),Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
            *Qt::LeftButton: happened event, if move event, this is Qt::NoButton
            * Qt::LeftButton: The state of the mouse when an event occurs
            * Qt::NoModifier:The state of the keyboard when an event occurs
            */
            QMouseEvent* e = new QMouseEvent(QEvent::MouseButtonRelease,
                this->mapFromGlobal(QCursor::pos()),
                Qt::LeftButton,
                Qt::LeftButton,
                Qt::NoModifier);
            //mouseReleaseEvent(event);

           // Blocking sending function£ºbool sendEvent(QObject * receiver, QEvent * event)
           //Non-blocking sending function£ºvoid postEvent(QObject * receiver, QEvent * event)
            QApplication::postEvent(this, e);
        }
    }
}

// Rewrite the mouse event of the tab
void CustomTabBar::mouseReleaseEvent(QMouseEvent* event)
{
    QTabBar::mouseReleaseEvent(event);
    m_bDragPress = false;
    m_bDragOut = false;
}
