#include "metricsdata.h"
#include <QFile>

MetricsData* MetricsData::self = nullptr;


QString MetricsData::getEyeDirection()
{
	return eyeDirection;
}

void MetricsData::setEyeDirection(QString dir)
{
	eyeDirection = dir;
}

QString MetricsData::getDutBarCode()
{
	return dutId;
}

void MetricsData::setDutBarCode(QString val)
{
	dutId = val;
}

qint64 MetricsData::getMTFStartTime()
{
	return mtfStartTime;
}

void MetricsData::setMTFStartTime(qint64 val)
{
	mtfStartTime = val;
}

QString MetricsData::getIQRecipeName()
{
    return iqRecipeName;
}

void MetricsData::setIQRecipeName(QString val)
{
    iqRecipeName = val;
}

QString MetricsData::getIQRecipeSeqDir()
{
    return iqRecipeDir;
}

void MetricsData::setIQRecipeSeqDir(QString path)
{
    iqRecipeDir = path;
}

QString MetricsData::getFiducialDir()
{
    return m_fiducialDir;
}

void MetricsData::setFiducialDir(QString path)
{
    m_fiducialDir = path;
}

QString MetricsData::getCsvDir()
{
    return m_csvDir;
}

void MetricsData::setCsvDir(QString path)
{
	m_csvDir = path;
}

Result MetricsData::writeToCSV(QString filename, QString msg, QString errorMsg)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << msg << endl;
        return Result();
    }
    return Result(false, errorMsg.toStdString());
}

Result MetricsData::writeConfigCsv(QString message)
{
    QString fileName = getConfigCsvFileName();

    return writeToCSV(fileName, message, "Write config csv failed.");
}

Result MetricsData::writeSetupReferenceCsv(QString message, bool isHeader)
{
    if (!isHeader){
        message = getDutBarCode() + "," + message;
	}

    QString fileName = getSetupReferenceFileName();

	return writeToCSV(fileName, message, "Write setup reference failed.");
}

Result MetricsData::writeImageInfoCsv(QString message)
{
    QString fileName = getImageInfoFileName();

	return writeToCSV(fileName, message, "Write image info csv failed.");
}

Result MetricsData::writeTempCsv(QString message)
{
    QString fileName = getTempFileName();

	return writeToCSV(fileName, message, "Write temp info csv failed.");
}

QString MetricsData::getCSVFileName(QString fileName)
{
    return getCsvDir() + "\\" + fileName;
}

QString MetricsData::getConfigCsvFileName()
{
    return getCSVFileName(m_configCsvFileName);
}

QString MetricsData::getSetupReferenceFileName()
{
    return getCSVFileName(m_setupReferenceFileName);
}

QString MetricsData::getImageInfoFileName()
{
    return getCSVFileName(m_imageInfoFileName);
}

QString MetricsData::getTempFileName()
{
    return getCSVFileName(m_tempFileName);
}

MetricsData::MetricsData()
	: QObject()
{
}

MetricsData::~MetricsData()
{
	self = nullptr;
}

MetricsData* MetricsData::instance()
{
	if (self == nullptr) {
		self = new MetricsData();
	}
	return self;
}

int MetricsData::getFiducialType() 
{
	return fiducialType;
}
void MetricsData::setFiducialType(int type) 
{
	fiducialType = type;
}

QString MetricsData::getMTFImgsDir() 
{
	return mtfimgsDir;
}

void MetricsData::setMTFImgsDir(QString direct) 
{
	mtfimgsDir = direct;
}

QString MetricsData::getRecipeSeqDir()
{
	return recipeSeqDir;
}

void MetricsData::setRecipeSeqDir(QString path)
{
	recipeSeqDir = path;
}

void MetricsData::setEyeboxCount(int count)
{
	eyeboxCount = count;
}

int MetricsData::getEyeboxCount()
{
	return eyeboxCount;
}

void MetricsData::setSequenceId(QString uuid)
{
	seqId = uuid;
}

QString MetricsData::getSequenceId()
{
	return seqId;
}
void MetricsData::setCurDpaColor(DPACOLOR color) 
{
	curdpacolor = color;
}

QString MetricsData::getCurDpaColorStr() 
{
	QString strColor = "w";
	switch (curdpacolor)
	{
	case 0:
		strColor = "r";
		break;
	case 1:
		strColor = "g";
		break;
	case 2:
		strColor = "b";
		break;
	case 3:
		strColor = "w";
		break;
	}
	return strColor;
}
DPACOLOR MetricsData::getCurDpaColor() 
{
	return curdpacolor;
}

int MetricsData::getFiducialCount()
{
	return fiducialCount;
}

void MetricsData::setFiducialCount(int val)
{
	fiducialCount = val;
}

QString MetricsData::getMTFRecipeName()
{
	return recipeName;
}

void MetricsData::setMTFRecipeName(QString val)
{
	recipeName = val;
}

inline QString MetricsData::getDate()
{
    return QDateTime().currentDateTime().toString("yyyyMMddTHHmmss");
}
