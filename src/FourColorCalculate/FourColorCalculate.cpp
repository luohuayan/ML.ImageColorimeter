#include "FourColorCalculate.h"

FourColorCalculate::FourColorCalculate(QObject* parent) : QObject(parent) { 
    widget = new FourColorWidget();
}

QWidget* FourColorCalculate::GetWidget() 
{
    return widget; 
}
