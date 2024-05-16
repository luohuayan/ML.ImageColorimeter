#include "CameraControlWidget.h"
#include "MLBusinessManagePlugin.h"
#include "MLBusinessManageConfig.h"
#include "PluginCore.h"
#include "LoggingWrapper.h"
#include "MeasureCameraPlugin.h"
#include <ML_addInInterface.h>

using ML::MLBusinessManage::MLBusinessManageConfig;
CameraControlWidget::CameraControlWidget(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
	init();
    ui.label_7->hide();
    ui.format->hide();
}

CameraControlWidget::~CameraControlWidget()
{
}

void CameraControlWidget::init()
{
    MLBusinessManagePlugin* managePlugin = PluginCore::getinstance()->GetPlugin<MLBusinessManagePlugin>("MLBusinessManagePlugin");
    manage = dynamic_cast<MLBusinessManage*>(managePlugin);
    ML::MLBusinessManage::ModuleConfig config = MLBusinessManageConfig::instance()->GetModuleInfo();
    for (auto it : config.CameraConfig_Map) {
        cameraName = it.second.Name;
        isColorCamera = it.second.ColourCamera;
    }
    MeasureCameraPlugin* measureCameraPlugin = PluginCore::getinstance()->GetPlugin<MeasureCameraPlugin>("MeasureCameraPlugin");
    connect(measureCameraPlugin->GetWidget(), SIGNAL(sigCamraGratLevel(int)),this, SLOT(updateGrayLevelBar(int)));
	connect(ui.binningModeBox, &QComboBox::currentTextChanged, this, &CameraControlWidget::setBinningMode);
    connect(ui.binningNumBox, &QComboBox::currentTextChanged, this, &CameraControlWidget::setBining);
    connect(ui.EtText, &QLineEdit::editingFinished, this, &CameraControlWidget::setExposureTime);
    connect(ui.autoExpBtn, &QPushButton::clicked, this,&CameraControlWidget::autoExposure);
    connect(ui.refreshBtn, &QPushButton::clicked, this, &CameraControlWidget::refreshSettingRes);
    connect(manage, SIGNAL(connectStatus(bool)), this, SLOT(updateConnectStatus(bool)));
    //if (!isColorCamera) {
    //   /* ui.binningNumBox->addItem(QString("FOUR_BY_FOUR"));*/
    //} else {
    //     ui.initEtLint->hide();
    //     ui.autoExpBtn->hide();
    //     ui.label->hide();
    //}
}
void CameraControlWidget::setBinningMode(QString binningMode) 
{
    if (manage == nullptr)
        return;
    if (static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(cameraName)) != nullptr) 
    {
        Result result;
        BinningMode binnmode = binningModeMap.key(binningMode);
        result = manage->ML_SetBinningMode(cameraName, binnmode);
        refreshSettingRes();
    }
}
void CameraControlWidget::setBining(QString binning)
{
    if (manage == nullptr)
        return;
    if (static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(cameraName)) != nullptr) 
    {
        Result result;
        Binning binn = binningMap.key(binning);
        result = manage->ML_SetBinning(cameraName, binn);
        refreshSettingRes();
    }

}
void CameraControlWidget::setExposureTime()
{
    if (manage == nullptr)
        return;
    if (static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(cameraName)) != nullptr) 
    {
        Result result;
        double expTime = ui.EtText->text().toDouble();
        result = manage->ML_SetExposureTime(cameraName, expTime);

        emit sigSetExposureTime(ui.EtText->text());
        refreshSettingRes();
    }

}
void CameraControlWidget::autoExposure() 
{
    if (manage == nullptr)
        return;

    Result result;
    MLPixelFormat m_formatType;
    int initialTime = ui.initEtLint->text().toInt();
    if (static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(cameraName)) != nullptr) 
    {
        m_formatType = isColorCamera ? MLPixelFormat::MLBayerGB12 : MLPixelFormat::MLMono12;
        manage->ML_SetBitDepth(cameraName, m_formatType);
        if (initialTime != 0) 
        {
            result = manage->ML_AutoExposureSync(cameraName, initialTime);
        } else 
        {
            result = manage->ML_AutoExposureSync(cameraName);
        }
        
        if (result.success) {
           double ET = manage->ML_GetExposureTime(cameraName);
            /*ui.EtText->setText(QString::number(ET));*/

            emit sigSetExposureTime(QString::number(ET));
            refreshSettingRes(); 
        }
        m_formatType = isColorCamera ? MLPixelFormat::MLBayerGB8 : MLPixelFormat::MLMono8;
        manage->ML_SetBitDepth(cameraName, m_formatType);
    }
}
void CameraControlWidget::refreshSettingRes() 
{
    if (manage == nullptr)
        return;
    if (static_cast<GrabberBase*>(manage->ML_GetOneModuleByName(cameraName)) != nullptr)
    {
        Binning binning = manage->ML_GetBinning(cameraName);
        BinningMode binningMode = manage->ML_GetBinningMode(cameraName);
        double ET = manage->ML_GetExposureTime(cameraName);
        MLPixelFormat format = manage->ML_GetBitDepth(cameraName);

        QString binning_str = binningMap.value(binning);
        QString binningMode_str = binningModeMap.value(binningMode);
        QString format_str = formatMap.value(format);

        ui.binningSta->setText(binning_str);
        ui.binningmodeSta->setText(binningMode_str);
        ui.etSta->setText(QString::number(ET));
        ui.format->setText(format_str);
    }
}

void CameraControlWidget::updateGrayLevelBar(int gray)
{
    ui.graylevleBar->setValue(gray);
}

void CameraControlWidget::updateConnectStatus(bool status)
{
    setBining(ui.binningNumBox->currentText());
    setBinningMode(ui.binningModeBox->currentText());
}
