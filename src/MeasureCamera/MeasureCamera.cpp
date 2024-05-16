#include "MeasureCamera.h"

MeasureCamera::MeasureCamera(QObject* parent) : QObject(parent)
{
	m_MeasureWidget = new MeasureCameraWidget();
}

QWidget* MeasureCamera::GetWidget()
{
	return m_MeasureWidget;
}