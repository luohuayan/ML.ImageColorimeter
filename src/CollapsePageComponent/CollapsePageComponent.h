#ifndef COLLAPSEPAGECOMPONENT_H
#define COLLAPSEPAGECOMPONENT_H

#include "collapsepagecomponent_global.h"
#include <QFrame>
#include <QWidget>
#include <QIcon>
#include <QVBoxLayout>
#include "CollpasePagePlugin.h"

class AdvancedToolBoxPrivate;
class ToolBoxTitle;
class ToolBoxSplitterHandle;

class COLLAPSEPAGECOMPONENT_EXPORT AdvancedToolBox : public QWidget
{
    Q_OBJECT
    
public:
    explicit AdvancedToolBox(QWidget* parent = nullptr);
    ~AdvancedToolBox();

    int indexOf(QWidget* widget);
    QWidget* takeIndex(int index);
    QWidget* widget(int index);

    void setItemExpand(int index, bool expand = true);
    void setItemVisible(int index, bool visible = true);

    void setItemText(int index, const QString& text);
    void setItemIcon(int index, const QIcon& icon);
    QString itemText(int index);
    QIcon itemIcon(int index);

    int textIndentation();
    void resetTextIndentation(int indent = -1);

public:
    void SetLayout(QVBoxLayout* qVBoxLayout, int index, int stretch = 0); //index为负默认放在布局的最后面
    void setStylesheet(QString color);
    void addWidget(QWidget* widget, const QString& label, const QIcon& icon = QIcon()); //把界面加到抽屉栏

public:
    int m_currentHeight = 200;

public slots:
    void updateWidgetHeight(int height);

protected:
    bool event(QEvent* e);
    void paintEvent(QPaintEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void startDrag(int index, const QPoint& gpos);
    void mouseMoveEvent(QMouseEvent* event);
private:
    Q_DECLARE_PRIVATE(AdvancedToolBox)
        Q_DISABLE_COPY(AdvancedToolBox)
        QScopedPointer<AdvancedToolBoxPrivate> d_ptr;

private:
    friend class ToolBoxTitle;
    friend class ToolBoxSplitterHandle;
};

class ToolBoxSplitterHandle : public QWidget
{
public:
    ToolBoxSplitterHandle(AdvancedToolBox* parent);
    void setIndex(int index);
    int index();
protected:
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    int _index = -1;
    bool pressed = false;
    QPoint moveStart;
};

class COLLAPSEPAGECOMPONENT_EXPORT CollpasePage : public QObject, public CollpasePagePlugin
{
    Q_OBJECT
    Q_INTERFACES(CollpasePagePlugin)
        Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "CollapsePageComponent.json")
public:
    CollpasePage(QObject* parent = nullptr);
    ~CollpasePage();

public:
    virtual QWidget* GetWidget();

    virtual void setItemExpand(int index, bool expand = true);
    virtual void SetLayout(QVBoxLayout* qVBoxLayout, int index, int stretch = 0);
    virtual void setStyle(QString color);
    virtual void addWidget(QWidget* widget, const QString& label, const QIcon& icon = QIcon());
private:
    AdvancedToolBox* m_advancedToolBox;
};

#endif