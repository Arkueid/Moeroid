#include "Systray.h"

#include <QApplication>
#include <QMenu>
#include <QJsonArray>
#include <QActionGroup>

#include "Process/PythonProcess.h"


Systray::Systray(): moeConfig(nullptr), menu(nullptr), descAction(nullptr)
{
}

Systray::~Systray()
{
    delete menu;
}

void Systray::initialize(MoeConfig* config, QWidget* view)
{
    moeConfig = config;

    QIcon icon(config->getString("modelDir").append("/16x16.ico"));
    setIcon(icon);
    setToolTip("Moeroid");

    menu = new QMenu();
    descAction = menu->addAction(config->getCurrentModelDesc());
    descAction->setIcon(icon);
    menu->addSeparator();
    QAction* stayOnTopAction = new QAction(tr("置顶模式"), this);
    stayOnTopAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::WindowNew));
    stayOnTopAction->setCheckable(true);
    stayOnTopAction->setChecked(moeConfig->getBoolean("stayOnTop"));
    connect(stayOnTopAction, &QAction::triggered, moeConfig, &MoeConfig::setStayOnTop);
    menu->addAction(stayOnTopAction);

    
    const QString& clan = config->getLan();

    QMenu* lanMenu = menu->addMenu(tr("语音"));
    lanMenu->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::AudioInputMicrophone));
    QActionGroup* group = new QActionGroup(this);
    group->setExclusive(true);
    
    QAction *cnAction = lanMenu->addAction(tr("CN"));
    cnAction->setCheckable(true);
    cnAction->setChecked(clan == "CN");
    QAction *jpAction = lanMenu->addAction(tr("JP"));
    jpAction->setCheckable(true);
    jpAction->setChecked(clan == "JP");
    QAction* enAction = lanMenu->addAction(tr("EN"));
    enAction->setCheckable(true);
    enAction->setChecked(clan == "EN");
    
    group->addAction(cnAction);
    group->addAction(jpAction);
    group->addAction(enAction);
    connect(cnAction, &QAction::triggered, [&]{
        moeConfig->setLan(CN);
    });
    connect(jpAction, &QAction::triggered, [&]{
        moeConfig->setLan(JP);
    });
    connect(enAction, &QAction::triggered, [&]{
        moeConfig->setLan(EN);
    });


    QMenu* modelMenu = menu->addMenu(tr("模型"));
    modelMenu->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::SyncSynchronizing));
    QActionGroup* skinGroup = new QActionGroup(this);
    skinGroup->setExclusive(true);
    const QString& currentName = config->getCurrentName();
    const int currentSkin = config->getCurrentSkin();
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
            skinAction->setCheckable(true);
            skinGroup->addAction(skinAction);
            if (key == currentName && idx == currentSkin)
            {
                skinAction->setChecked(true);
            }
            connect(skinAction, &QAction::triggered, [=](){
                moeConfig->setCurrent(key, idx);
            });
            idx++;
        }
    }

    QAction* historyAction = new QAction(tr("聊天记录"), this);
    historyAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpenRecent));
    menu->addAction(historyAction);
    connect(historyAction, &QAction::triggered, [=](){
        view->show();
    });

    QAction* aboutActon = new QAction(tr("关于"), this);
    aboutActon->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout));
    menu->addAction(aboutActon);
    connect(aboutActon, &QAction::triggered, [&](){
        about.show();
    });

    QAction* quitAction = new QAction(tr("退出"), this);
    quitAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::SystemShutdown));
    menu->addSeparator();
    menu->addAction(quitAction);

    connect(quitAction, &QAction::triggered, qApp, &QApplication::exit);
    
    connect(moeConfig, &MoeConfig::currentModelChanged, [&](){
        descAction->setText(moeConfig->getCurrentModelDesc());
    });

    setContextMenu(menu);
}
