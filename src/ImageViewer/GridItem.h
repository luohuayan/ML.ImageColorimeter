#include <QGraphicsItem>
#include <QPainter>

class GridItem : public QGraphicsItem {
public:
    GridItem(int gridXSize = 10, int gridYSize = 10, const QColor& color = Qt::lightGray, const QRectF& rect = QRectF())
        : gridXSize(gridXSize), gridYSize(gridYSize),color(color), m_boundingRect(rect) {}

    QRectF boundingRect() const override { return m_boundingRect; }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {
        QPen pen;
        pen.setColor(color);
        pen.setWidth(5);
        painter->setPen(pen);
        if (gridXSize == 0)
            return;
        qreal left = int(m_boundingRect.left()) - (int(m_boundingRect.left()) % gridXSize);
        qreal top = int(m_boundingRect.top()) - (int(m_boundingRect.top()) % gridYSize);
        QVector<QLineF> lines;
        for (qreal x = left; x < m_boundingRect.right(); x += gridXSize)
            lines.append(QLineF(x, m_boundingRect.top(), x, m_boundingRect.bottom()));
        for (qreal y = top; y < m_boundingRect.bottom(); y += gridYSize)
            lines.append(QLineF(m_boundingRect.left(), y, m_boundingRect.right(), y));
        painter->drawLines(lines.data(), lines.size());
    }

private:
    int gridXSize;
    int gridYSize;
    QColor color;
    QRectF m_boundingRect;
};
