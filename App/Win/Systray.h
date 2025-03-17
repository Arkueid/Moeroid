#pragma once

#include <QSystemTrayIcon>

#include "../Config/MoeConfig.h"

class QAction;

class Systray : public QSystemTrayIcon
{
    MoeConfig* moeConfig;
    QMenu* menu;
    QAction* descAction;

public:
    Systray();
    ~Systray() override;
    void initialize(MoeConfig* config);

};
