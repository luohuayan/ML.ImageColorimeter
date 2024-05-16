#include "FilterWheel.h"


FilterWheel::FilterWheel(QObject* parent) : QObject(parent)
{
	widget = new FilterWheelWidget();
}

QWidget* FilterWheel::GetWidget()
{
	return widget;
}
