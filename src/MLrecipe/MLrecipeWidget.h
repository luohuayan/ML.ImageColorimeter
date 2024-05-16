#pragma once
#include "ui_MLrecipeWidget.h"

class MLrecipeWidget : public QWidget
{
	Q_OBJECT

public:
	MLrecipeWidget(QWidget* parent = nullptr);
	~MLrecipeWidget();
private slots:
	void on_loadRcipe_clicked();
private:
	Ui::MLrecipeClass ui;

};

