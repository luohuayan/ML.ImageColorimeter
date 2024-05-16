#include "CameraControl.h"

CameraControl::CameraControl(QObject* parent) : QObject(parent)
{
	cameraWidget = new CameraControlWidget();
}

QWidget* CameraControl::GetWidget()
{
	return cameraWidget;
}
