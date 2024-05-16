#ifndef COMMON_MLMOTION2D_H_
#define COMMON_MLMOTION2D_H_

#pragma once
#include"ml.h"
#include"ml_motion.h"
#include <QWidget>
#include <QtPlugin>
using namespace CORE;

//class MLMotionCallback
//{
//public:
//    virtual void NotifyMotionStateChanged(MLMotionState old_state, MLMotionState new_state) = 0;
//};


class MLMotion2D
{

public:
    virtual ~MLMotion2D() {};

    virtual bool Connect(const char* type_name) = 0;

    virtual bool Disconnect() = 0;

    virtual bool IsConnected() = 0;

    virtual void SetEnable(bool enable) = 0;

    virtual bool IsEnabled() = 0;

    virtual void MoveRelAsync(int posX, int posY) = 0;

    virtual void MoveAbsAsync(int pos, int posY) = 0;

    virtual void MoveRel(int posX, int posY) = 0;

    virtual void MoveAbs(int posX, int posY) = 0;

    virtual void StopMove() = 0;

    virtual void Homing() = 0;

    virtual bool IsHome() = 0;

    virtual void SetSpeed(int speed) = 0;

    virtual int GetSpeed() = 0;

    virtual bool IsMoving() = 0;

    virtual ML_Point GetPosition() = 0;

    virtual MLMotionState GetState() = 0;

    virtual void Subscribe(MLMotionEvent event, MLMotionCallback* callback) = 0;

    virtual void Unsubscribe(MLMotionEvent event, MLMotionCallback* callback) = 0;

    virtual void clearAlarm() = 0;

    virtual QWidget* GetWidget() = 0;
};

//Plug-in identifier
#define AbstractInterface_iid "motion2D interface plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(MLMotion2D, AbstractInterface_iid)
#endif // !COMMON_MLMOTION_H_