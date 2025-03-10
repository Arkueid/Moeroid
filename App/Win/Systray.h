#pragma once

#include <QtWidgets/QSystemTrayIcon>

#include "../Config/MoeConfig.h"

class Systray : public QSystemTrayIcon
{
    MoeConfig* moeConfig;
    QMenu* menu;

public:
    Systray();
    ~Systray() override;
    void initialize(MoeConfig* config);

};
