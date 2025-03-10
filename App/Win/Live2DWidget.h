#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtCore/QTimer>
#include "../Config/MoeConfig.h"

class LAppModel;
class Input;

#include "../Config/StickState.h"

class Live2DWidget : public QOpenGLWidget
{
    Q_OBJECT

    LAppModel* live2DModel;
    int iParamMouthOpenY;
    std::string modelJson;

    bool cursorEntered; // 光标进入窗口
    bool cursorOnL2D; // 光标在 live2d 角色上
    bool leftButtonPressed; // 窗口可以被拖动
    bool rightButtonPressed;

    int leftClickX; // 左键点击位置 x
    int leftClickY; // 左键点击位置 y
    int xWhenClicked; // 左键点击时窗口 x
    int yWhenClicked; // 左键点击时窗口 y
    bool windowMoved; // 窗口移动过则不响应点击事件

    int autoStickMargin; // 到屏幕边缘自动吸附的最大距离
    int stickOffset; // 微调距离
    StickState stickState;
    float stickRotate;

    int framesElapsedRightPress;
    int framesThresholdRightPress;

    Input* input;

    int screenWidth;

    QTimer configSaveTimer;

    MoeConfig* config;

public:
    Live2DWidget();
    ~Live2DWidget() override;
    void initialize(MoeConfig* config);
    StickState getStickState() const;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void timerEvent(QTimerEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void processTransparentForMouse(int mouseLocalX, int mouseLocalY);
    void processStick();
    void showInput();

public slots:
    void saveConfig() const;
};
