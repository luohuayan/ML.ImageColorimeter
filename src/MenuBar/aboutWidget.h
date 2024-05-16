#pragma once
#include "ui_aboutWidget.h"
#include "../MLTester/resource1.h"
class aboutClass : public QWidget{
 public:
    aboutClass(QWidget* parent = nullptr) {
        ui.setupUi(this); 
        this->setWindowIcon(QIcon(":/image/images/logo/mlico1.ico"));
        ui.appIcon->setIcon(QIcon(":/image/images/logo/mlico1.ico"));
        ui.version->setText(QString(PRODUCTVERSION_));
        connect(ui.okBtn, &QPushButton::clicked, this,
                [=]() {this->close(); });
    };
    ~aboutClass(){};

 private:
    Ui::aboutWidgetClass ui;
 };