#pragma once

#include <QWidget>
#include "ui_FilterWheelWidget.h"
#include "MLBusinessManage.h"

class FilterWheelWidget : public QWidget
{
	Q_OBJECT

public:
	FilterWheelWidget(QWidget *parent = nullptr);
	~FilterWheelWidget();
signals:
    void sigfilterChanged(QString type,QString name);
 public slots:
    void updateConnectStatus(bool connect);
	void setNDFilterAsync(QString name);
	void setXYZFilterAsync(QString name);
    void on_refreshButton_clicked();
    void updateNDComb(QString name) { ui.NDList->setCurrentText(name); };
    void updateXYZComb(QString name) {ui.XYZList->setCurrentText(name);};
 private:
	void initUI();

	struct Mycmp {
        bool operator()(const std::pair<std::string, int>& p1,
                        const std::pair<std::string, int>& p2) {
            return p1.second < p2.second;
        }
    };

private:
	Ui::FilterWheelWidgetClass ui;
    ML::MLBusinessManage::MLBusinessManage* manage;
};
