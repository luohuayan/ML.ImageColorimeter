#pragma once
#include <QList>
#include "service/Result.h"
#include "ml.h"

struct ResultLimit{
    QString name;
    double lower;
    double upper;
};

class McUtil
{
  public:
    static Result readResultLimit(QString fileName, QMap<QString, ResultLimit> &resultLimits);
    static Result readResultFile(const QList<QString> &files, QMap<ML::EyeMode, QMap<QString, QString>> &results);
    static Result readResultFile(QString fileName, QMap<ML::EyeMode, QMap<QString, QString>> &results);

    static Result compareResult(const QMap<QString, ResultLimit> &limits, const QMap<QString, QString> &results, ML::EyeMode eyeMode);
    static QList<QString> stringSplitNonempty(const QString &data, const QString &Separator);
  private:
    static QList<QList<QString>> readFile(QString fileName, QString separator);
    static QList<QString> readFileRows(QString fileName);
    static Result analyzeResult(const QList<QList<QString>> &rowColumns, QMap<ML::EyeMode, QMap<QString, QString>> &results);
};
