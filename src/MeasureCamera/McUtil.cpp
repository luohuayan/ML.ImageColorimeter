#include "McUtil.h"
#include <QFile>
#include <QTextStream>

Result McUtil::readResultLimit(QString fileName, QMap<QString, ResultLimit> &resultLimits)
{
    QList<QList<QString>> rowColumns = readFile(fileName, ";");
    if (rowColumns.size() <= 1){
        return Result(false, QString("Read result limit error, file is %1, rows is %2.")
                                 .arg(fileName)
                                 .arg(rowColumns.size())
                                 .toStdString());
    }

    for (int i = 1; i < rowColumns.size(); ++i){
        QList<QString> columns = rowColumns.at(i);
        if (columns.size() < 3)
        {
            return Result(false, QString("Read result limit error, file is %1, columns is %2, columns less than 3.")
                                     .arg(fileName)
                                     .arg(columns.size())
                                     .toStdString());
        }

        ResultLimit limit;
        limit.name = columns.at(0).trimmed();
        limit.lower = columns.at(1).trimmed().toDouble();
        limit.upper = columns.at(2).trimmed().toDouble();

        resultLimits[limit.name] = limit;
    }

    return Result();
}

Result McUtil::readResultFile(const QList<QString> &files, QMap<ML::EyeMode, QMap<QString, QString>> &results)
{
    Result ret;
    for (int i = 0; i < files.size() ; ++i){
        ret = readResultFile(files.at(i), results);
        if(!ret.success){
            return ret;
        }
    }
    return Result();
}

Result McUtil::readResultFile(QString fileName, QMap<ML::EyeMode, QMap<QString, QString>> &results)
{
    QList<QList<QString>> rowColumns = readFile(fileName, ",");
    if (rowColumns.size() <= 1)
    {
        return Result(false, QString("Read result file error, file is %1, rows is %2.")
                                 .arg(fileName)
                                 .arg(rowColumns.size())
                                 .toStdString());
    }

    return analyzeResult(rowColumns, results);
}

Result McUtil::compareResult(const QMap<QString, ResultLimit> &limits, const QMap<QString, QString> &results,
                             ML::EyeMode eyeMode)
{
    QMap<QString, ResultLimit>::const_iterator limitIter;
    for (limitIter = limits.begin(); limitIter != limits.end(); ++limitIter)
    {
        QString key = limitIter.key().toLower();
        ResultLimit rLinmit = limitIter.value();

        double value = results[key].toDouble();
        if (value < rLinmit.lower || value > rLinmit.upper)
        {
            QString tips = "Left";
            if (eyeMode == ML::Right_Eye)
            {
                tips = "Right";
            }
            tips = QString("%1 eye judge result failed. %2 value is %3, limit is %4-%5")
                       .arg(tips)
                       .arg(key)
                       .arg(value, 0, 'Q', 4)
                       .arg(rLinmit.lower, 0, 'Q', 4)
                       .arg(rLinmit.upper, 0, 'Q', 4);
            return Result(false, tips.toStdString());
        }
    }
    return Result();
}

QList<QString> McUtil::stringSplitNonempty(const QString &data, const QString &Separator)
{
    QList<QString> list;
    QStringList strList = data.split(Separator);
    for (int i = 0; i < strList.size(); ++i)
    {
        QString str = strList.at(i).trimmed();
        if (!str.isEmpty())
        {
            list.push_back(str);
        }
    }
    return list;
}

QList<QList<QString>> McUtil::readFile(QString fileName, QString separator)
{
    QList<QList<QString>> rowColumns;

    QList<QString> rows = readFileRows(fileName);
    for (int i = 0; i < rows.size(); ++i)
    {
        QString row = rows.at(i);
        QList<QString> column = row.split(separator);
        rowColumns.append(column);
    }

    return rowColumns;
}

QList<QString> McUtil::readFileRows(QString fileName)
{
    QList<QString> lineDatas;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty())
            {
                lineDatas.push_back(line);
            }
        }
    }
    file.close();
    return lineDatas;
}

Result McUtil::analyzeResult(const QList<QList<QString>> &rowColumns, QMap<ML::EyeMode, QMap<QString, QString>> &results)
{
    QMap<QString, QString> data;
    for (int i = 1; i < rowColumns.size(); ++i)
    {
        QList<QString> columns = rowColumns.at(i);
        if (columns.size() < 3)
        {
            continue;
        }
        data[columns.at(1).trimmed().toLower()] = columns.at(2).trimmed();
    }

    ML::EyeMode eyeMode = ML::Left_Eye;
    if(data["eye"].compare("right", Qt::CaseInsensitive) == 0){
        eyeMode = ML::Right_Eye;
    }
    results[eyeMode] = data;
    return Result();
}
