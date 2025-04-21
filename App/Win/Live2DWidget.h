#pragma once

#include <QOpenGLWidget>
#include <QTimer>
#include "../Config/MoeConfig.h"

class Model;
class Input;

#include "../Config/StickState.h"

class LLMTTSWorker;

class Live2DWidget : public QOpenGLWidget
{
    Q_OBJECT

    Model* live2DModel;
    int iParamMouthOpenY;

    bool cursorEntered; // 光标进入窗口
    bool cursorOnL2D; // 光标在 live2d 角色上
    bool leftButtonPressed; // 窗口可以被拖动
    bool rightButtonPressed;

    int leftClickX; // 左键点击位置 x
    int leftClickY; // 左键点击位置 y
    int xWhenClicked; // 左键点击时窗口 x
    int yWhenClicked; // 左键点击时窗口 y
    bool windowMoved; // 窗口移动过则不响应点击事件

    int rightClickX;
    int rightClickY;

    int autoStickMargin; // 到屏幕边缘自动吸附的最大距离
    int stickOffset; // 微调距离
    StickState stickState;
    float stickRotate;

    int framesElapsedRightPress;
    int framesThresholdRightPress;

    Input* input;

    int screenWidth;
    int screenHeight;

    QTimer configSaveTimer;

    MoeConfig* config;

    QMenu* menu;
    QMenu* expMenu;

    long long lastUpdateTime;

    QStringList activeExpressions;

    QString modelName;

public:
    Live2DWidget();
    ~Live2DWidget() override;
    void initialize(MoeConfig* config);
    const LLMTTSWorker* getWorker() const;

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
    void processDrag(float mx, float my);
    void processStick();
    void showInput();

    void loadExraMotions();

public slots:
    void saveConfig() const;

    void onExpMenuTriggered(QAction* action);
};
