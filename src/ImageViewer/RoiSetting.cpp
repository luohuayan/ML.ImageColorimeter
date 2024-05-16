#include "RoiSetting.h"
#include <QDebug>

RoiSetting* RoiSetting::self = nullptr;

RoiSetting* RoiSetting::instance()
{
	if (self == nullptr) {
		self = new RoiSetting();
	}
	return self;
}
RoiSetting::RoiSetting(QWidget* parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/image/images/logo/mlico1.ico"));
	colordlg = new QColorDialog();
	colordlg->setWindowIcon(QIcon(":/image/images/logo/mlico1.ico"));
	roiSettings->insert("width", "55");
	roiSettings->insert("height", "55");
	roiSettings->insert("offset", "400");
	roiSettings->insert("color", "#00FFFF");
	roiSettings->insert("shape", "Rectangle");
	connect(ui.width_slider, &QSlider::valueChanged, [=](int value) {ui.width_spin->setValue(value); roiSettings->insert("width", QString::number(value));});
	connect(ui.height_slider, &QSlider::valueChanged, [=](int value) {ui.height_spin->setValue(value);roiSettings->insert("height", QString::number(value));});
	connect(ui.offset_slider, &QSlider::valueChanged, [=](int value) {ui.offset_spin->setValue(value); roiSettings->insert("offset", QString::number(value)); });
	
	connect(ui.width_spin, &QSpinBox::textChanged, [=](QString value) {ui.width_slider->setValue(value.toInt()); roiSettings->insert("width", value);});
	connect(ui.height_spin, &QSpinBox::textChanged, [=](QString value) {ui.height_slider->setValue(value.toInt()); roiSettings->insert("height", value); });
	connect(ui.offset_spin, &QSpinBox::textChanged, [=](QString value) {ui.offset_slider->setValue(value.toInt()); roiSettings->insert("offset", value); });


	connect(ui.colorbtn, &QPushButton::clicked, [=](bool click) { colordlg->open(); });
	connect(colordlg, &QColorDialog::finished, [=]() {
			QColor color = colordlg->currentColor();
			if (color.isValid())
			{
				QString str_color = color.name();
				QString styleSheet = QString("background-color: %1").arg(str_color);
				ui.colorbtn->setStyleSheet(styleSheet);
				roiSettings->insert("color", str_color);
			}
		});
	connect(ui.applybtn, &QPushButton::clicked, [=]() {emit roiSettingDone(); this->close(); });
	connect(ui.revertbtn, &QPushButton::clicked, [=]() {
		ui.width_slider->setValue(55); //roiSettings->insert("width", "55");
		ui.height_slider->setValue(55);// roiSettings->insert("height", "55");
		ui.offset_slider->setValue(200); //roiSettings->insert("offset", "200");
		ui.colorbtn->setStyleSheet(QString("background-color:rgb(0,255,255)"));
		roiSettings->insert("color", "#00FFFF");
		});
	connect(ui.shapecBox, &QComboBox::currentTextChanged, [=](QString text) {roiSettings->insert("shape", ui.shapecBox->currentText());});
}

RoiSetting::~RoiSetting()
{}

QMap<QString, QString>* RoiSetting::getRoiSettings()
{
	return roiSettings;
}
void RoiSetting::initRanges(int width, int height) 
{
	ui.offset_slider->setMaximum(std::max(width, height) / 3);
	ui.offset_spin->setMaximum(ui.offset_slider->maximum());

	ui.height_slider->setMaximum(std::max(width, height) / 5);
	ui.height_spin->setMaximum(ui.height_slider->maximum());
	ui.width_slider->setMaximum(ui.height_slider->maximum());
	ui.width_spin->setMaximum(ui.height_slider->maximum());
}