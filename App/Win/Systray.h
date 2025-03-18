#pragma once

#include <QSystemTrayIcon>

#include "../Config/MoeConfig.h"

class QAction;

class Systray : public QSystemTrayIcon
{
    Q_OBJECT

    MoeConfig* moeConfig;
    QMenu* menu;
    QAction* descAction;
    QWidget* history;

public:
    Systray();
    ~Systray() override;
    void initialize(MoeConfig* config, QWidget* view);
};
