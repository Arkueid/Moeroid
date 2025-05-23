#pragma once
#include <QMouseEvent>

#include "ui_Input.h"
#include <QPainter>

class Output;
class Live2DWidget;
class LLMTTSWorker;

class Input : public QWidget
{
    Ui::Input ui;

    bool leftButtonPressed;
    int xWhenRightClicked;
    int yWhenRightClicked;
    int rightClickedX;
    int rightClickedY;

    LLMTTSWorker* worker;
    QThread* thread;

    Output* output;

    QWidget* anchor;

    QColor backgroundColor;
    QColor shadowColor;

    int shadowWidth;

public:
    Input();
    ~Input() override;

    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    const LLMTTSWorker* getWorker() const;
    void setAnchor(QWidget* inAnchor);
    void showEvent(QShowEvent* event) override;
signals:
    void onInput() const;

public slots:
    void onTextReceived(const QString& text) const;
    void onTextReceiveFinished() const;
};

inline void Input::setAnchor(QWidget* inAnchor)
{
    this->anchor = inAnchor;
}
