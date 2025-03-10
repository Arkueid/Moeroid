#include "Systray.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/QMenu>


Systray::Systray(): moeConfig(nullptr), menu(nullptr)
{
}

Systray::~Systray()
{
    delete menu;
}

void Systray::initialize(MoeConfig* config)
{
    moeConfig = config;

    setIcon(QIcon(config->getString("modelDir").append("/16x16.ico")));
    setToolTip("Moeroid");

    menu = new QMenu();
    QAction* stayOnTopAction = new QAction("置顶模式", this);
    stayOnTopAction->setCheckable(true);
    stayOnTopAction->setChecked(moeConfig->getBoolean("stayOnTop"));
    connect(stayOnTopAction, &QAction::triggered, moeConfig, &MoeConfig::setStayOnTop);
    menu->addAction(stayOnTopAction);

    QAction* quitAction = new QAction("退出", this);
    menu->addSeparator();
    menu->addAction(quitAction);

    connect(quitAction, &QAction::triggered, qApp, &QApplication::exit);

    setContextMenu(menu);
}
