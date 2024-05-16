#include "CustomTabWidget.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPixmap>
#include <QMimeData>
#include <QPainter>
#include <QCursor>
#include <QDrag>
#include <QDebug>
#include "CustomPopDialog.h"

CustomTabWidget::CustomTabWidget(QWidget *parent)
	: QTabWidget(parent)
{
	//ui.setupUi(this);

	//Stylesheet does not work Solution
	setAttribute(Qt::WA_StyledBackground);

	initTabBar();

    setMouseTracking(true);
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(getCloseRequested(int)));
}

CustomTabWidget::~CustomTabWidget()
{
}

void CustomTabWidget::getCloseRequested(int value)
{
    removeTab(value);
}

// add a page
int CustomTabWidget::addTab(QWidget* page)
{
	return addTab(page, page->windowTitle());
}

int CustomTabWidget::addTab(QWidget* page, const QString& windowTitle)
{
    if (!page)
        return -1;

    // This window is in pop-up status, and cannot be added repeatedly
    if (m_QLPopedPage.contains(page))
    {
        return -2;
    }

    // !!! Set to release when calling close, and sometimes release when it cannot be closed
    //page->setAttribute(Qt::WA_DeleteOnClose);

    page->setWindowTitle(windowTitle);          // widget title 和 tab title一致
    const int index = QTabWidget::addTab(page, windowTitle);
    setCurrentIndex(index);
    emit updateTabWidgetPage(this);
    return index;
}

int CustomTabWidget::addTab(QWidget* page, const QIcon& icon, const QString& windowTitle)
{
    if (!page)
        return -1;

    // This window is in pop-up status, and cannot be added repeatedly
    if (m_QLPopedPage.contains(page))
    {
        return -2;
    }

    // !!! Set to release when calling close, and sometimes release when it cannot be closed
    //page->setAttribute(Qt::WA_DeleteOnClose);

    page->setWindowTitle(windowTitle);          // widget title 和 tab title一致
    const int index = QTabWidget::addTab(page, icon, windowTitle);
    setCurrentIndex(index);
    emit updateTabWidgetPage(this);
    return index;
}

// Add a fixed page, which cannot be dragged out
void CustomTabWidget::addFixedTab(QWidget* page)
{
    if (!page)
        return;
    addTab(page);
    setFixedTabPage(page);
}

//Set that a page cannot be closed and dragged out
void CustomTabWidget::setFixedTabPage(QWidget* page)
{
    if (!page)
        return;
    setFixedTabPage(indexOf(page));
}

void CustomTabWidget::setFixedTabPage(int index)
{
    if (isTabPageValid(index))
    {
        QWidget* page = this->widget(index);
        if (page && !m_QLfixedPage.contains(page))
        {
            m_QLfixedPage.push_back(page);
            //Do not display the close button, replace with nullptr
            tabBar()->setTabButton(index, QTabBar::RightSide, nullptr);
        }
    }
}

// Remove tab
void CustomTabWidget::removeTab(int index)
{
    if (isTabPageValid(index))
    {
        QWidget* page = this->widget(index);

        //Judge whether it is fixed and not deleted
        if (!page || m_QLfixedPage.contains(page))
            return;

        //The removeTab is only removed from the tabbar and not released
        QTabWidget::removeTab(index);
        emit updateTabWidgetPage(this);
        emit updateTabMode(page);
        //You can call delete to release tab page or set the tab page to WA_ DeleteOnClose, release on close
        //delete page;
        //page->close();
    }
}

// Remove tab
void CustomTabWidget::removeTab(QWidget* page)
{
    if (!page)
        return;

    removeTab(indexOf(page));
}

// Remove current tab
void CustomTabWidget::removeCurrentTab()
{
    removeTab(currentIndex());
}



// Clear all pages (no include fixed pages), only the tabs on the tabbar will be cleared, and the pop-up will not be cleared
void CustomTabWidget::clearTab()
{
    const int tab_count = this->count();

    //Delete from the back to the front, so that the index position is fixed
    for (int index = tab_count - 1; index >= 0; index--)
    {
        removeTab(index);
    }
}

// Clear all pages (include fixed pages), only the tabs on the tabbar will be cleared, and the pop-up will not be cleared
void CustomTabWidget::clearAllTab()
{
    //全部移除
    while (this->count() > 0)
    {
        //QWidget *page=this->widget(0);
        QTabWidget::removeTab(0);           // 直接调用QTabWidget函数，不判断是否为固定标签

        // if(page)
        //     page->close();
    }
    m_QLfixedPage.clear();
}

// page is valid
bool CustomTabWidget::isTabPageValid(int index) const
{
    if (index < 0 || index >= this->count())
        return false;
    return true;
}

// page is valid
bool CustomTabWidget::isTabPageValid(QWidget* page) const
{
    if (!page)
        return false;
    return isTabPageValid(indexOf(page));
}

bool CustomTabWidget::isTabPagePoped(QWidget* page) const
{
    if (!page)
        return false;

    // 是否包含
    return m_QLPopedPage.contains(page);
}

void CustomTabWidget::updateTabWidgetShow(std::map<QWidget*, bool> tabWidgetState)
{
    for (auto it = tabWidgetState.begin(); it != tabWidgetState.end(); it++)
    {
        if (it->second)
        {
            QWidget* widget = it->first;
            if (isTabPagePoped(widget))
            {
                emit closeDialogWidget(widget);
            }
            else
            {
                addTab(widget);
            }
        }
        else
        {
            QWidget* widget = it->first;
            removeTab(widget);
        }
    }
}

void CustomTabWidget::showEvent(QShowEvent* event)
{
    QTabWidget::showEvent(event);
    //When initializing, set the existing settings to be released when closing
    for (int index = 0; index < this->count(); index++)
    {
        QWidget* page = this->widget(index);
        if (page)
            page->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void CustomTabWidget::mousePressEvent(QMouseEvent* event)
{
    int a = 0;
}

void CustomTabWidget::mouseMoveEvent(QMouseEvent* event)
{
    int a = 0;
}

// Set whether the label can be closed, and connect the signal and slot when it can be closed
void CustomTabWidget::setTabsClosable(bool flag)
{
    if (flag)
    {
        connect(m_pBar, SIGNAL(QTabBar::tabCloseRequested(int)), this, SLOT(MyTabWidget::removeTab(int)));
    }

    QTabWidget::setTabsClosable(flag);
}

/**************************************/
void CustomTabWidget::initTabBar()
{
    CustomTabBar* m_pBar = new CustomTabBar(this);
    this->m_pBar = m_pBar;
    setTabBar(m_pBar);

    //Drag outside - mouse not released yet
    connect(m_pBar, &CustomTabBar::beginDragOut, this, [this, m_pBar](int index) {
        if (!isTabPageValid(index))
        {
            qDebug() << "beginDragOut index error";
            return;
        }
        QWidget* drag_tab = this->widget(index);

        //Fixed tab will not be dragged out
        if (!drag_tab || m_QLfixedPage.contains(drag_tab))
        {
            qDebug() << "beginDragOut fixed tab";
            return;
        }

        //Drag the current page as a snapshot
        //Size added with title block and border
        QPixmap pixmap(drag_tab->size() + QSize(2, 31));
        pixmap.fill(Qt::transparent); //Set image transparency
        QPainter painter(&pixmap);
        if (painter.isActive())
        {
            //I want to make a title bar above the content here
            //But i can't get the image of the default title bar. Just draw a rectangular box
            //If the external theme color is set, it needs to be changed
            QRect title_rect{ 0,0,pixmap.width(),30 };
            painter.fillRect(title_rect, Qt::white);
            painter.drawText(title_rect, Qt::AlignLeft | Qt::AlignVCenter, "  " + drag_tab->windowTitle());
            /*Return an adjusted QRectand input the coordinates of the upper left cornerand the lower right corner of the rectangle
            *QRect rect1 = QRect(0,0,10,20);
            *rect1.adjust(5,5,-1,-2);//rect1 is (5,5,9,18)
            *auto rect2 = rect1.adjusted(-2,-1,5,-3);//rect1 is (5,5,9,18) rect2 is (3,4,14,15)
            */
            painter.drawRect(pixmap.rect().adjusted(0, 0, -1, -1));
        }
        painter.end();
        drag_tab->render(&pixmap, QPoint(1, 30)); //copy widget to pixmap, QPoint is offset

        //The Drag class provides support for MIME-based drag data conversion
        QMimeData* mime = new QMimeData;
        QDrag* drag = new QDrag(m_pBar);
        drag->setMimeData(mime);
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(10, 0)); //Set the icon display position when dragging

        //The drag is released when the mouse pops up. At this time, judge whether the drag is external
        connect(drag, &QDrag::destroyed, this, [=] {
            QPoint bar_point = m_pBar->mapFromGlobal(QCursor::pos());

            if (!m_pBar->contentsRect().contains(bar_point)) {
                popPage(drag_tab);
            }
            });

        drag->exec(Qt::MoveAction);
        });
}

void CustomTabWidget::popTab(QWidget* page)
{
    if (page == nullptr)
        return;

    removeTab(indexOf(page));

    // 加入到已弹出列表中
    if (!m_QLPopedPage.contains(page))
    {
        m_QLPopedPage.push_back(page);
    }
}

void CustomTabWidget::isDialogIntoTabBar(QPoint point, QDialog* dialog)
{
    const QPoint bar_pos = tabBar()->mapFromGlobal(point);
    QRect BarRect(tabBar()->rect());
    if (BarRect.right() < 1)                                                   //Only when there is no tab can it be so small, expand the scope
    {
        BarRect.setBottom(tabBar()->rect().bottom() + 30);
        BarRect.setRight(tabBar()->rect().right() + 100);
    }
    if (BarRect.contains(bar_pos))
    {
        dialog->setWindowOpacity(0.4);
    }
    else
    {
        dialog->setWindowOpacity(1);
    }
}

// Pop up a page and create a new window to place the widget
void CustomTabWidget::popPage(QWidget* page)
{
    if (page == nullptr)
        return;

    //Remove a page from the tab without closing it
    popTab(page);

    CustomPopDialog* pPopDialog = new CustomPopDialog(this);
    connect(pPopDialog, SIGNAL(mouseGlobalPoint(QPoint, QDialog*)), this, SLOT(isDialogIntoTabBar(QPoint, QDialog*)));
    //this signal can close dialog widget
    connect(this, SIGNAL(closeDialogWidget(QWidget*)), pPopDialog, SIGNAL(PopDialogClose(QWidget*)));
    pPopDialog->setAttribute(Qt::WA_DeleteOnClose);        // !!!Set to release when calling close
    pPopDialog->setContentWidget(page);
    pPopDialog->setWindowTitle(page->windowTitle());
    pPopDialog->resize(page->size());

    //The dragged position is a little offset
    pPopDialog->move(QCursor::pos() - QPoint(10, 10));

    //Judge whether the independent window is dragged back to the tab
    connect(pPopDialog, &CustomPopDialog::dragRelease, this, [=](const QPoint& pos) {
        const QPoint bar_pos = tabBar()->mapFromGlobal(pos);

        //If you drag the widget back into the tabbar range, take it out and put it back into the tab
        //if(tabBar()->contentsRect().contains(bar_pos))                        // This writing range is too small. If everything is dragged out, it will not go back
        QRect BarRect(tabBar()->rect());
        if (BarRect.right() < 1)                                                   //Only when there is no tab can it be so small, expand the scope
        {
            BarRect.setBottom(tabBar()->rect().bottom() + 30);
            BarRect.setRight(tabBar()->rect().right() + 100);
        }

        if (BarRect.contains(bar_pos))
        {
            QWidget* content = pPopDialog->getContentWidget();    //Get widget from window

            // Remove from the list of popup windows
            if (isTabPagePoped(page))
            {
                m_QLPopedPage.removeOne(page);
            }

            this->addTab(content);                 //Add a widget to a new tag
           // pPopDialog->disconnect();
            //关闭的时候会在原来的位置闪一下？
            if (!pPopDialog->isDialogClose)
            {
                pPopDialog->isDialogClose = true;
                pPopDialog->close();
            }
        }
        });

    // When the pop-up window is destroyed, remove the widget from the_ QLPopedPage 
    connect(pPopDialog, &CustomPopDialog::PopDialogClose, this, [=](QWidget* page) {
        // 从已弹出的窗口列表中删除
        if (isTabPagePoped(page))
        {
            m_QLPopedPage.removeOne(page);
        }

        this->addTab(page);                 // 将widget加到新标签中
        //pPopDialog->disconnect();
        if (!pPopDialog->isDialogClose)
        {
            pPopDialog->isDialogClose = true;
            pPopDialog->close();
        }
        });

    pPopDialog->show();
    page->show();
    pPopDialog->activateWindow();
    pPopDialog->setFocus();
}
