#include "Systray.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/QMenu>


Systray::Systray(): moeConfig(nullptr), menu(nullptr)
{
}

Systray::~Systray()
{
    delete menu;
    for (QAction* action : actions)
    {
        delete action;
    }
}

void Systray::initialize(MoeConfig* config)
{
    moeConfig = config;

    setIcon(QIcon(config->getString("modelDir").append("/16x16.ico")));
    setToolTip("Moeroid");

    menu = new QMenu();
    QAction* stayOnTopAction = new QAction("置顶模式");
    stayOnTopAction->setCheckable(true);
    stayOnTopAction->setChecked(moeConfig->getBoolean("stayOnTop"));
    connect(stayOnTopAction, &QAction::triggered, moeConfig, &MoeConfig::setStayOnTop);
    actions.append(stayOnTopAction);

    QAction* quitAction = new QAction("退出");
    menu->addSeparator();
    actions.append(quitAction);

    connect(quitAction, &QAction::triggered, qApp, &QApplication::exit);

    menu->addActions(actions);
    setContextMenu(menu);
}
