#include "Win/Live2DWidget.h"

#include <QApplication>
#include <LAppAllocator.hpp>
#include <LAppPal.hpp>
#include <Windows.h>
#include <Win/HistoryView.h>

#include "Config/MoeConfig.h"
#include "LipSync/LipSync.h"
#include "Process/PythonProcess.h"
#include "Sqlite/SQLite.h"
#include "Win/Systray.h"
#include "Util/CubismHelper.hpp"
#include "Task/LLMTTSWorker.h"

#include <QTranslator>
#include <QSharedMemory>
#include <QMessageBox>
#include <Log.hpp>

int main(int argc, char *argv[])
{
#ifdef _DEBUG
    SetConsoleOutputCP(65001);
#endif

    QApplication app(argc, argv);

    app.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    QString locale = QLocale::system().name();

    Info("locale: %s\n", QString(":/i18n/moe_%1").arg(locale).toStdString().c_str());

    if (translator.load(QString(":/i18n/moe_%1").arg(locale)))
    {
        app.installTranslator(&translator);
    }
    else
    {
        if (translator.load(":/i18n/moe_zh_CN"))
        {
            app.installTranslator(&translator);
        }
    }

    // ensure single process
    QSharedMemory singleton("Moeroid");
    if (!singleton.create(1))
    {
#ifdef _DEBUG
        Info("Another instance of Moeroid is running.");
#else
        QMessageBox::information(nullptr, "Moeroid", QObject::tr("Moeroid is running. >_<"));
#endif
        app.exit(-1);
        return -1;
    }

    MoeConfig moeConfig;
    moeConfig.loadFile("../../moe.config.json");

    SQLite::initialize(&moeConfig);

    PythonProcess::create(&moeConfig);

    LipSync::initialize(&moeConfig);

    CubismHelper::Initialize();

    HistoryView view(moeConfig.getString("dataDir").append("/Audio"));

    Systray *tray = new Systray();
    tray->initialize(&moeConfig, &view);
    tray->show();

    Live2DWidget *win = new Live2DWidget();
    win->initialize(&moeConfig);
    win->show();

    QObject::connect(win->getWorker(), &LLMTTSWorker::textReceived, &view, &HistoryView::onMsgReceived);
    QObject::connect(win->getWorker(), &LLMTTSWorker::textReceiveFinished, &view, &HistoryView::onMsgReceived);

    QObject::connect(&moeConfig, &MoeConfig::currentModelChanged, [&]()
                     {
        delete win;
        CubismHelper::Dispose();  // 清除之前 OpenGL 上下文

        CubismHelper::Initialize();
        win = new Live2DWidget();
        win->initialize(&moeConfig);
        win->show(); });

    QApplication::exec();

    delete win;
    delete tray;

    LipSync::dispose();

    SQLite::dispose();

    PythonProcess::dispose();
    return 0;
}
