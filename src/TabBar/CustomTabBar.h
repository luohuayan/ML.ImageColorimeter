#pragma once

#include <QTabBar>

/**
 * @brief �Զ���TabBar
 * @details ��֧��Tabҳ�������ϳ�
 * @note bar.rect�ķ�Χ����Ч��Χ������tabWidget�Ŀ��
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
