#pragma once
#include <QWidget>
#include "ui_FocusControlWidget.h"
#include "MLBusinessManage.h"

class FocusControlWidget : public QWidget {
	Q_OBJECT

public:
	FocusControlWidget(QWidget *parent = nullptr);
	~FocusControlWidget();

public slots:
	void on_focus_Addition_clicked();
	void on_focus_subtraction_clicked();
	void on_focus_move_clicked();
	void on_focus_stop_clicked();
	void on_getPosition_clicked();
    void updateFocusControlShow(int id, bool status);
    void updateConnectStatus(bool status);

private:
	void initUI();

private:
	Ui::FocusControlWidgetClass ui;
    ML::MLBusinessManage::MLBusinessManage* manage;
    std::string motionName = "";
};
