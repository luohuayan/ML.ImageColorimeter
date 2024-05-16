#ifndef COMMON_MLMOTION_H_
#define COMMON_MLMOTION_H_
//#include "..\plugincore_global.h"
#include"ml.h"
#include <QObject>
#include <QtPlugin>
using namespace CORE;

class MLMotionCallback
{
public:
    virtual void NotifyMotionStateChanged(MLMotionState old_state, MLMotionState new_state) = 0;
    virtual void NotifyMotionPosition(int position) = 0;
};

class __declspec(dllexport) MLMotion
{
public:

    virtual bool Connect(const char* type_name) = 0;

    virtual bool Disconnect() = 0;

    virtual bool IsConnected() = 0;

    virtual void SetEnable(bool enable) = 0;

    virtual bool IsEnabled() = 0;

    virtual void MoveRelAsync(int step) = 0;

    virtual void MoveAbsAsync(int pos) = 0;

    virtual void MoveRel(int step) = 0;

    virtual void MoveAbs(int pos) = 0;

    virtual void StopMove() = 0;

    virtual void Homing() = 0;

    virtual bool IsHome() = 0;

    virtual void SetSpeed(int speed) = 0;

    virtual int GetSpeed() = 0;

    virtual bool IsMoving() = 0;

    virtual int GetPosition() = 0;

    virtual MLMotionState GetState() = 0;

    virtual void Subscribe(MLMotionEvent event, MLMotionCallback* callback) = 0;

    virtual void Unsubscribe(MLMotionEvent event, MLMotionCallback* callback) = 0;

    int direction_;

};
#endif // !COMMON_MLMOTION_H_