#ifndef COMMON_MLMOTION3D_H_
#define COMMON_MLMOTION3D_H_
//#include "..\plugincore_global.h"
#include"ml.h"
//#include"ml_motion.h"
#include <QWidget>
#include <QtPlugin>
using namespace CORE;

class MLMotion3DCallback
{
public:
    virtual void NotifyMotion3DStateChanged(CORE::MLMotionState old_state, CORE::MLMotionState new_state) = 0;
    virtual void NotifyMotion3DPosition(int x,int y,int z) = 0;
};

class MLMotion3D
{
public:
    virtual ~MLMotion3D() {};

    virtual bool Connect(const char* type_name) = 0;

    virtual bool Disconnect() = 0;

    virtual bool IsConnected() = 0;

    virtual void SetEnable(bool enable) = 0;

    virtual bool IsEnabled() = 0;

    virtual void MoveRelAsync(int posX, int posY, int posZ) = 0;

    virtual void MoveAbsAsync(int pos, int posY, int posZ) = 0;

    virtual void MoveRelAsync(const char* axis, int pos) = 0;

    virtual void MoveAbsAsync(const char* axis, int pos) = 0;

    virtual void MoveRel(int posX, int posY, int posZ) = 0;

    virtual void MoveAbs(int posX, int posY, int posZ) = 0;

    virtual void StopMove() = 0;

    virtual void Homing() = 0;

    virtual bool IsHome() = 0;

    virtual void SetSpeed(int speed) = 0;

    virtual int GetSpeed() = 0;

    virtual bool IsMoving() = 0;

    virtual ML_Point3D GetPosition() = 0;

    virtual CORE::MLMotionState GetState() = 0;

    virtual void Subscribe(CORE::MLMotionEvent event, MLMotion3DCallback* callback) = 0;

    virtual void Unsubscribe(CORE::MLMotionEvent event, MLMotion3DCallback* callback) = 0;

    virtual void clearAlarm() = 0;

    virtual QWidget* GetWidget() = 0;
};

//Plug-in identifier
#define AbstractInterface_iid "motion3D interface plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(MLMotion3D, AbstractInterface_iid)

#endif // !COMMON_MLMOTION3D_H_