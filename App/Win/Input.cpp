#include "Input.h"

#include <iostream>
#include <QtCore/QThread>
#include "../Task/LLMTTSWorker.h"
#include <QtCore/QMetaObject>

#include "Output.h"

Input::Input()
{
    ui.setupUi(this);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(ui.sendBtn, &QPushButton::clicked, this, &Input::onInput);

    connect(ui.closeBtn, &QPushButton::clicked, this, &Input::close);

    thread = new QThread();
    thread->start();

    worker = new LLMTTSWorker();
    worker->moveToThread(thread);  // worker 的信号槽会在该线程中执行

    connect(worker, &LLMTTSWorker::textReceived, this, &Input::onTextReceived);

    connect(worker, &LLMTTSWorker::textReceiveFinished, this, &Input::onTextReceiveFinished);

    output = new Output();
}

Input::~Input()
{
    thread->quit();
    thread->deleteLater();
    worker->deleteLater();
    output->deleteLater();
}

void Input::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.setBrush(Qt::black);
    painter.drawRoundedRect(this->rect(), 8, 8);
}

void Input::onInput() const
{
    const QString text = ui.editor->text();
    if (text.isEmpty())
    {
        return;
    }
    ui.editor->setText("Speaking...");
    ui.sendBtn->setDisabled(true);
    ui.editor->setReadOnly(true);
    QMetaObject::invokeMethod(worker, "startWork", Qt::QueuedConnection, text);
}

void Input::onTextReceived(QString text) const
{
    output->show(text, anchor);
}

void Input::onTextReceiveFinished() const
{
    ui.editor->clear();
    ui.sendBtn->setEnabled(true);
    ui.editor->setReadOnly(false);
}

void Input::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        rightButtonPressed = true;
        const QPointF pointF = event->globalPosition();
        rightClickedX = pointF.x();
        rightClickedY = pointF.y();
        xWhenRightClicked = x();
        yWhenRightClicked = y();
    }
    else if (event->button() == Qt::LeftButton)
    {
        close();
    }
}

void Input::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        rightButtonPressed = false;
    }
}

void Input::mouseMoveEvent(QMouseEvent* event)
{
    if (rightButtonPressed)
    {
        const QPointF pointF = event->globalPosition();
        const int dx = pointF.x() - rightClickedX;
        const int dy = pointF.y() - rightClickedY;
        move(xWhenRightClicked + dx, yWhenRightClicked + dy);
    }
}

void Input::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
}
