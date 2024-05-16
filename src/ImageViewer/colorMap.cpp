#include "colormap.h"
#include <QPainter>
#include <QDebug>
ColorMap::ColorMap(QWidget* parent) : QWidget(parent)
{ 
}

void ColorMap::paintEvent(QPaintEvent* event)
{
    colors.clear();
    int parentHeight = parentWidget()->height();
    float colorBarLength = parentHeight;
    float tempLength = colorBarLength / 4;
    QColor color;
    for (int i = 0; i < tempLength / 2; i++)// jet
    {
        color.setRgbF(0, 0, (tempLength / 2 + i) / tempLength);
        colors.push_back(color);
    }
    for (int i = tempLength / 2 ; i < tempLength / 2 + tempLength; i++)// jet
    {
        color.setRgbF(0, (i - tempLength / 2) / tempLength, 1);
        colors.push_back(color);
    }
    for (int i = tempLength / 2 + tempLength ; i < tempLength / 2 + 2 * tempLength; i++)// jet
    {
        color.setRgbF((i - tempLength - tempLength / 2) / tempLength, 1, (tempLength * 2 + tempLength / 2 - i) / tempLength);
        colors.push_back(color);
    }
    for (int i = tempLength / 2 + 2 * tempLength ; i < tempLength / 2 + 3 * tempLength; i++)// jet
    {
        color.setRgbF(1, (tempLength * 3 + tempLength / 2 - i) / tempLength, 0);
        colors.push_back(color);
    }
    for (int i = tempLength / 2 + 3 * tempLength ; i < colorBarLength; i++)// jet
    {
        color.setRgbF((colorBarLength - i + tempLength / 2) / (tempLength), 0, 0);
        if (color == QColor(0, 0, 0))
            qDebug() << "black color";
        colors.push_back(color);
    }
    qDebug() << "colors's num: " << colors.size();

    QPainter painter(this);
    if (height() < colorBarLength)
        return;
    for (int i = 0; i < colorBarLength; i++) {
        QRect rect(0, colorBarLength - i, 20, 1);
        painter.fillRect(rect, colors.at(i));
    }
    for (int i = 0; i < 6; i++) {
        QString strValue;
        strValue.sprintf("%.2f", min + i * (max - min) / 5);
        QFont font;
        font.setFamily("Microsoft YaHei");
        font.setPointSize(8);
        QFontMetrics fm(font);
        QRect rec = fm.boundingRect(strValue);
        int textWidth = rec.width();
        int textHeight = rec.height();
        QRect textRect(25, colorBarLength + textHeight - (colorBarLength / 5) * i, textWidth + 20, textHeight);
        painter.drawText(textRect, strValue);
    }

}


void ColorMap::setMinMax(double min, double max)
{
    this->min = min;
    this->max = max;
    update();
}

QColor ColorMap::getColor(double value)
{
    if (value > max || value < min)
        return QColor(0, 0, 0);
    int index = ((value - min) / (max - min)) * parentWidget()->height();
    return colors.at(index);
}
QSize ColorMap::sizeHint() const
{
    return QSize(50, 400); 
}

QSize ColorMap::minimumSizeHint() const
{
    return QSize(50, 400);
}