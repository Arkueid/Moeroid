#include "Output.h"
#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainterPath>
#include "../Config/StickState.h"

Output::Output(): font("Arial", 12),
                  metrics(font),
                  padding(10),
                  radius(10),
                  maxWidth(306),
                  color(20, 20, 20, 150)
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

    QPainterPath path;
    path.addRoundedRect(bubbleRect, radius, radius);

    painter.fillPath(path, Qt::black);

    painter.setBrush(color);
    painter.setFont(font);
    painter.drawText(padding, padding, fontRect.width(), fontRect.height(), Qt::AlignLeft | Qt::TextWordWrap, text);
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
    bubbleRect = QRect(0, 0, fontRect.width() + padding * 2, fontRect.height() + padding * 2);
    resize(bubbleRect.width(), bubbleRect.height());

    if (state == STICK_NONE)
    {
        move(anchor->x() + anchor->width() / 2 - width() / 2, anchor->y() + anchor->height() / 2);
    }
    else if (state == STICK_LEFT) // left
    {
        move(anchor->x() + anchor->width(), anchor->y() + anchor->height() / 2);
    }
    else if (state == STICK_RIGHT) // right
    {
        move(anchor->x() + anchor->width() / 2 - width(), anchor->y() + anchor->height() / 2);
    }

    QWidget::show();

    timer.setInterval(5000);
    timer.start(); // close after 5 seconds
}
