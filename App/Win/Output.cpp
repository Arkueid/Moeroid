#include "Output.h"
#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include "../Config/StickState.h"

#include <QGraphicsDropShadowEffect>

Output::Output() : font("微软雅黑", 12),
                   metrics(font),
                   padding(10),
                   radius(10),
                   maxWidth(306),
                   color(250, 250, 250, 255),
                   shadowColor(0, 0, 0, 64),
                   tailA(15),
                   tailB(5),
                   shadowWidth(1)
{
    setAttribute(Qt::WA_TranslucentBackground);

    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    connect(&timer, &QTimer::timeout, this, &Output::close);
}

Output::~Output() = default;

void Output::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillPath(shadowBubble, shadowColor);
    painter.fillPath(shadowTail, shadowColor);
    painter.fillPath(bubble, color);
    painter.fillPath(tail, color);

    painter.setPen(Qt::black);
    painter.setFont(font);
    painter.drawText(textOffsetX + padding, padding, fontRect.width(), fontRect.height(), Qt::AlignLeft | Qt::TextWordWrap, text);
}

void Output::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        close();
    }
}

void Output::show(const QString &inText, const QWidget *anchor)
{
    timer.stop();
    hide();

    const int state = GetStickState();

    this->text = inText;
    fontRect = metrics.boundingRect(0, 0, maxWidth, 0, Qt::AlignLeft | Qt::TextWordWrap, inText);

    bubble.clear();
    tail.clear();
    shadowBubble.clear();
    shadowTail.clear();

    const int marginTotal = shadowWidth * 2;
    const int repairRadius = radius + 2;

    if (state == STICK_NONE)
    {
        QRect bubbleRect(shadowWidth, shadowWidth, fontRect.width() + padding * 2, fontRect.height() + padding * 2);
        resize(marginTotal + bubbleRect.width(), bubbleRect.height() + tailA + marginTotal);
        move(anchor->x() + anchor->width() / 2 - width() / 2, anchor->y() - height());

        bubble.addRoundedRect(bubbleRect, radius, radius);
        tail.moveTo(shadowWidth + bubbleRect.width() / 2 - tailB, shadowWidth + bubbleRect.height());
        tail.lineTo(shadowWidth + bubbleRect.width() / 2, shadowWidth + bubbleRect.height() + tailA);
        tail.lineTo(shadowWidth + bubbleRect.width() / 2 + shadowWidth + tailB, bubbleRect.height());
        tail.lineTo(shadowWidth + bubbleRect.width() / 2 - shadowWidth + tailB, bubbleRect.height());

        QRect sbr(0, 0, bubbleRect.width() + marginTotal, bubbleRect.height() + marginTotal);
        shadowBubble.addRoundedRect(sbr, repairRadius, repairRadius);
        shadowTail.moveTo(sbr.width() / 2 - tailB, sbr.height());
        shadowTail.lineTo(sbr.width() / 2, sbr.height() + tailA);
        shadowTail.lineTo(sbr.width() / 2 + tailB, sbr.height());
        shadowTail.lineTo(sbr.width() / 2 + tailB, sbr.height());

        textOffsetX = shadowWidth;
    }
    else if (state == STICK_LEFT) // left
    {
        QRect bubbleRect(shadowWidth + tailA, shadowWidth, fontRect.width() + padding * 2, fontRect.height() + padding * 2);
        resize(bubbleRect.width() + tailA + marginTotal, bubbleRect.height() + marginTotal);
        move(anchor->x() + anchor->width() * 2 / 3 + tailA, anchor->y() + anchor->height() / 3);

        bubble.addRoundedRect(bubbleRect, radius, radius);
        tail.moveTo(shadowWidth + tailA, shadowWidth + bubbleRect.height() / 2 - tailB);
        tail.lineTo(shadowWidth, shadowWidth + bubbleRect.height() / 2);
        tail.lineTo(shadowWidth + tailA, shadowWidth + bubbleRect.height() / 2 + tailB);
        tail.lineTo(shadowWidth + tailA, shadowWidth + bubbleRect.height() / 2 - tailB);

        QRect sbr(tailA, 0, bubbleRect.width() + marginTotal, bubbleRect.height() + marginTotal);
        shadowBubble.addRoundedRect(sbr, repairRadius, repairRadius);
        shadowTail.moveTo(tailA, sbr.height() / 2 - tailB);
        shadowTail.lineTo(0, sbr.height() / 2);
        shadowTail.lineTo(tailA, sbr.height() / 2 + tailB);
        shadowTail.lineTo(tailA, sbr.height() / 2 - tailB);

        textOffsetX = tailA + shadowWidth;
    }
    else if (state == STICK_RIGHT) // right
    {
        QRect bubbleRect(shadowWidth, shadowWidth, fontRect.width() + padding * 2, fontRect.height() + padding * 2);
        resize(bubbleRect.width() + tailA + marginTotal, bubbleRect.height() + marginTotal);
        move(anchor->x() + anchor->width() / 3 - width() - tailA, anchor->y() + anchor->height() / 3);

        bubble.addRoundedRect(bubbleRect, radius, radius);
        tail.moveTo(shadowWidth + bubbleRect.width(), shadowWidth + bubbleRect.height() / 2 - tailB);
        tail.lineTo(shadowWidth + bubbleRect.width() + tailA, shadowWidth + bubbleRect.height() / 2);
        tail.lineTo(shadowWidth + bubbleRect.width(), shadowWidth + bubbleRect.height() / 2 + tailB);
        tail.lineTo(shadowWidth + bubbleRect.width(), shadowWidth + bubbleRect.height() / 2 - tailB);

        QRect sbr(0, 0, bubbleRect.width() + marginTotal, bubbleRect.height() + marginTotal);
        shadowBubble.addRoundedRect(sbr, repairRadius, repairRadius);
        shadowTail.moveTo(sbr.width(), sbr.height() / 2 - tailB);
        shadowTail.lineTo(sbr.width() + tailA, sbr.height() / 2);
        shadowTail.lineTo(sbr.width(), sbr.height() / 2 + tailB);
        shadowTail.lineTo(sbr.width(), sbr.height() / 2 - tailB);

        textOffsetX = shadowWidth;
    }

    QWidget::show();

    timer.setInterval(5000);
    timer.start(); // close after 5 seconds
}
