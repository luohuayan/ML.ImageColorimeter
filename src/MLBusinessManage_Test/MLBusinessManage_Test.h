#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_MLBusinessManage_Test.h"

namespace ML {
    namespace MLBusinessManage {
class MLBusinessManage;
}
}

class MLBusinessManage_Test : public QMainWindow {
    Q_OBJECT

 public:
    MLBusinessManage_Test(QWidget *parent = nullptr);
    ~MLBusinessManage_Test();

 public slots:
    void on_SetConfigPath_clicked();
    void on_Connect_clicked();
    void on_DisConnect_clicked();
    void on_FilterWheelTest_clicked();
    void on_MotionTest_clicked();
    void on_CameraTest_clicked();
    void on_CalibrationTest_clicked();
    void on_CalibrationOfflineTest_clicked();
    void on_ColorCameraCalibrationTest_clicked();

 private:
    Ui::MLBusinessManage_TestClass ui;
    QString m_configPath;
    ML::MLBusinessManage::MLBusinessManage* manage;
};
