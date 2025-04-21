#pragma once

#include <QString>


struct MoeMoe
{
    long long firstLaunchTime;
    long long lastShutdownTime;
    unsigned long long hours;
    unsigned long long minutes;
    unsigned long long seconds;
    unsigned long long milliseconds;
    MoeMoe();
};


class Moe
{
public:
    Moe() = delete;
    static void update();
    static void load(const QString& fileName);
    static void save(const QString& fileName);
    static MoeMoe* getMoeMoe();
};