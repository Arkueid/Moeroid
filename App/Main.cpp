#include "Win/Live2DWidget.h"

#include <QtWidgets/QApplication>
#include <LAppAllocator.hpp>
#include <LAppPal.hpp>
#include <Windows.h>
#include "Config/MoeConfig.h"
#include "LipSync/LipSync.h"
#include "Process/PythonProcess.h"
#include "Win/Systray.h"

#include <Live2DCubismCore.h>

int main(int argc, char* argv[])
{
    SetConsoleOutputCP(65001);
    
    Csm::CubismFramework::Option option;
    LAppAllocator allocator;
    option.LogFunction = LAppPal::PrintLn;
#ifdef _DEBUG
    option.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Verbose;
#else
    option.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Off;
#endif
    Csm::CubismFramework::StartUp(&allocator, &option);
    Csm::CubismFramework::Initialize();

    QApplication app(argc, argv);

    MoeConfig moeConfig;
    moeConfig.loadFile("../../moe.config.json");

    Systray* tray = new Systray();
    tray->initialize(&moeConfig);
    tray->show();
    
    PythonProcess::create(&moeConfig);

    LipSync::initialize(&moeConfig);

    Live2DWidget* win = new Live2DWidget();
    win->initialize(&moeConfig);
    win->show();

    QApplication::exec();

    delete win;
    delete tray;
    Live2D::Cubism::Framework::CubismFramework::Dispose();

    PythonProcess::dispose();

    LipSync::dispose();
    return 0;
}

