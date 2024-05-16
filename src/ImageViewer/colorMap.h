#ifndef COLORMAP_H
#define COLORMAP_H
#include <QWidget>
class ColorMap : public QWidget
{
    Q_OBJECT
public:
    explicit ColorMap(QWidget* parent = nullptr);
    QColor getColor(double value);
protected:
    virtual void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
signals:

public slots:
    void setMinMax(double min, double max);
private:
    QVector<QColor> colors;
    double min = 0;
    double max = 100;
    double m_width;
    double m_height;
};

#endif
