#include "MLBusinessManage_Test.h"

#include <QFileDialog>
#include <QMessageBox>

#include "MLBusinessManage.h"
#include "MLBusinessManageConfig.h"
#include "MLBusinessManagePlugin.h"
#include "PluginCore.h"

std::string get_time() {
    auto t = std::chrono::system_clock::now();
    std::time_t curr_time = std::chrono::system_clock::to_time_t(t);
    std::tm time_info;
    localtime_s(&time_info, &curr_time);
    std::ostringstream ss;
    ss << std::put_time(&time_info, "%Y-%m-%d_%H-%M-%S");
    return ss.str();
}

MLBusinessManage_Test::MLBusinessManage_Test(QWidget* parent)
    : QMainWindow(parent) {
    ui.setupUi(this);
    using ML::MLBusinessManage::MLBusinessManage;
    MLBusinessManagePlugin* plugin =
        PluginCore::getinstance()->GetPlugin<MLBusinessManagePlugin>(
            "MLBusinessManagePlugin");
    manage = dynamic_cast<MLBusinessManage*>(plugin);
}

MLBusinessManage_Test::~MLBusinessManage_Test() {
    if (manage != nullptr) {
        delete manage;
        manage = nullptr;
    }
}

void MLBusinessManage_Test::on_SetConfigPath_clicked() {
    ui.lineEdit->clear();
    QString dir = QFileDialog::getExistingDirectory();
    ui.lineEdit->setText(dir);
    m_configPath = dir;
}

void MLBusinessManage_Test::on_Connect_clicked() {
    manage->ML_SetConfigPath(m_configPath.toStdString().c_str());

    Result rt;
    bool bl;

    rt = manage->ML_CreateModule();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_CreateModule Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_ConnectModule();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_ConnectModule Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    bl = manage->ML_IsModuleConnect();
    if (!bl) {
        QMessageBox::information(NULL, "Title", "ML_IsModuleConnect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    QMessageBox::information(NULL, "Title", "Connect finish",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);
}

void MLBusinessManage_Test::on_DisConnect_clicked() {
    Result rt;
    bool bl;
    rt = manage->ML_DisconnectModule();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DisconnectModule Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    bl = manage->ML_IsModuleConnect();
    if (bl) {
        QMessageBox::information(NULL, "Title", "ML_IsModuleConnect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    QMessageBox::information(NULL, "Title", "DisConnect finish",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);
}

void MLBusinessManage_Test::on_FilterWheelTest_clicked() {
    Result rt;
    bool bl;
    std::string str;

    for (auto config : ML::MLBusinessManage::MLBusinessManageConfig::instance()
                           ->GetModuleInfo()
                           .NDFilterConfig_Map) {
        for (auto channel : config.second.positionName_List) {
            rt = manage->ML_MoveND_XYZFilterByNameSync(config.second.name,
                                                       channel.first);
            if (!rt.success) {
                QMessageBox::information(NULL, "Title", "ML_MoveND_XYZFilterByNameSync Error",
                                         QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::Yes);
            }
            str = manage->ML_GetND_XYZChannelName(config.second.name);
            if (str != channel.first) {
                QMessageBox::information(NULL, "Title", "ML_GetND_XYZChannelName Error",
                                         QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::Yes);
            }
        }
    }
    QMessageBox::information(NULL, "Title", "FilterWheel test finish",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);
}

void MLBusinessManage_Test::on_MotionTest_clicked() {
    Result rt;
    bool bl;
    for (auto config : ML::MLBusinessManage::MLBusinessManageConfig::instance()
                           ->GetModuleInfo()
                           .MotionConfig_Map) {
        rt = manage->ML_SetFocusAsync(1000);
        if (!rt.success) {
            QMessageBox::information(NULL, "Title", "ML_SetFocusSync Error",
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::Yes);
        }
        bool ismoving = manage->ML_IsModuleMotorsMoving();
        rt = manage->ML_WaitForMovingStop(2000);
        if (!rt.success) {
            QMessageBox::information(NULL, "Title", "ML_WaitForMovingStop Error",
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::Yes);
        }
        double vid = manage->ML_GetVID();

        rt = manage->ML_SetFocusSync(800);
        if (!rt.success) {
            QMessageBox::information(NULL, "Title", "ML_SetFocusSync Error",
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::Yes);
        }
        vid = manage->ML_GetVID();

        rt = manage->ML_SetPosistionAbsSync(config.second.Name, 13);
        if (!rt.success) {
            QMessageBox::information(NULL, "Title", "ML_SetPosistionAbsSync Error",
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::Yes);
        }
        rt = manage->ML_SetPosistionAbsAsync(config.second.Name, 6);
        if (!rt.success) {
            QMessageBox::information(
                NULL, "Title", "ML_SetPosistionAbsSync Error",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        }

        double pos = manage->ML_GetMotionPosition(config.second.Name);

        rt = manage->ML_SetPositionRelSync(config.second.Name, 1);
        if (!rt.success) {
            QMessageBox::information(NULL, "Title", "ML_SetPositionRelSync Error",
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::Yes);
        }
        pos = manage->ML_GetMotionPosition(config.second.Name);
        rt = manage->ML_SetPositionRelAsync(config.second.Name, -1);
        if (!rt.success) {
            QMessageBox::information(
                NULL, "Title", "ML_SetPositionRelSync Error",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        }

        pos = manage->ML_GetMotionPosition(config.second.Name);
        rt = manage->ML_StopMotionMovement(config.second.Name);
        if (!rt.success) {
            QMessageBox::information(
                NULL, "Title", "ML_StopMotionMovement Error",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        }
    }
    QMessageBox::information(NULL, "Title", "Motion test finish",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);
}

void MLBusinessManage_Test::on_CameraTest_clicked() {
    Result rt;
    bool bl;
    std::string str;
    using namespace ML::CameraV2;

    /*rt = manage->ML_SetBinning("Vieworks", Binning::ONE_BY_ONE);
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetBinning Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    Binning bin = manage->ML_GetBinning("Vieworks");
    if (bin != Binning::ONE_BY_ONE) {
        QMessageBox::information(NULL, "Title", "ML_GetBinning Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }*/
    /*rt = manage->ML_SetBinningMode("Vieworks" ,BinningMode::AVERAGE);
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetBinningMode Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    BinningMode binMode = manage->ML_GetBinningMode("Vieworks");
    if (binMode != BinningMode::AVERAGE) {
        QMessageBox::information(NULL, "Title", "ML_GetBinningMode Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }*/
    rt = manage->ML_SetBitDepth("Vieworks", MLPixelFormat::MLMono12);
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetBitDepth Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    MLPixelFormat format = manage->ML_GetBitDepth("Vieworks");
    if (format != MLPixelFormat::MLMono12) {
        QMessageBox::information(NULL, "Title", "ML_GetBitDepth Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_SetExposureTime("Vieworks", 300);
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetExposureTime Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    double exptime = manage->ML_GetExposureTime("Vieworks");
    if (exptime != 300) {
        QMessageBox::information(NULL, "Title", "ML_GetExposureTime Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    rt = manage->ML_CaptureImageSync("Vieworks");
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_CaptureImageSync Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    cv::Mat img = manage->ML_GetImage("Vieworks");
    cv::imwrite("D:\\test\\test.tif", img);
    QMessageBox::information(NULL, "Title", "Camera test finish",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);

    rt = manage->ML_CaptureImageSync("Vieworks");
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_CaptureImageSync Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    cv::Mat img2 = manage->ML_GetImage("Vieworks");
    cv::imwrite("D:\\test\\test2.tif", img2);
    QMessageBox::information(NULL, "Title", "Camera test finish",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);

    rt = manage->ML_SetAperture("3mm");
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetAperture Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    std::string aperture = manage->ML_GetAperture();
    if (aperture != "3mm") {
        QMessageBox::information(NULL, "Title", "ML_GetAperture Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    rt = manage->ML_SetColorLight("G");
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetColorLight Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    std::string colorlight = manage->ML_GetColorLight();
    if (colorlight != "G") {
        QMessageBox::information(NULL, "Title", "ML_GetColorLight Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    std::string modulename = manage->ML_GetModuleName();
    std::string modulesn = manage->ML_GetModuleSerialNumber();
}

void MLBusinessManage_Test::on_CalibrationTest_clicked() {
    Result rt;
    bool bl;
    std::string str;
    rt = manage->ML_SetBitDepth("Vieworks", ML::CameraV2::MLPixelFormat::MLMono12);
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetBitDepth Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_SetBinningMode("Vieworks", ML::CameraV2::BinningMode::AVERAGE);
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetBinningMode Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_SetBinning("Vieworks", ML::CameraV2::Binning::FOUR_BY_FOUR);
    rt = manage->ML_MoveND_XYZFilterByNameSync("ND", "ND0");

    std::map<std::string, ML::MLBusinessManage::CaptureData> map;
    
    rt = manage->ML_MoveND_XYZFilterByNameSync("XYZ", "X");
    rt = manage->ML_SetExposureTime("Vieworks", 2);
    rt = manage->ML_CaptureImageSync("Vieworks");
    ML::MLBusinessManage::CaptureData xdata = manage->ML_GetCaptureData();
    cv::Mat img = xdata.Img;
    map.insert(std::make_pair(xdata.ColorFilter, xdata));

    rt = manage->ML_MoveND_XYZFilterByNameSync("XYZ", "Y");
    rt = manage->ML_SetExposureTime("Vieworks", 2);
    rt = manage->ML_CaptureImageSync("Vieworks");
    ML::MLBusinessManage::CaptureData ydata = manage->ML_GetCaptureData();
    map.insert(std::make_pair(ydata.ColorFilter, ydata));

    rt = manage->ML_MoveND_XYZFilterByNameSync("XYZ", "Z");
    rt = manage->ML_SetExposureTime("Vieworks", 2);
    rt = manage->ML_CaptureImageSync("Vieworks");
    ML::MLBusinessManage::CaptureData zdata = manage->ML_GetCaptureData();
    map.insert(std::make_pair(zdata.ColorFilter, zdata));

    rt = manage->ML_ClearCalibrationDataMap();
    rt = manage->ML_SetCaptureDataMap(map);
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetCaptureDataMap Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_LoadFFCData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadFFCData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    /*rt = manage->ML_LoadColorShiftData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadColorShiftData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }*/
    rt = manage->ML_LoadDistortionData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadDistortionData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_LoadFourColorData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadFourColorData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    /*rt = manage->ML_DoFFCCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoFFCCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }*/
    /*rt = manage->ML_DoColorShiftCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoColorShiftCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }*/
    /*rt = manage->ML_DoDistortionCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoDistortionCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }*/
    rt = manage->ML_DoExposureCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoExposureCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_DoFourColorCalibration();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoFourColorCalibration Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    rt = manage->ML_SaveCalibrationImage(
        "D:\\test\\weilaixing\\output\\0422\\" + get_time());
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SaveCalibrationImage Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    QMessageBox::information(NULL, "Title", "Calibration test finish",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);
}

void MLBusinessManage_Test::on_CalibrationOfflineTest_clicked() {
    manage->ML_SetConfigPath(m_configPath.toStdString().c_str());

    std::map<std::string, ML::MLBusinessManage::CaptureData> map;
    ML::MLBusinessManage::CaptureData xdata;
    xdata.Aperture = "3mm";
    xdata.ColorFilter = "X";
    xdata.NDFilter = "ND1";
    xdata.ExposureTime = 2;
    xdata.Img = cv::imread(
        "D:\\bino_test\\X.tif",
        -1);
    xdata.Binning = ML::CameraV2::Binning::TWO_BY_TWO;
    cv::resize(xdata.Img, xdata.Img, cv::Size(3960, 3002));
    map.insert(std::make_pair(xdata.ColorFilter, xdata));

    ML::MLBusinessManage::CaptureData ydata;
    ydata.Aperture = "3mm";
    ydata.ColorFilter = "Y";
    ydata.NDFilter = "ND1";
    ydata.ExposureTime = 2;
    ydata.Img = cv::imread(
        "D:\\bino_test\\Y.tif",
        -1);
    ydata.Binning = ML::CameraV2::Binning::TWO_BY_TWO;
    cv::resize(ydata.Img, ydata.Img, cv::Size(3960, 3002));
    map.insert(std::make_pair(ydata.ColorFilter, ydata));

    ML::MLBusinessManage::CaptureData zdata;
    zdata.Aperture = "3mm";
    zdata.ColorFilter = "Z";
    zdata.NDFilter = "ND1";
    zdata.ExposureTime = 2;
    zdata.Img = cv::imread(
        "D:\\bino_test\\Z.tif",
        -1);
    cv::resize(zdata.Img, zdata.Img, cv::Size(3960, 3002));
    zdata.Binning = ML::CameraV2::Binning::TWO_BY_TWO;
    map.insert(std::make_pair(zdata.ColorFilter, zdata));

    Result rt;
    rt = manage->ML_ClearCalibrationDataMap();
    rt = manage->ML_SetCaptureDataMap(map);
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetCaptureDataMap Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_LoadFFCData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadFFCData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_LoadColorShiftData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadColorShiftData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_LoadDistortionData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadDistortionData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_LoadFourColorData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadFourColorData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    rt = manage->ML_DoFFCCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoFFCCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_DoColorShiftCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoColorShiftCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_DoDistortionCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoDistortionCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_DoExposureCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoExposureCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_DoFourColorCalibration();
    if (!rt.success) {
        QMessageBox::information(
            NULL, "Title", "ML_DoFourColorCalibration Error",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }

    rt = manage->ML_SaveCalibrationImage("D:\\test\\weilaixing\\output\\" +
                                         get_time());
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SaveCalibrationImage Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    QMessageBox::information(NULL, "Title", "Calibration offline test finish",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);
}

void MLBusinessManage_Test::on_ColorCameraCalibrationTest_clicked() {
    Result rt;
    bool bl;
    std::string str;
    rt = manage->ML_MoveND_XYZFilterByNameSync("ND", "ND0");
    rt = manage->ML_SetBitDepth("Vieworks",
                                ML::CameraV2::MLPixelFormat::MLBayerGB12);
    rt = manage->ML_SetExposureTime("Vieworks", 100);
    rt = manage->ML_CaptureImageSync("Vieworks");

    std::map<std::string, ML::MLBusinessManage::CaptureData> map;
    map = manage->ML_GetColorCameraCaptureData();
    rt = manage->ML_ClearCalibrationDataMap();
    rt = manage->ML_SetCaptureDataMap(map);
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SetCaptureDataMap Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_LoadFFCData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadFFCData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_LoadColorShiftData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadColorShiftData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_LoadDistortionData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadDistortionData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_LoadFourColorData();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_LoadFourColorData Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }

    rt = manage->ML_DoFFCCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoFFCCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_DoColorShiftCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoColorShiftCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_DoDistortionCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoDistortionCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_DoExposureCorrect();
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_DoExposureCorrect Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    rt = manage->ML_DoFourColorCalibration();
    if (!rt.success) {
        QMessageBox::information(
            NULL, "Title", "ML_DoFourColorCalibration Error",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }

    rt = manage->ML_SaveCalibrationImage("D:\\test\\weilaixing\\output");
    if (!rt.success) {
        QMessageBox::information(NULL, "Title", "ML_SaveCalibrationImage Error",
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
    }
    QMessageBox::information(NULL, "Title", "ColorCamera Calibration test finish",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);
}


