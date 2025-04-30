#include <fine-grained/Model.hpp>
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
#include <QDir>
#include <QJsonArray>

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
                               config(nullptr)
{

    setWindowTitle("❤Moe Moe Kyun~❤");

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

    live2DModel = new Model();
    // restore preferences
    resize(config->getCurrentPreferenceInt("windowWidth"), config->getCurrentPreferenceInt("windowHeight"));
    stickRotate = config->getCurrentPreferenceFloat("stickRotate");
    stickOffset = config->getCurrentPreferenceInt("stickOffset");
    live2DModel->SetOffset(config->getCurrentPreferenceFloat("offsetX"), config->getCurrentPreferenceFloat("offsetY"));
    live2DModel->SetScale(config->getCurrentPreferenceFloat("scale"));

    modelName = config->getCurrentName();

    connect(&configSaveTimer, &QTimer::timeout, this, &Live2DWidget::saveConfig);
}

const LLMTTSWorker *Live2DWidget::getWorker() const
{
    return input->getWorker();
}

typedef bool(__stdcall *SwapInterval)(int);

void Live2DWidget::initializeGL()
{
    gladLoadGL();

    ((SwapInterval)wglGetProcAddress("wglSwapIntervalEXT"))(1);
    // load model assets
    const std::string &path = config->getCurrentModelJson().toStdString();
    live2DModel->LoadModelJson(path.c_str());

    loadExraMotions();

    const Csm::CubismId *handle = Csm::CubismFramework::GetIdManager()->GetId("ParamMouthOpenY");
    iParamMouthOpenY = live2DModel->GetModel()->GetParameterIndex(handle);

    activeExpressions = config->getActiveExpressions();

    const QJsonObject expColorSchemes = config->getColorSchemes();
    for (const QString &exp : activeExpressions)
    {
        live2DModel->AddExpression(exp.toStdString().c_str());
        if (!expColorSchemes.isEmpty())
        {
            const QJsonArray expColorScheme = expColorSchemes[exp].toArray();
            if (expColorScheme.size() > 0)
            {
                setColorScheme(expColorScheme);
            }
        }
    }

    menu = new QMenu(this);
    QAction *action = menu->addAction(tr("重置参数"));
    connect(action, &QAction::triggered, [&]()
            { live2DModel->ResetAllParameters(); });
    action = menu->addAction(tr("重置姿势"));
    connect(action, &QAction::triggered, [&]()
            { live2DModel->ResetPose(); });
    expMenu = menu->addMenu(tr("表情"));
    expMenu->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::SyncSynchronizing));
    void *x[2] = {expMenu, &activeExpressions};
    live2DModel->GetExpressions(x,
                                [](void *collector, const char *expId, const char *file)
                                {
                                    QMenu *expMenu = (QMenu *)((void **)collector)[0];
                                    QAction *action = expMenu->addAction(expId);
                                    action->setCheckable(true);
                                    QStringList *list = (QStringList *)(((void **)collector)[1]);
                                    if (list->contains(expId))
                                    {
                                        action->setChecked(true);
                                    }
                                });
    action = expMenu->addAction(tr("重置"));
    action->setData("reset");
    connect(expMenu, &QMenu::triggered, this, &Live2DWidget::onExpMenuTriggered);

    processStick();

    live2DModel->CreateRenderer();

    lastUpdateTime = QDateTime::currentMSecsSinceEpoch();

    startTimer(1000 / config->getFps());
}

void Live2DWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    live2DModel->Resize(w, h);
}

void Live2DWidget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    long long currentTime = QDateTime::currentMSecsSinceEpoch();
    float deltaTime = float(((double)QDateTime::currentMSecsSinceEpoch() - (double)lastUpdateTime) / 1000.0);
    lastUpdateTime = currentTime;

    bool motionUpdated = false;
    live2DModel->LoadParameters();
    if (!live2DModel->IsMotionFinished())
    {
        motionUpdated = live2DModel->UpdateMotion(deltaTime);
    }

    live2DModel->SetParameterValue(iParamMouthOpenY, LipSync::getMouthOpenY());

    live2DModel->SaveParameters();

    if (!motionUpdated)
    {
        live2DModel->UpdateBlink(deltaTime);
    }
    live2DModel->UpdateExpression(deltaTime);

    live2DModel->UpdateDrag(deltaTime);

    live2DModel->UpdateBreath(deltaTime);

    live2DModel->UpdatePhysics(deltaTime);

    live2DModel->UpdatePose(deltaTime);
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

#ifdef _DEBUG
            live2DModel->StartRandomMotion("drag", 3, this, [](ACubismMotion *motion)
                                           { qDebug() << "drag motion started: " << motion->no; });
#else
            live2DModel->StartRandomMotion("drag", 3);
#endif
        }
        else // 窗口未移动，则响应鼠标点击事件
        {
#ifdef _DEBUG
            live2DModel->StartRandomMotion("touch", 3, this, [](ACubismMotion *motion)
                                           { qDebug() << "click motion started: " << motion->no; });
#else
            live2DModel->StartRandomMotion("touch", 3);
#endif
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

    const float halfWW = wWidth / 2;
    const float halfWH = wHeight / 2;

    // 窗口中心全局坐标
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
            fty = (my - tcy) / max(abs(screenHeight - tcy), 1.f) * halfWW + halfWW;
        }
    }
    else
    {
        if (my > tcy) // 第四象限
        {
            ftx = (mx - tcx) / max(screenWidth - tcx, 1.f) * halfWW + halfWW;
            fty = (my - tcy) / max(abs(screenHeight - tcy), 1.f) * halfWH + halfWH;
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

void Live2DWidget::loadExraMotions()
{
    const QString path = config->getString("modelDir");
    QDir dir(path + "/public_motions");
    int touchIndex = 0;
    int dragIndex = 0;
    for (const QString &file : dir.entryList(QDir::Files)) // 加载公共动作
    {
        if (file.endsWith(".motion3.json"))
        {
            if (file.startsWith("touch"))
            {
                const std::string mtnPath = dir.absoluteFilePath(file).toStdString();
                live2DModel->LoadExtraMotion("touch", touchIndex++, mtnPath.c_str());
            }
            else if (file.startsWith("drag"))
            {
                const std::string mtnPath = dir.absoluteFilePath(file).toStdString();
                live2DModel->LoadExtraMotion("drag", dragIndex++, mtnPath.c_str());
            }
        }
    }
}

void Live2DWidget::setColorScheme(const QJsonArray &expColorScheme)
{
    const int size = expColorScheme.size();
    for (int i = 0; i < size; i++)
    {
        const QJsonObject partColor = expColorScheme[i].toObject();
        const QJsonArray colors = partColor["color"].toArray();
        float r = colors[0].toDouble();
        float g = colors[1].toDouble();
        float b = colors[2].toDouble();
        if (partColor.contains("partIndex"))
        {
            const int partIndex = partColor["partIndex"].toInt();
            live2DModel->SetPartMultiplyColor(partIndex, r, g, b, 1.0f);
        }
        else if (partColor.contains("drawableIndex"))
        {
            const int drawableIndex = partColor["drawableIndex"].toInt();
            live2DModel->SetDrawableMultiplyColor(drawableIndex, r, g, b, 1.0f);
        }
    }
}

void Live2DWidget::clearColorScheme(const QJsonArray &expColorScheme)
{
    for (int i = 0; i < expColorScheme.size(); i++)
    {
        const QJsonObject partColor = expColorScheme[i].toObject();
        if (partColor.contains("partIndex"))
        {
            const int partIndex = partColor["partIndex"].toInt();
            live2DModel->SetPartMultiplyColor(partIndex, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        else if (partColor.contains("drawableIndex"))
        {
            const int drawableIndex = partColor["drawableIndex"].toInt();
            live2DModel->SetDrawableMultiplyColor(drawableIndex, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
}

void Live2DWidget::onExpMenuTriggered(QAction *action)
{
    const QString s = action->data().toString();
    const std::string expId = action->text().toStdString();
    if (s == "reset")
    {
        expMenu->disconnect();
        for (auto &action : expMenu->actions()) // 重置菜单
        {
            action->setChecked(false);
        }
        live2DModel->ResetExpressions();
        activeExpressions.clear();
        connect(expMenu, &QMenu::triggered, this, &Live2DWidget::onExpMenuTriggered);
    }
    else if (!action->isChecked())
    {
        live2DModel->RemoveExpression(expId.c_str());
        activeExpressions.removeAll(expId.c_str());
        // qDebug() << "reset expression";

        const QJsonObject expColorSchemes = config->getColorSchemes();
        if (!expColorSchemes.isEmpty())
        {
            const QJsonArray expColorScheme = expColorSchemes[expId.c_str()].toArray();
            if (expColorScheme.size() > 0)
            {
                clearColorScheme(expColorScheme);
            }
        }
    }
    else
    {
        live2DModel->AddExpression(expId.c_str());
        activeExpressions.append(expId.c_str());

        const QJsonObject expColorSchemes = config->getColorSchemes();
        if (!expColorSchemes.isEmpty())
        {
            const QJsonArray expColorScheme = expColorSchemes[expId.c_str()].toArray();
            if (expColorScheme.size() > 0)
            {
                setColorScheme(expColorScheme);
            }
        }
    }
    configSaveTimer.start();
}

void Live2DWidget::saveConfig() const
{
    config->setInt("windowX", x());
    config->setInt("windowY", y());
    config->setActiveExpressions(modelName, activeExpressions);
    config->writeFile();
    Info("config saved.");
}
