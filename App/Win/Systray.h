#pragma once

#include <QtWidgets/QSystemTrayIcon>
#include <QtGui/QAction>

#include "../Config/MoeConfig.h"

class Systray : public QSystemTrayIcon
{
    MoeConfig* moeConfig;
    QMenu* menu;
    QList<QAction*> actions;

public:
    Systray();
    ~Systray() override;
    void initialize(MoeConfig* config);

};
