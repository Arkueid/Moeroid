#include "Output.h"
#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtGui/QMouseEvent>
#include "../Config/StickState.h"

Output::Output(): font("Arial", 12),
                  metrics(font),
                  padding(10),
                  radius(10),
                  maxWidth(306),
                  color(250, 250, 250, 255),
                  tailA(15),
                  tailB(5)
{
    setWindowFlags(Qt::SubWindow | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    connect(&timer, &QTimer::timeout, this, &Output::close);
}

Output::~Output()
= default;

void Output::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillPath(bubble, color);
    painter.fillPath(tail, color);

    painter.setPen(Qt::black);
    painter.setFont(font);
    painter.drawText(textOffsetX + padding, padding, fontRect.width(), fontRect.height(), Qt::AlignLeft | Qt::TextWordWrap, text);
}

void Output::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        close();
    }
}


void Output::show(const QString& inText, const QWidget* anchor)
{
    timer.stop();
    hide();

    const int state = GetStickState();

    this->text = inText;
    fontRect = metrics.boundingRect(0, 0, maxWidth, 0, Qt::AlignLeft | Qt::TextWordWrap, inText);

    bubble.clear();
    tail.clear();

    if (state == STICK_NONE)
    {
        QRect bubbleRect(0, 0, fontRect.width() + padding * 2, fontRect.height() + padding * 2);
        resize(bubbleRect.width(), bubbleRect.height() + tailA);
        move(anchor->x() + anchor->width() / 2 - width() / 2, anchor->y() + anchor->height() / 2);

        bubble.addRoundedRect(bubbleRect, radius, radius);
        // tail.moveTo(bubbleRect.width()/2 - tailB, bubbleRect.height());
        // tail.lineTo(bubbleRect.width()/2, bubbleRect.height() + tailA);
        // tail.lineTo(bubbleRect.width()/2 + tailB, bubbleRect.height());
        // tail.lineTo(bubbleRect.width()/2 - tailB, bubbleRect.height());

        textOffsetX = 0;
    }
    else if (state == STICK_LEFT) // left
    {
        QRect bubbleRect(tailA, 0, fontRect.width() + padding * 2, fontRect.height() + padding * 2);
        resize(bubbleRect.width() + tailA, bubbleRect.height());
        move(anchor->x() + anchor->width() / 2 + tailA, anchor->y() + anchor->height() / 2);

        bubble.addRoundedRect(bubbleRect, radius, radius);
        tail.moveTo(tailA, bubbleRect.height() / 2 - tailB);
        tail.lineTo(0, bubbleRect.height() / 2);
        tail.lineTo(tailA, bubbleRect.height() / 2 + tailB);
        tail.lineTo(tailA, bubbleRect.height() / 2 - tailB);

        textOffsetX = tailA;
    }
    else if (state == STICK_RIGHT) // right
    {
        QRect bubbleRect(0, 0, fontRect.width() + padding * 2, fontRect.height() + padding * 2);
        resize(bubbleRect.width() + tailA, bubbleRect.height());
        move(anchor->x() + anchor->width() / 2 - width() - tailA, anchor->y() + anchor->height() / 2);

        bubble.addRoundedRect(bubbleRect, radius, radius);
        tail.moveTo(bubbleRect.width(), bubbleRect.height() / 2 - tailB);
        tail.lineTo(bubbleRect.width() + tailA, bubbleRect.height() / 2);
        tail.lineTo(bubbleRect.width(), bubbleRect.height() / 2 + tailB);
        tail.lineTo(bubbleRect.width(), bubbleRect.height() / 2 - tailB);

        textOffsetX = 0;
    }

    QWidget::show();

    timer.setInterval(5000);
    timer.start(); // close after 5 seconds
}
