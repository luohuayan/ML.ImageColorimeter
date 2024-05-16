/*************************************************
Author:jason.huang
Date:2023.12.25
Version:0.0.1
Description:filter wheel interface control
**************************************************/

#pragma once

#include "cameracontrol_global.h"
#include <QtPlugin>
#include <QObject>
#include <QWidget>

class CAMERACONTROL_EXPORT CameraControlPlugin
{
public:
    virtual ~CameraControlPlugin() {}
    //virtual bool InitConfig() = 0;
    virtual QWidget* GetWidget() = 0;
};
//Plug-in identifier
#define AbstractInterface_iid "CameraControl plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(CameraControlPlugin, AbstractInterface_iid)

