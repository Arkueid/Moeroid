#pragma once
#include <QWidget>
#include <QTimer>
#include <QPainterPath>

class Output : public QWidget
{
    QString text;
    QTimer timer;
    QFont font;
    QFontMetrics metrics;
    QRect fontRect;
    
    QPainterPath bubble;
    QPainterPath tail;
    QPainterPath shadowBubble;
    QPainterPath shadowTail;

    int padding;
    int textOffsetX;
    int radius;
    int tailB;
    int tailA;

    int maxWidth;

    int shadowWidth;

    QColor color;
    QColor shadowColor;

public:
    Output();
    ~Output() override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void show(const QString& inText, const QWidget* anchor);
};
