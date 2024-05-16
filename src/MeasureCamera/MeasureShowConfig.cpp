#include "MeasureShowConfig.h"
#include <fstream>
#include <QFileInfo>
#include <filesystem>
#include <QTextStream>
#include <algorithm>

MeasureShowConfig* MeasureShowConfig::m_measureShowCfg=nullptr;

MeasureShowConfig::MeasureShowConfig() {}

MeasureShowConfig::~MeasureShowConfig() {}

MeasureShowConfig* MeasureShowConfig::GetInstance()
{
	if (m_measureShowCfg == nullptr)
	{
		m_measureShowCfg = new MeasureShowConfig();
	}
	return m_measureShowCfg;
}

void MeasureShowConfig::DeleteInstance()
{
	if (m_measureShowCfg != nullptr)
	{
		delete m_measureShowCfg;
		m_measureShowCfg = nullptr;
	}
}

bool MeasureShowConfig::LoadMeasureConfig(std::string path)
{
	json jsonmonocular;
	std::string cfgPath = path + "\\ModuleShowConfig.json";
	std::ifstream ifs(cfgPath, std::fstream::in);
	if (ifs.fail())
	{
		return false;
	}
	ifs >> jsonmonocular;
	m_moduleShow.ipdModule.enable = jsonmonocular["IPD"]["Enable"].get<int>();
	m_moduleShow.motionModule.enable = jsonmonocular["Motion"]["Enable"].get<int>();
	m_moduleShow.motionModule.motionWidget.ndFilter = jsonmonocular["Motion"]["Widget"]["NDFilter"].get<int>();
	m_moduleShow.motionModule.motionWidget.colorFilter = jsonmonocular["Motion"]["Widget"]["ColorFilter"].get<int>();
	m_moduleShow.motionModule.motionWidget.rx = jsonmonocular["Motion"]["Widget"]["RX"].get<int>();
	m_moduleShow.motionModule.motionWidget.capture = jsonmonocular["Motion"]["Widget"]["Capture"].get<int>();
	m_moduleShow.focusModule.enable = jsonmonocular["Focus"]["Enable"].get<int>();
	m_moduleShow.focusModule.focusWidget.cameraMotion = jsonmonocular["Focus"]["Widget"]["CameraMotion"].get<int>();
	m_moduleShow.focusModule.focusWidget.LenMotion = jsonmonocular["Focus"]["Widget"]["LenMotion"].get<int>();
	m_moduleShow.cameraModule.enable = jsonmonocular["Camera"]["Enable"].get<int>();
	m_moduleShow.captureModule.enable = jsonmonocular["Calcuate"]["Enable"].get<int>();
	m_moduleShow.fourColorModule.enable = jsonmonocular["FourColor"]["Enable"].get<int>();
    m_moduleShow.mtfMeasureModule.enable = jsonmonocular["MTFMeasure"]["Enable"].get<int>();
	return true;
}

bool MeasureShowConfig::LoadFilterConfig(std::string path)
{
	json m_JsonFilterMap;

	std::string filterMapfile = path + "/InstrumentConfig/FilterConvertMap.json";

	std::ifstream ifs2(filterMapfile.c_str(), std::fstream::in);

	if (ifs2.fail())
	{
		return false;
	}

	ifs2 >> m_JsonFilterMap;

	FilterArray info;

	for (size_t j = 0; j < m_JsonFilterMap["FilterConvertMap"].size(); j++)
	{
		if (m_JsonFilterMap["FilterConvertMap"][j]["Name"] == "XYZ")
		{
			for (size_t i = 0; i < m_JsonFilterMap["FilterConvertMap"][j]["Map"].size(); i++)
			{
				info.XYZArray.push_back(m_JsonFilterMap["FilterConvertMap"][j]["Map"][i]["Name"].get<std::string>());
			}
		}
		if (m_JsonFilterMap["FilterConvertMap"][j]["Name"] == "ND")
		{
			for (size_t i = 0; i < m_JsonFilterMap["FilterConvertMap"][j]["Map"].size(); i++)
			{
				info.NDArray.push_back(m_JsonFilterMap["FilterConvertMap"][j]["Map"][i]["Name"].get<std::string>());
			}
		}
		if (m_JsonFilterMap["FilterConvertMap"][j]["Name"] == "Cylinder")
		{
			for (size_t i = 0; i < m_JsonFilterMap["FilterConvertMap"][j]["Map"].size(); i++)
			{
				std::string name = m_JsonFilterMap["FilterConvertMap"][j]["Map"][i]["Name"].get<std::string>();
				int index = name.find("-");
				if (index > 0)
				{
					name = name.substr(name.find_first_of("-"));
					if (name == "-0")
					{
						name = "0";
					}
				}
                info.CylinderArray.push_back(name);
			}
		}
	}

	m_filterArray = info;


	return true;
}

bool MeasureShowConfig::LoadInstructmentModule(std::string path)
{
    m_cameraModuleArray.clear();
    m_cameraModuleNameArray.clear();

	json m_Jsoninstrument;

	std::string instrumentfile = path + "/InstrumentConfig/InstrumentConfig.json";

	std::ifstream ifs(instrumentfile.c_str(), std::fstream::in);

	if (ifs.fail())
	{
		return false;
	}

	ifs >> m_Jsoninstrument;

	auto cameraModules = m_Jsoninstrument["Modules"];
	for (auto& it : cameraModules.items())
	{
		CameraModule m_cameraModule;
		std::string name = it.value()["ModuleName"].get<std::string>();
		std::string serial = it.value()["SerialNumber"].get<std::string>();
        std::string aperture = it.value()["Aperture"].get<std::string>();
		name = name.substr(0,name.find_first_of("-"));
		m_cameraModule.name = name;
		m_cameraModule.serial = serial;
        m_cameraModule.aperture = aperture;
		m_cameraModuleArray.push_back(m_cameraModule);
	}
	for (int i = 0; i < m_cameraModuleArray.size(); i++)
	{
		m_cameraModuleNameArray.push_back(m_cameraModuleArray[i].name);
	}

	return true;
}

bool MeasureShowConfig::LoadRxConfig(std::string path)
{
	std::string serialName = "";
	for (int i = 0; i < m_cameraModuleArray.size(); i++)
	{
		serialName = m_cameraModuleArray[i].serial;
	}
	if (serialName == "")
	{
		return false;
	}
	QString filePath = QString::fromStdString(path) + "/ModuleData/" + QString::fromStdString(serialName) +"/config/RX_Selection.csv";
	QFile file(filePath);
	bool isOpen = file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!isOpen)
	{
		return false;
	}
	else
	{
		QTextStream in(&file);
		stringArray lines;
		int i = 0;
		while (!in.atEnd())
		{
			i++;
			QString line = in.readLine();
			QString rx = line.split(',')[0];
			std::string rxStr = rx.toStdString();
			if (rx.contains("d"))
			{
				rxStr = rxStr.substr(0, rxStr.find_first_of("d"));
			}
			if (i == 1)
			{
				continue;
			}
			lines.push_back(rxStr);
		}
		std::sort(lines.begin(), lines.end());
		lines.erase(unique(lines.begin(), lines.end()), lines.end());
		m_sphereArray = lines;
	}	
	file.close();
	return true;
}

ModuleShow MeasureShowConfig::GetModuleShowInfo()
{
	return m_moduleShow;
}

FilterArray MeasureShowConfig::GetFilterArray()
{
	return m_filterArray;
}

stringArray MeasureShowConfig::GetCameraModuleArray()
{
	return m_cameraModuleNameArray;
}

stringArray MeasureShowConfig::GetSphereArray()
{
	return m_sphereArray;
}

cameraModuleArray MeasureShowConfig::GetCameraModule()
{
    return m_cameraModuleArray;
}




