#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include "qcustomplot.h"
#include "squarelegenditem.h"

class CustomPlot : public QCustomPlot
{
    Q_OBJECT

public:
    explicit CustomPlot(QWidget *parent = 0) : QCustomPlot(parent)
    {
    }

    void setLegend(QVector<QPair<QString, QString>> column_names, int count_in_row = 5)
    {
        legend->setVisible(true);
        plotLayout()->addElement(1, 0, legend);
        legend->setFillOrder(QCPLayoutGrid::foColumnsFirst, true);
        legend->setWrap(count_in_row);
        legend->setFont(QFont("ubuntu", 14));
        for (int i = 0; i < column_names.size(); i++)
        {
            QCPPlottableLegendItem *oldItem = legend->itemWithPlottable(graph(i));
            if (oldItem)
                legend->removeItem(oldItem);
            SquareLegendItem *customLegendItem = new SquareLegendItem(
                legend,
                graph(i),
                graph(i)->pen().color(),
                column_names[i].first,
                column_names[i].second);
            legend->addItem(customLegendItem);
            legend->item(i)->setFont(QFont("ubuntu", 14));
        }
    }

    void setRangeWithTicker(QCPAxis *axis, double lower, double upper, double step, QString value)
    {
        axis->setRange(lower, upper);
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        for (double i = lower; i <= upper; i += step)
            textTicker->addTick(i, QString::number(i) + value);
        textTicker->addTick(upper, QString::number(upper) + value);
        axis->setTicker(textTicker);
    }

    void setRangeWithTicker(QCPAxis *axis, QCPRange range, double step, QString value)
    {
        axis->setRange(range);
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        for (double i = range.lower; i <= range.upper; i += step)
            textTicker->addTick(i, QString::number(i) + value);
        textTicker->addTick(range.upper, QString::number(range.upper) + value);
        axis->setTicker(textTicker);
    }
};

#endif