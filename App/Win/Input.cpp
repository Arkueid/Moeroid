#include "Input.h"

#include <iostream>
#include <QThread>
#include "../Task/LLMTTSWorker.h"
#include <QMetaObject>
#include <QGraphicsDropShadowEffect>

#include "Output.h"

Input::Input() : backgroundColor("#FFD9E8"), shadowColor(0, 0, 0, 64), shadowWidth(2)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(ui.sendBtn, &QPushButton::clicked, this, &Input::onInput);

    connect(ui.closeBtn, &QPushButton::clicked, this, &Input::hide);

    thread = new QThread();
    thread->start();

    worker = new LLMTTSWorker();
    worker->moveToThread(thread); // worker 的信号槽会在该线程中执行

    connect(worker, &LLMTTSWorker::textReceived, this, &Input::onTextReceived);

    connect(worker, &LLMTTSWorker::textReceiveFinished, this, &Input::onTextReceiveFinished);

    output = new Output();
}

Input::~Input()
{
    thread->terminate();
    thread->deleteLater();
    worker->deleteLater();
    output->deleteLater();
}

void Input::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(shadowColor);
    painter.drawRoundedRect(0, 0, width(), height(), 10, 10);

    painter.setBrush(backgroundColor);
    painter.drawRoundedRect(shadowWidth, shadowWidth, width() - shadowWidth * 2, height() - shadowWidth * 2, 8, 8);
}

void Input::onInput() const
{
    const QString text = ui.editor->text();
    if (text.isEmpty())
    {
        return;
    }
    ui.editor->setText("思考中...");
    ui.sendBtn->setDisabled(true);
    ui.editor->setReadOnly(true);
    QMetaObject::invokeMethod(worker, "startWork", Qt::QueuedConnection, text);
}

void Input::onTextReceived(const QString &text) const
{
    output->show(text, anchor);
}

void Input::onTextReceiveFinished() const
{
    ui.editor->clear();
    ui.sendBtn->setEnabled(true);
    ui.editor->setReadOnly(false);
}

void Input::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        leftButtonPressed = true;
        const QPointF pointF = event->globalPosition();
        rightClickedX = pointF.x();
        rightClickedY = pointF.y();
        xWhenRightClicked = x();
        yWhenRightClicked = y();
    }
}

void Input::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        leftButtonPressed = false;
    }
}

void Input::mouseMoveEvent(QMouseEvent *event)
{
    if (leftButtonPressed)
    {
        const QPointF pointF = event->globalPosition();
        const int dx = pointF.x() - rightClickedX;
        const int dy = pointF.y() - rightClickedY;
        move(xWhenRightClicked + dx, yWhenRightClicked + dy);
    }
}

void Input::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
}

const LLMTTSWorker *Input::getWorker() const
{
    return worker;
}

void Input::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    ui.editor->activateWindow();
    ui.editor->setFocus();
}