#ifndef SQUARELEGENDITEM_H
#define SQUARELEGENDITEM_H
#include "qcustomplot.h"

class SquareLegendItem : public QCPPlottableLegendItem
{
private:
    QColor brush;
    QString name;
    QString val;

public:
    SquareLegendItem(QCPLegend *parent, QCPAbstractPlottable *plottable, QColor brush, QString name, QString val)
        : QCPPlottableLegendItem(parent, plottable), brush(brush), name(name), val(val) {}

    virtual void draw(QCPPainter *painter) override
    {
        painter->setPen(QPen(Qt::black));
        painter->setBrush(QBrush(brush));
        QRect rect = this->rect();
        QPoint center = rect.center();

        int size = qMin(qMin(rect.width(), rect.height()) / 2, 56);

        painter->setFont(QFont("Consolas", qMin(size * 0.29f, 14.0f)));
        QRect squareRect(center.x() - 2 * size, center.y() - 0.8 * size, size, 0.8 * size);
        painter->drawText(QRect(center.x() - size / 2, center.y() - 0.8 * size, 400, 0.8 * size), name + "\r\n" + val, QTextOption());
        painter->drawRect(squareRect);
    }

    void setVal(QString val) { this->val = val; };
};

#endif // SQUARELEGENDITEM_H