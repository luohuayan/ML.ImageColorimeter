#include "DispatchRecipeModule.h"
#include "irecipewrapper.h"
#include "recipewrapperregistry.h"

DispatchRecipeModule::DispatchRecipeModule(QObject *parent) : QObject(parent)
{
    // moc for motion wrapper
    /*motion = new moc_MotionWrapper(this);
    connect(motion, SIGNAL(SendStatusSignal(CORE::WrapperStatus, QString)), this, SLOT(onResponse(ML::WrapperStatus,
    QString)));*/
    qRegisterMetaType<CORE::WrapperStatus>("CORE::WrapperStatus");
    MLSystemWrapper *sysWrapper = new MLSystemWrapper();
    setSystemWrapper(sysWrapper);
}

void DispatchRecipeModule::dispatch(const QString &model, const QString &cmd, const QString &params)
{
    if (model == "System")
    {
        system->Invoke(cmd, params);
    }
    else
    {
        IRecipeWrapper *wrapper = RecipeWrapperRegistry::Instance()->getWrapper(model);
        if (wrapper != nullptr)
        {
            if (!wrapper->getSignalConnected())
            {
                wrapper->setSignalConnected(true);
                connect(wrapper, SIGNAL(SendStatusSignal(CORE::WrapperStatus, QString)), this,
                        SLOT(onResponse(CORE::WrapperStatus, QString)));
            }
            wrapper->Invoke(cmd, params);
        }else{
            onResponse(CORE::Wrapper_Error_fatal, QString("\"%1\" model is mismatch, please check the execution fields.").arg(model));
        }
    }
}
void DispatchRecipeModule::setSystemWrapper(MLSystemWrapper *systemWrapper)
{
    system = systemWrapper;
    connect(system, SIGNAL(SendStatusSignal(CORE::WrapperStatus, QString)), this,
            SLOT(onResponse(CORE::WrapperStatus, QString)));
}

void DispatchRecipeModule::onResponse(CORE::WrapperStatus s, QString msg)
{
    emit dispatchResponseToRecipeGui(s, msg);
}