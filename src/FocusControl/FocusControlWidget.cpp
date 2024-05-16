#include "FocusControlWidget.h"
#include "PluginCore.h"
#include "LoggingWrapper.h"
#include <QButtonGroup>
#include "service/Result.h"
#include "MLBusinessManagePlugin.h"
#include "MLBusinessManageConfig.h"
#include "ML_addInInterface.h"
using namespace ML::MLBusinessManage;
using ML::MLBusinessManage::MLBusinessManageConfig;
FocusControlWidget::FocusControlWidget(QWidget *parent): QWidget(parent)
{
	ui.setupUi(this);
    initUI();
}

FocusControlWidget::~FocusControlWidget()
{}

void FocusControlWidget::initUI()
{
    QButtonGroup* btnGroup = new QButtonGroup();
    btnGroup->addButton(ui.focus_relative);
    btnGroup->addButton(ui.focus_global);
    btnGroup->addButton(ui.focus_VID);
    bool ret = connect(btnGroup, SIGNAL(buttonToggled(int, bool)), this, SLOT(updateFocusControlShow(int, bool)));
    ui.focus_relative->setChecked(true);

    MLBusinessManagePlugin* managePlugin = PluginCore::getinstance()->GetPlugin<MLBusinessManagePlugin>("MLBusinessManagePlugin");
    manage = dynamic_cast<MLBusinessManage*>(managePlugin);
    if (manage != nullptr)
        connect(manage, SIGNAL(motionStatus(bool)), this,
                SLOT(updateConnectStatus(bool)));
    ML::MLBusinessManage::ModuleConfig m_moduleCig = MLBusinessManageConfig::instance()->GetModuleInfo();
    std::map<std::string, MotionConfig> motionConfig_Map = m_moduleCig.MotionConfig_Map;
    std::map<std::string, MotionConfig>::iterator iter;
    for (iter = motionConfig_Map.begin(); iter != motionConfig_Map.end();++iter) {
        std::string name = iter->first;
        motionName = name;
    }
}


void FocusControlWidget::on_focus_Addition_clicked()
{
    if (manage == nullptr)
        return;
    if (static_cast<ActuatorBase*>(manage->ML_GetOneModuleByName(motionName)) !=nullptr &&
        static_cast<ActuatorBase*>(manage->ML_GetOneModuleByName(motionName))->IsConnected()) 
    {
        Result result;
        double pos = ui.focus_relative_text->text().toDouble();
        result = manage->ML_SetPositionRelSync(motionName,pos);
        if (!result.success) {
            return;
        }
        on_getPosition_clicked();
    }
}

void FocusControlWidget::on_focus_subtraction_clicked()
{
    if (manage == nullptr)
        return;
    if (static_cast<ActuatorBase*>(manage->ML_GetOneModuleByName(motionName)) !=nullptr &&
        static_cast<ActuatorBase*>(manage->ML_GetOneModuleByName(motionName))->IsConnected()) 
    {
        Result result;
        double pos = ui.focus_relative_text->text().toDouble();
        double negPos = -pos;
        result = manage->ML_SetPositionRelSync(motionName, negPos);
        if (!result.success) {
            return;
        }
        on_getPosition_clicked();
    }
}

void FocusControlWidget::on_focus_move_clicked()
{
    if (manage == nullptr)
        return;
    if (static_cast<ActuatorBase*>(manage->ML_GetOneModuleByName(motionName)) !=nullptr) 
    {
        Result result;
        if (ui.focus_VID->isChecked()) {
            double vid = ui.focus_vid_text->text().toDouble();
            result = manage->ML_SetFocusSync(vid);
            if (!result.success) {
                return;
            }
            on_getPosition_clicked();
            return;
        }

        double pos = ui.focus_global_text->text().toDouble();
        result = manage->ML_SetPosistionAbsSync(motionName, pos);
        if (!result.success) {
            return;
        }
        on_getPosition_clicked();

    }
   
}

void FocusControlWidget::on_focus_stop_clicked()
{
    if (manage == nullptr)
        return;
    if (static_cast<ActuatorBase*>(manage->ML_GetOneModuleByName(motionName)) != nullptr) 
    {
        Result result;
        result = manage->ML_StopMotionMovement(motionName);
        on_getPosition_clicked();
    }

}

void FocusControlWidget::on_getPosition_clicked()
{
    if (manage == nullptr)
        return;
    if (static_cast<ActuatorBase*>(manage->ML_GetOneModuleByName(motionName)) !=nullptr)
    {
        double camera_pos = manage->ML_GetMotionPosition(motionName);
        double vid = manage->ML_GetVID();
        ui.camera_pos->setText(QString::number(camera_pos) + ",");
        ui.vid->setText(QString::number(vid) + ",");
    }
}

void FocusControlWidget::updateFocusControlShow(int id, bool status)
{
    if (ui.focus_global->isChecked())
    {
        ui.focus_global_text->setEnabled(true);
        ui.focus_move->setEnabled(true);
        ui.focus_stop->setEnabled(true);
        ui.focus_relative_text->setEnabled(false);
        ui.focus_subtraction->setEnabled(false);
        ui.focus_Addition->setEnabled(false);
        ui.focus_vid_text->setEnabled(false);
        
    }
    else if (ui.focus_VID->isChecked())
    {
        ui.focus_vid_text->setEnabled(true);
        ui.focus_move->setEnabled(true);
        ui.focus_stop->setEnabled(true);
        ui.focus_global_text->setEnabled(false);
        ui.focus_relative_text->setEnabled(false);
        ui.focus_subtraction->setEnabled(false);
        ui.focus_Addition->setEnabled(false);
    }
    else
    {
        ui.focus_relative_text->setEnabled(true);
        ui.focus_subtraction->setEnabled(true);
        ui.focus_Addition->setEnabled(true);
        ui.focus_global_text->setEnabled(false);
        ui.focus_move->setEnabled(false);
        ui.focus_stop->setEnabled(false);
        ui.focus_vid_text->setEnabled(false);
    }   
}

void FocusControlWidget::updateConnectStatus(bool status)
{
    if (status)
    {
        on_getPosition_clicked();
    }
}
