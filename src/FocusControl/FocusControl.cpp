#include "FocusControl.h"

FocusControl::FocusControl(QObject* parent) : QObject(parent)
{
	widget = new FocusControlWidget();
}

QWidget* FocusControl::GetWidget()
{
	return widget;
}
