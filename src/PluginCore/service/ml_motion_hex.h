#ifndef COMMON_MLMOTIONHEX_H_
#define COMMON_MLMOTIONHEX_H_

#pragma once
#include"ml.h"
#include<tuple>

using namespace ML;

class MLPIHEXMotionCallback
{
public:
    virtual void NotifyMotionStateChanged(MLMotionState old_state, MLMotionState new_state) = 0;

    //need to implement
    //virtual void NotifyMotionPotistionChanged(int X, int Y, int Z, double U, double V, double W) = 0;
};


class MLMotionHex
{

public:
    virtual ~MLMotionHex() {};

    virtual bool Connect(const char* type_name, int port) = 0;

    virtual bool Disconnect() = 0;

    virtual bool IsConnected() = 0;

    virtual void SetEnable(bool enable) = 0;

    virtual bool IsEnabled() = 0;

    virtual void MoveRelAsync(int X, int Y, int Z, double U, double V, double W) = 0;

    virtual void MoveAbsAsync(int X, int Y, int Z, double U, double V, double W) = 0;

    virtual void MoveRel(int X, int Y, int Z, double U, double V, double W) = 0;

    virtual void MoveAbs(int X, int Y, int Z, double U, double V, double W) = 0;

    virtual void StopMove() = 0;

    virtual void Homing() = 0;

    virtual bool IsHome() = 0;

    virtual void SetSpeed(double speed) = 0;

    virtual double GetSpeed() = 0;

    virtual bool IsMoving() = 0;

    virtual ML_Point3D GetPosition() = 0;
	
	virtual ML_AxialAngle GetTipTilt() = 0;

    virtual MLMotionState GetState() = 0;

    virtual void setPivot(int x, int y, int z) = 0;

    virtual ML_Point3D getPivot() = 0;

    virtual void Subscribe(MLMotionEvent event, MLPIHEXMotionCallback* callback) = 0;

    virtual void Unsubscribe(MLMotionEvent event, MLPIHEXMotionCallback* callback) = 0;


};
#endif // 