#include "Win/Live2DWidget.h"

#include <QApplication>
#include <LAppAllocator.hpp>
#include <LAppPal.hpp>
#include <Windows.h>
#include "Config/MoeConfig.h"
#include "LipSync/LipSync.h"
#include "Process/PythonProcess.h"
#include "Win/Systray.h"
#include "Util/CubismHelper.hpp"

int main(int argc, char* argv[])
{
#ifdef _DEBUG
    SetConsoleOutputCP(65001);
#endif
    QApplication app(argc, argv);

    MoeConfig moeConfig;
    moeConfig.loadFile("../../moe.config.json");

    Systray* tray = new Systray();
    tray->initialize(&moeConfig);
    tray->show();

    PythonProcess::create(&moeConfig);

    LipSync::initialize(&moeConfig);

    CubismHelper::Initialize();
    Live2DWidget* win = new Live2DWidget();
    win->initialize(&moeConfig);
    win->show();

    QObject::connect(&moeConfig, &MoeConfig::currentModelChanged, [&]() {
        delete win;
        CubismHelper::Dispose();  // 清除之前 OpenGL 上下文

        CubismHelper::Initialize();
        win = new Live2DWidget();
        win->initialize(&moeConfig);
        win->show();
    });

    QApplication::exec();

    delete win;
    delete tray;

    LipSync::dispose();

    PythonProcess::dispose();
    return 0;
}

