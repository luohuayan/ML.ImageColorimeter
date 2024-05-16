#pragma once

#include <QObject>
#include "plugincore_global.h"
#include <vector>
#include <map>
#include <QDateTime>
#include <QTextStream>
#include "opencv2/opencv.hpp"
#include "service/Result.h"
#include <QMap>
#include <unordered_map>

#define TEMP_DATA std::pair<std::vector<float>, std::unordered_map<std::string, std::vector<float>>>

enum DPACOLOR {
	RED = 0,
    GREEN = 1,
	BLUE = 2,
	WHITE =3,
	CLOSE =4
};
enum MeasureDUT {
	SLB =0,
	DUT =1
};

struct ImageNameInfo{
    QString deviceColor = "white";
    QString deviceImage = "Sloid";
    cv::Point3f eyeboxCoord = cv::Point3f(0,0,0);
	int eye = 1;           //left 1,right 2
    QString xyzFilter = "X"; //X,Y,Z
	bool isRawImage = true;

	ImageNameInfo()
    {
    }

	~ImageNameInfo()
    {
    }

	QString imageFileNotDevice()
    {
        return QString("X%1Y%2Z%3_EYE%4_%5")
            .arg(eyeboxCoord.x)
            .arg(eyeboxCoord.y)
            .arg(eyeboxCoord.z)
            .arg(eye)
            .arg(xyzFilter);
	}

	QString imageFile(){
        return QString("result%1")
			.arg(xyzFilter);
	}
};

struct DutControlInfo{
	QString colorLight;
};

// Temp Data from Delphi DUT
class TempData
{
  public:
    QString timeStamp;
    QString side;
    QVector<float> arm_temps;
    QMap<QString, QVector<float>> die_temps;
    QString tempJsonString;

    // Assuming QVector of floats
    static QString vec2String(const QVector<float> &vector)
    {
        QStringList strings;
        strings.reserve(vector.size());

        for (int i : vector)
        {
            strings.append(QString::number(i));
        }

        QString parsedString = QStringLiteral("{%1}").arg(strings.join(','));
        return convertJsonToCSVString(parsedString);
    }

    static QString getFirstValueFromVector(const QVector<float> &vector)
    {
        if (vector.isEmpty())
        {
            return "";
        }

        return QString::number(vector[0]);
    }

    // helper method for json strings to make them non-broken in csv files
    static QString convertJsonToCSVString(QString jsonString)
    {
        return "\"" + jsonString.replace("\"", "\'") + "\"";
    }

    TempData()
    {
    }

    TempData(QString time, QString sideString, TEMP_DATA data, QString jsonString = "")
    {
        timeStamp = time;
        side = sideString;
        parseTempData(data);
        if (!jsonString.isEmpty())
        {
            tempJsonString = convertJsonToCSVString(jsonString);
        }
        else
        {
            tempJsonString = "";
        }
    }

    void parseTempData(TEMP_DATA data)
    {
        std::vector<float> armTempVector = data.first;
        std::unordered_map<std::string, std::vector<float>> dieTempVector = data.second;

        arm_temps = QVector<float>::fromStdVector(armTempVector);
        std::vector<std::string> colors = {"red", "green", "blue"};

        for (std::string color : colors)
        {
            die_temps[QString::fromStdString(color)] = QVector<float>::fromStdVector(dieTempVector[color]);
        }
    }
};

class PLUGINCORE_EXPORT MetricsData : public QObject
{
	Q_OBJECT

public:
	~MetricsData();
	static MetricsData* instance();
	static MetricsData* self;
	QString getEyeDirection();
	void setEyeDirection(QString dir);

	int getFiducialType();
	void setFiducialType(int type);
	QString getMTFImgsDir();
	void setMTFImgsDir(QString direct);
	QString getRecipeSeqDir();
	void setRecipeSeqDir(QString path);
	void setEyeboxCount(int count);
	int getEyeboxCount();
	void setSequenceId(QString uuid);
	QString getSequenceId();

	void setCurDpaColor(DPACOLOR color);
	DPACOLOR getCurDpaColor();
	QString getCurDpaColorStr();
	int getFiducialCount();
	void setFiducialCount(int val);

	QString getMTFRecipeName();
	void setMTFRecipeName(QString val);

	QString getDutBarCode();
	void setDutBarCode(QString val);

	qint64 getMTFStartTime();
	void setMTFStartTime(qint64 val);

	QString getIQRecipeName();
    void setIQRecipeName(QString val);

	QString getIQRecipeSeqDir();
    void setIQRecipeSeqDir(QString path);

	QString getFiducialDir();
    void setFiducialDir(QString path);

	QString getCsvDir();
    void setCsvDir(QString path);

	Result writeConfigCsv(QString message);
    Result writeSetupReferenceCsv(QString message, bool isHeader = false);
    Result writeImageInfoCsv(QString message);
    Result writeTempCsv(QString message);

    inline QString getDate();

  private:
	QString getConfigCsvFileName();
    QString getSetupReferenceFileName();
    QString getImageInfoFileName();
    QString getTempFileName();

    Result writeToCSV(QString filename, QString msg, QString errorMsg);
    QString getCSVFileName(QString fileName);

 public:
    // image name info
    ImageNameInfo m_imageNameInfo;

	// eye:left,right
	QMap<QString, DutControlInfo> m_dutControlInfo;

  private :
	MetricsData();
	QString eyeDirection = "Left";
	QString mtfimgsDir = "";
	int fiducialType = 1;
	QString recipeSeqDir = "d:";
	QString seqId = "";
	int eyeboxCount = 9;
	DPACOLOR curdpacolor = DPACOLOR::CLOSE;
	int fiducialCount = 2;
	QString recipeName = "";
	QString dutId = "";
    QString iqRecipeName = "";
	qint64 mtfStartTime;
    QString iqRecipeDir = "";
    QString m_fiducialDir;
	QString m_csvDir;

	const QString m_configCsvFileName = "Configuration.csv";
    const QString m_setupReferenceFileName = "Setup reference.csv";
    const QString m_imageInfoFileName = "Image info.csv";
    const QString m_tempFileName = "Temperature.csv";
};
