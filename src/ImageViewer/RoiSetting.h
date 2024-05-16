#pragma once
#include <QWidget>
#include "ui_RoiSetting.h"
#include <QColorDialog>
class RoiSetting : public QWidget
{
	Q_OBJECT

public:
	RoiSetting(QWidget* parent = nullptr);
	~RoiSetting();
	static RoiSetting* instance();
	QMap<QString, QString>*  getRoiSettings();
signals:
	void roiSettingDone();
public slots:
	void initRanges(int,int);
private:
	Ui::ROISetting ui;
	static RoiSetting* self;
	QColorDialog* colordlg = nullptr;
	QMap<QString, QString>* roiSettings = new QMap<QString, QString>();
};
