#pragma once
#include <QSlider>
#include <QStyleOptionSlider>
#include <QStylePainter>
#include <QToolTip>

class MySlider : public QSlider {
public:
    MySlider(QWidget* parent = nullptr) : QSlider(parent) {}

protected:
    void paintEvent(QPaintEvent* event) override {
        QSlider::paintEvent(event);

        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QRect grooveRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
        QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        int xPos = handleRect.center().x();
        int value = this->value();

        QString text = QString::number(value);
        QPoint textPos = handleRect.center() - QPoint(fontMetrics().width(text) / 2, fontMetrics().height() + 35);
        QToolTip::showText(mapToGlobal(textPos), text, this);
    }
};