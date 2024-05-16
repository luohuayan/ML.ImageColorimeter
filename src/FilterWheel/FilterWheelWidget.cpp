#include "FilterWheelWidget.h"
#include <fstream>
#include <QListView>
#include <QMessageBox>
#include <QIntValidator>
#include "PluginCore.h"
#include "LoggingWrapper.h"
#include "MLBusinessManagePlugin.h"
#include "MLBusinessManageConfig.h"
#include "service/Result.h"
#include "ML_addInInterface.h"
using namespace ML::MLBusinessManage;
using ML::MLBusinessManage::MLBusinessManageConfig;
using ML::MLFilterWheel::MLNDFilterConfiguation;
FilterWheelWidget::FilterWheelWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    initUI();

}

FilterWheelWidget::~FilterWheelWidget()
{}


void FilterWheelWidget::initUI() {
    MLBusinessManagePlugin* managePlugin = PluginCore::getinstance()->GetPlugin<MLBusinessManagePlugin>("MLBusinessManagePlugin");
    manage = dynamic_cast<MLBusinessManage*>(managePlugin);

    if (manage != nullptr) {
        connect(manage, SIGNAL(filterWheelStatus(bool)), this,SLOT(updateConnectStatus(bool)));
    }

    // Add filter item from config
    ML::MLBusinessManage::ModuleConfig m_moduleCig = MLBusinessManageConfig::instance()->GetModuleInfo();
    std::map<std::string, MLNDFilterConfiguation> ndConfig_Map = m_moduleCig.NDFilterConfig_Map;
    std::map<std::string, MLNDFilterConfiguation>::iterator iter;
    for (iter = ndConfig_Map.begin(); iter != ndConfig_Map.end(); ++iter) {
        std::string name = iter->first;
        MLNDFilterConfiguation ndConfig = iter->second;

        std::vector<std::pair<std::string, int>> nameVec(
            ndConfig.positionName_List.begin(),
            ndConfig.positionName_List.end());
        std::sort(nameVec.begin(), nameVec.end(), Mycmp());
        for (auto i = nameVec.begin(); i != nameVec.end();
             ++i) {
            if (name == "ND") {
                ui.NDList->addItem((i->first).c_str());
            } else {
                ui.XYZList->addItem((i->first).c_str());
            }
        }
    }
    std::map<std::string, CameraConfig> cameraConfig_Map = m_moduleCig.CameraConfig_Map;
    for (auto it : cameraConfig_Map) 
    {
        CameraConfig cameraConfig = it.second;
        if (cameraConfig.ColourCamera) {
            ui.groupBox_2->hide();
            ui.label_4->hide();
            ui.xyzSta->hide();
        }
    }

    ui.connectStatus->setText("Not Connected");
    connect(ui.NDList, &QComboBox::currentTextChanged, this,&FilterWheelWidget::setNDFilterAsync);
    connect(ui.XYZList, &QComboBox::currentTextChanged, this,&FilterWheelWidget::setXYZFilterAsync);
}

void FilterWheelWidget::setNDFilterAsync(QString name)
{
    if (manage == nullptr)
        return;
    if (static_cast<FilterWheelBase*>(manage->ML_GetOneModuleByName("ND")) !=nullptr )
    {
        std::string nd = name.toStdString();
        Result ret =
            manage->ML_MoveND_XYZFilterByNameAsync("ND", name.toStdString());
        emit sigfilterChanged("ND", name);
    }
}

void FilterWheelWidget::setXYZFilterAsync(QString name)
{
    if (manage == nullptr)
        return;
    if (static_cast<FilterWheelBase*>(manage->ML_GetOneModuleByName("XYZ")) !=nullptr)
    {
        std::string xyz = name.toStdString();
        Result ret =
        manage->ML_MoveND_XYZFilterByNameAsync("XYZ", name.toStdString());
        emit sigfilterChanged("XYZ", name);
    }
}

void FilterWheelWidget::on_refreshButton_clicked() 
{
    if (manage == nullptr)
        return;
    if (static_cast<FilterWheelBase*>(manage->ML_GetOneModuleByName("XYZ")) != nullptr) 
    {
         std::string curND = manage->ML_GetND_XYZChannelName("ND");
        std::string curXYZ = manage->ML_GetND_XYZChannelName("XYZ");
        ui.ndSta->setText(QString::fromStdString(curND));
        ui.xyzSta->setText(QString::fromStdString(curXYZ));
    }
}

void FilterWheelWidget::updateConnectStatus(bool connect) 
{
    if (connect) {
        setNDFilterAsync(ui.NDList->currentText());
        setXYZFilterAsync(ui.XYZList->currentText());
        ui.connectStatus->setText("Connected");
    } 
    else 
    {
        ui.connectStatus->setText("Not Connected");
    }
}