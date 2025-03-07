#pragma once
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>

class Output : public QWidget
{
    QString text;
    QTimer timer;
    QFont font;
    QFontMetrics metrics;
    QRect fontRect;
    QRect bubbleRect;

    int padding;
    int radius;

    int maxWidth;

    QColor color;

public:
    Output();
    ~Output() override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void show(const QString inText, const QWidget* anchor);
};
