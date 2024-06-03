#ifndef COLOR_H
#define COLOR_H

#include <QColor>

// включает цвет по цветовому колесу, принимает 0-1530
inline QColor colorWheel(int color)
{
    int r, g, b;
    if (color <= 255)
    { // красный макс, зелёный растёт
        r = 255;
        g = color;
        b = 0;
    }
    else if (color > 255 && color <= 510)
    { // зелёный макс, падает красный
        r = 510 - color;
        g = 255;
        b = 0;
    }
    else if (color > 510 && color <= 765)
    { // зелёный макс, растёт синий
        r = 0;
        g = 255;
        b = color - 510;
    }
    else if (color > 765 && color <= 1020)
    { // синий макс, падает зелёный
        r = 0;
        g = 1020 - color;
        b = 255;
    }
    else if (color > 1020 && color <= 1275)
    { // синий макс, растёт красный
        r = color - 1020;
        g = 0;
        b = 255;
    }
    else if (color > 1275 && color <= 1530)
    { // красный макс, падает синий
        r = 255;
        g = 0;
        b = 1530 - color;
    }
    return QColor(r, g, b);
}

#endif