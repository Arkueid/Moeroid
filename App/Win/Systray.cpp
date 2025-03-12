#include "Systray.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/QMenu>
#include <QtCore/QJsonArray>


Systray::Systray(): moeConfig(nullptr), menu(nullptr), descAction(nullptr)
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
    descAction = menu->addAction(config->getCurrentModelDesc());
    descAction->setEnabled(false);
    QAction* stayOnTopAction = new QAction("置顶模式", this);
    stayOnTopAction->setCheckable(true);
    stayOnTopAction->setChecked(moeConfig->getBoolean("stayOnTop"));
    connect(stayOnTopAction, &QAction::triggered, moeConfig, &MoeConfig::setStayOnTop);
    menu->addAction(stayOnTopAction);

    QMenu* modelMenu = menu->addMenu("模型");
    const QJsonObject& models = config->getData()["models"].toObject();
    for (const QString& key: models.keys())
    {
        const QJsonArray& skins = models[key].toObject()["skins"].toArray();
        QMenu* skinMenu = modelMenu->addMenu(key);
        int idx = 0;
        for (const QJsonValueConstRef& ref: skins)
        {
            const QString& name = ref.toObject()["name"].toString();
            QAction* skinAction = skinMenu->addAction(name);
            connect(skinAction, &QAction::triggered, [=](){
                moeConfig->setCurrent(key, idx);
            });
            idx++;
        }
    }

    QAction* quitAction = new QAction("退出", this);
    menu->addSeparator();
    menu->addAction(quitAction);

    connect(quitAction, &QAction::triggered, qApp, &QApplication::exit);
    
    connect(moeConfig, &MoeConfig::currentModelChanged, [&](){
        descAction->setText(moeConfig->getCurrentModelDesc());
    });

    setContextMenu(menu);
}
