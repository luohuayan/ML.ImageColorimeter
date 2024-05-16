#pragma once

#include <QTabBar>

/**
 * @brief 自定义TabBar
 * @details 以支持Tab页的拖入拖出
 * @note bar.rect的范围是有效范围而不是tabWidget的宽度
 * 
 * To support drag and drop of tab pages
 * The range of bar.rect is a valid range, not the width of tabWidget
 */

class CustomTabBar  : public QTabBar
{
	Q_OBJECT

public:
	CustomTabBar(QWidget *parent = nullptr);
	~CustomTabBar();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    //send Signal when drop of
    void beginDragOut(int index);
    
private:
    //Mouse down
    bool m_bDragPress = false;
    //Drag to the outside
    bool m_bDragOut = false;
    //Position when pressed
    //QPoint pressPos;
};
