#include "CustomPopDialog.h"
#include <QVBoxLayout>
#include <QDebug>
CustomPopDialog::CustomPopDialog(QWidget* parent)
    : QDialog(parent)
{
    setMouseTracking(true);
    m_timer = new QTimer(this);
    setWindowFlags(Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(mousePointByTimer()));
}

CustomPopDialog::~CustomPopDialog()
{

}

void CustomPopDialog::setContentWidget(QWidget* page)
{
    if (!page)
        return;
    m_pWidget = page;
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(page);
}

QWidget* CustomPopDialog::getContentWidget()
{
    return m_pWidget;
}

void CustomPopDialog::mousePointByTimer()
{
    QPoint point = QCursor::pos();
    emit mouseGlobalPoint(point, this);
}

bool CustomPopDialog::event(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::MouseButtonRelease:
        //Event of mouse releasing window title
    case QEvent::NonClientAreaMouseButtonRelease:
    {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        if (e && e->button() == Qt::LeftButton)
        {
            emit dragRelease(e->globalPos());
        }
        m_isPress = false;
        if (m_timer->isActive() == true)
        {
            m_timer->stop();
        }
    }
    break;
    case QEvent::Close:
    {
        // Signal the pop-up window to destroy
        emit PopDialogClose(m_pWidget);
        m_isPress = false;
        if (m_timer->isActive() == true)
        {
            m_timer->stop();
        }
        break;
    }
    case QEvent::NonClientAreaMouseButtonPress:
    {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        if (e && e->button() == Qt::LeftButton)
        {
            m_isPress = true;
            if (m_timer->isActive() == false)
            {
                m_timer->start(10);
            }
        }
        break;
    }
   /* case QEvent::NonClientAreaMouseMove:
    {
        
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        
        if (m_isPress)
        {
            int a = 0;
        }

        if (e)
        {
            emit mouseGlobalPoint(e->pos());
        }
        break;
    }*/
    }
    return QDialog::event(event);
}

void CustomPopDialog::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        // Signal the pop-up window to destroy
        emit PopDialogClose(m_pWidget);
        break;

    default:
        QDialog::keyPressEvent(event);
    }
}
