#include <LAppModel.hpp>
#include <LAppPal.hpp>
#include <Log.hpp>
#include <iostream>
#include <Id/CubismIdManager.hpp>

#include "Live2DWidget.h"
#include "Input.h"
#include "../LipSync/LipSync.h"

#include <Windows.h>

#include <QMenu>
#include <QActionGroup>
#include <QStyle>

Live2DWidget::Live2DWidget() : live2DModel(nullptr),
                               leftButtonPressed(false),
                               rightButtonPressed(false),
                               autoStickMargin(100),
                               stickOffset(20),
                               stickState(STICK_NONE),
                               stickRotate(45),
                               framesElapsedRightPress(0),
                               framesThresholdRightPress(30),
                               leftClickX(0),
                               leftClickY(0),
                               xWhenClicked(0),
                               yWhenClicked(0),
                               windowMoved(false),
                               iParamMouthOpenY(-1),
                               cursorEntered(false),
                               cursorOnL2D(true),
                               config(nullptr),
                               playing(false),
                               framesElapsedOffsetY(0),
                               framesThresholdOffsetY(0),
                               dragDeltaY(0.08)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    input = new Input();
    input->setAnchor(this);

    const QRect &rect = QGuiApplication::primaryScreen()->geometry();
    screenWidth = rect.width();
    screenHeight = rect.height();

    configSaveTimer.setSingleShot(true);
    configSaveTimer.setInterval(2000);

    QCursor cursor;
    cursor.setShape(Qt::CursorShape::PointingHandCursor);

    setCursor(cursor);
}

Live2DWidget::~Live2DWidget()
{
    delete live2DModel;
    delete input;
    if (configSaveTimer.isActive())
    {
        configSaveTimer.stop();
    }
    else
    {
        saveConfig();
    }
}

void Live2DWidget::initialize(MoeConfig *config)
{
    this->config = config;

    framesThresholdRightPress = config->getFps() / 3;

    setWindowFlag(Qt::WindowStaysOnTopHint, config->getBoolean("stayOnTop"));
    connect(config, &MoeConfig::stayOnTopChanged, this, [&](bool on)
            {
        setWindowFlag(Qt::WindowStaysOnTopHint, on);
        show(); });

    move(config->getInt("windowX"), config->getInt("windowY"));

    live2DModel = new LAppModel();
    // restore preferences
    resize(config->getCurrentPreferenceInt("windowWidth"), config->getCurrentPreferenceInt("windowHeight"));
    stickRotate = config->getCurrentPreferenceFloat("stickRotate");
    stickOffset = config->getCurrentPreferenceInt("stickOffset");
    offsetX = config->getCurrentPreferenceFloat("offsetX");
    defaultOffsetY = config->getCurrentPreferenceFloat("offsetY");
    offsetY = defaultOffsetY;
    live2DModel->SetOffset(offsetX, offsetY);
    live2DModel->SetScale(config->getCurrentPreferenceFloat("scale"));

    framesThresholdOffsetY = config->getFps();
    offsetYStep = dragDeltaY / framesThresholdOffsetY;

    connect(&configSaveTimer, &QTimer::timeout, this, &Live2DWidget::saveConfig);
}

const LLMTTSWorker *Live2DWidget::getWorker() const
{
    return input->getWorker();
}

// typedef bool(__stdcall *SwapInterval)(int);

void Live2DWidget::initializeGL()
{
    gladLoadGL();

    // ((SwapInterval)wglGetProcAddress("wglSwapIntervalEXT"))(1);
    // load model assets
    const std::string &path = config->getCurrentModelJson().toStdString();
    live2DModel->LoadModelJson(path.c_str());
    const Csm::CubismId *handle = Csm::CubismFramework::GetIdManager()->GetId("ParamMouthOpenY");
    iParamMouthOpenY = live2DModel->GetModel()->GetParameterIndex(handle);

    QString curExp = config->getCurrentPreferenceString("expression");
    live2DModel->SetExpression(curExp.toStdString().c_str());

    menu = new QMenu(this);
    QMenu *expMenu = menu->addMenu(tr("表情"));
    QActionGroup *group = new QActionGroup(expMenu);
    group->setExclusive(true);
    void *x[3] = {expMenu, group, &curExp};
    live2DModel->GetExpressionIds(x, [](void *collector, const char *expId)
                                  {
        QMenu* expMenu = (QMenu*)((void**)collector)[0];
        QActionGroup* group = (QActionGroup*)((void**)collector)[1];
        QAction* action = expMenu->addAction(expId);
        action->setCheckable(true);
        const QString& s = (*((QString*)((void**)collector)[2]));
        if (s != "" && s == expId)
        {
            action->setChecked(true);
        }
        group->addAction(action); });
    expMenu->addAction(tr("重置   "));
    connect(expMenu, &QMenu::triggered, [&](QAction *action)
            {
        const std::string s = action->text().toStdString();
        if (s == "重置   ")
        {
            live2DModel->ResetExpression();
            config->setCurrentPreferenceString("expression", "");
        }
        else
        {
            config->setCurrentPreferenceString("expression", s.c_str());
            live2DModel->SetExpression(s.c_str());
        }
        configSaveTimer.start(); });

    processStick();

    startTimer(1000 / config->getFps());
}

void Live2DWidget::resizeGL(int w, int h)
{
    live2DModel->Resize(w, h);
}

void Live2DWidget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    live2DModel->Update();

    live2DModel->SetIndexParamValue(iParamMouthOpenY, LipSync::getMouthOpenY());

    live2DModel->Draw();
}

void Live2DWidget::timerEvent(QTimerEvent *event)
{
    const QPoint globalPosition = QCursor::pos();
    const float mx = globalPosition.x();
    const float my = globalPosition.y();
    const int mouseLocalX = mx - x();
    const int mouseLocalY = my - y();
    processTransparentForMouse(mouseLocalX, mouseLocalY);

    processDrag(mx, my);

    if (rightButtonPressed)
    {
        framesElapsedRightPress += 1;
    }

    if (playing)
    {
        if (framesElapsedOffsetY == 0.0f)
        {
            live2DModel->StartRandomMotion("drag_down", 3);
        }
        framesElapsedOffsetY += 1;
        offsetY -= offsetYStep;
        if (framesElapsedOffsetY > framesThresholdOffsetY)
        {
            framesElapsedOffsetY = 0;
            offsetY = defaultOffsetY;
            playing = false;
        }
        live2DModel->SetOffset(offsetX, offsetY);
    }


    update(); // 更新画面
}

void Live2DWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (leftButtonPressed)
    {
        const QPointF pointF = event->globalPosition();
        const int cx = pointF.x();
        const int cy = pointF.y();
        const int dx = cx - leftClickX;
        const int dy = cy - leftClickY;
        if (abs(dx) > 2 && abs(dy) > 2) // 窗口移动阈值
        {
            live2DModel->Rotate(0);

            if (stickState != STICK_NONE)
            {
                move(cx - width() / 2, cy - height() / 2);
            }
            else
            {
                move(xWhenClicked + dx, yWhenClicked + dy);
            }

            if (!windowMoved)
            {
                offsetY = defaultOffsetY + dragDeltaY;
                live2DModel->SetOffset(offsetX, offsetY);
            }

            windowMoved = true;
        }
        else
        {
            windowMoved = false;
        }
    }
}

void Live2DWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        leftButtonPressed = true;
        const QPointF point = event->globalPosition();
        leftClickX = point.x();
        leftClickY = point.y();
        xWhenClicked = x();
        yWhenClicked = y();
    }
    else if (event->button() == Qt::RightButton)
    {
        rightButtonPressed = true;
        const QPointF point = event->globalPosition();
        rightClickX = point.x();
        rightClickY = point.y();
    }
}

void Live2DWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        leftButtonPressed = false;

        if (windowMoved)
        {
            configSaveTimer.start();

            processStick();
            playing = true;
        }
        else // 窗口未移动，则响应鼠标点击事件
        {
            live2DModel->StartRandomMotion(nullptr, 3);
        }

    }
    else if (event->button() == Qt::RightButton)
    {
        rightButtonPressed = false;
        if (!windowMoved)
        {
            if (framesElapsedRightPress < framesThresholdRightPress)
            {
                // live2DModel->SetRandomExpression();
                menu->exec(event->globalPosition().toPoint());
            }
            else
            {
                showInput();
            }
        }
        framesElapsedRightPress = 0;
    }

    windowMoved = false; // 复位
}

void Live2DWidget::processTransparentForMouse(const int mouseLocalX, const int mouseLocalY)
{
    // 不能配合 enterEvent 和 leaveEvent 来判断，因为一旦窗口不响应事件后，不再起作用
    // 鼠标是否在窗口内
    cursorEntered = mouseLocalX <= width() && mouseLocalX >= 0 && mouseLocalY <= height() && mouseLocalY >= 0;

    if (!cursorEntered)
    {
        return;
    }

    // 系统屏幕缩放比例，会影响鼠标所在位置对应像素透明度的判断
    const double systemScaling = devicePixelRatio();

    unsigned char alpha;
    // 读取鼠标所在位置的像素值
    glReadPixels(static_cast<int>(mouseLocalX * systemScaling), static_cast<int>((height() - mouseLocalY) * systemScaling), 1, 1, GL_ALPHA,
                 GL_UNSIGNED_BYTE,
                 &alpha);

    const bool lastMouseOnL2D = cursorOnL2D;
    cursorOnL2D = alpha != 0;
    if (cursorOnL2D != lastMouseOnL2D) // 和之前状态不一样才切换，防止过多开销
    {
        HWND hWnd = reinterpret_cast<HWND>(this->winId());
        cursorOnL2D // 在 live2d 绘制区域则不透明，反之透明
            ? SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLongW(hWnd, GWL_EXSTYLE) & (~WS_EX_TRANSPARENT))
            : SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
    }
}

void Live2DWidget::processDrag(float mx, float my)
{
    // 面部朝向点局部坐标
    float ftx;
    float fty;

    const float wWidth = width();
    const float wHeight = height();
    // 窗口中心全局坐标
    const float halfWW = wWidth / 2;
    const float halfWH = wHeight / 2;

    // 贴右边缘时窗口中心恰好在屏幕边缘，此时若以窗口中心为分界点，则无法向右看
    float tcx = min(x() + halfWW, screenWidth - (screenWidth - x()) / 2);
    // 左边缘
    tcx = max((x() + width()) / 2, tcx);
    const float tcy = y() + halfWH;

    if (mx <= tcx)
    {
        if (my <= tcy) // 第二象限
        {
            ftx = mx / max(tcx, 1.f) * halfWW;
            fty = my / max(tcy, 1.f) * halfWH;
        }
        else // 第三象限
        {
            ftx = mx / max(tcx, 1.f) * halfWW;
            fty = (my - tcy) / max(screenHeight - tcy, 1.f) * halfWW + halfWW;
        }
    }
    else
    {
        if (my <= tcy) // 第四象限
        {
            ftx = (mx - tcx) / max(screenWidth - tcx, 1.f) * halfWW + halfWW;
            fty = (my - tcy) / max(screenHeight - tcy, 1.f) * halfWH + halfWH;
        }
        else // 第一象限
        {
            ftx = (mx - tcx) / max(screenWidth - tcx, 1.f) * halfWW + halfWW;
            fty = my / tcy * halfWH;
        }
    }

    live2DModel->Drag(ftx, fty);
}

void Live2DWidget::processStick()
{
    if (x() + width() / 2 + autoStickMargin >= screenWidth) // 右吸附
    {
        Info("stick state: right");
        move(screenWidth - width() / 2 - stickOffset, y());
        live2DModel->Rotate(stickRotate);
        stickState = STICK_RIGHT;
        SetStickState(STICK_RIGHT);
    }
    else if (x() + width() / 2 - autoStickMargin <= 0) // 左吸附
    {
        Info("stick state: left");
        move(-width() / 2 + stickOffset, y());
        live2DModel->Rotate(-stickRotate);
        stickState = STICK_LEFT;
        SetStickState(STICK_LEFT);
    }
    else
    {
        Info("stick state: none");
        live2DModel->Rotate(0);
        stickState = STICK_NONE;
        SetStickState(STICK_NONE);
    }
}

void Live2DWidget::showInput()
{
    if (stickState == STICK_NONE)
    {
        input->move(x() + width() / 2 - input->width() / 2, y() - input->height());
    }
    else if (stickState == STICK_LEFT)
    {
        input->move(0, y() - input->height());
    }
    else if (stickState == STICK_RIGHT)
    {
        input->move(screenWidth - input->width(), y() - input->height());
    }
    input->show();
}

void Live2DWidget::saveConfig() const
{
    config->setInt("windowX", x());
    config->setInt("windowY", y());
    config->writeFile();
    Info("config saved.");
}
