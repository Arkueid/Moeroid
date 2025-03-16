#pragma once
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <QtGui/QPainterPath>

class Output : public QWidget
{
    QString text;
    QTimer timer;
    QFont font;
    QFontMetrics metrics;
    QRect fontRect;
    
    QPainterPath bubble;
    QPainterPath tail;

    int padding;
    int textOffsetX;
    int radius;
    int tailB;
    int tailA;

    int maxWidth;

    QColor color;

public:
    Output();
    ~Output() override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void show(const QString& inText, const QWidget* anchor);
};
