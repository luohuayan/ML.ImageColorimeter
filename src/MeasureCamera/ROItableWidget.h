#pragma once
#include "ui_ROItableWidget.h"

class ROItableWidget : public QWidget
{
	Q_OBJECT

public:
	ROItableWidget(QWidget* parent = nullptr);
	~ROItableWidget();
	QItemSelectionModel* getSelectionModel();
	
private:
	Ui::ROItable ui;
};
