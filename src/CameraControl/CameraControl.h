#pragma once

#include "cameracontrol_global.h"
#include "CameraControlPlugin.h"
#include "CameraControlWidget.h"


class CAMERACONTROL_EXPORT CameraControl : public QObject, public CameraControlPlugin
{
	Q_OBJECT
		/*
*Use Q_INTERFACES() macro tells Qt's meta-object system about the interface and tells the meta-object system "I want to use this interface
*/
Q_INTERFACES(CameraControlPlugin)
/*
* Use Q_ PLUGIN_ METADATA() macro export plug-in
*/
Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "CameraControl.json")

public:
	CameraControl(QObject* parent = nullptr);

public:
	QWidget* GetWidget();
	//bool InitConfig();

private:
	CameraControlWidget* cameraWidget;
};