#pragma once
#include <QtGui/QMouseEvent>

#include "../ui/Input.h"
#include <QtGui/QPainter>

class Output;
class Live2DWidget;
class LLMTTSWorker;

class Input : public QWidget
{
    Ui::Input ui;

    bool rightButtonPressed;
    int xWhenRightClicked;
    int yWhenRightClicked;
    int rightClickedX;
    int rightClickedY;

    LLMTTSWorker* worker;
    QThread* thread;

    Output* output;

    QWidget* anchor;

public:
    Input();
    ~Input() override;

    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void setAnchor(QWidget* inAnchor);
signals:
    void onInput() const;

public slots:
    void onTextReceived(QString text) const;
};

inline void Input::setAnchor(QWidget* inAnchor)
{
    this->anchor = inAnchor;
}
