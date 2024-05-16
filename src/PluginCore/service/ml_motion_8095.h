#ifndef COMMON_MLMOTION8095_H_
#define COMMON_MLMOTION8095_H_

#pragma once
#include"ml.h"
#include<tuple>
using namespace ML;

class ML8095MotionCallback
{
public:
    virtual void NotifyMotionStateChanged(MLMotionState old_state, MLMotionState new_state) = 0;
    virtual void NotifyMotionPosition(ML_POINT6D position6D) = 0;
};

class MLMotion8095
{

public:

    virtual bool Connect(int ID) = 0;

    virtual bool Disconnect() = 0;

    virtual bool IsConnected() = 0;

    virtual void SetEnable(bool enable) = 0;

    virtual bool IsEnabled() = 0;

    virtual void MoveRelAsync(int X, int Y, int Z, int dX, int dY[], int dZ[]) = 0;

    virtual void MoveAbsAsync(int X, int Y, int Z, int dX, int dY[], int dZ[], int toZeroMotion = 0) = 0;

    virtual void MoveRel(int X, int Y, int Z, int dX, int dY[], int dZ[]) = 0;

    virtual void MoveAbs(int X, int Y, int Z, int dX, int dY[], int dZ[], int toZeroMotion = 0) = 0;

    virtual void StopMove() = 0;

    virtual void Homing() = 0;

    virtual bool IsHome() = 0;

    virtual void SetSpeed(int speed) = 0;

    virtual int GetSpeed() = 0;

    virtual bool IsMoving() = 0;

    virtual ML_POINT6D GetPosition6D() = 0;

    virtual MLMotionState GetState() = 0;

    virtual void Subscribe(MLMotionEvent event, ML8095MotionCallback* callback) = 0;

    virtual void Unsubscribe(MLMotionEvent event, ML8095MotionCallback* callback) = 0;
};
#endif // 