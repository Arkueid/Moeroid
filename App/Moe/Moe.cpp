#include <Moe/Moe.hpp>
#include <QFile>
#include <QTime>
#include <QDateTime>

MoeMoe::MoeMoe()
{
    firstLaunchTime = QDateTime::currentMSecsSinceEpoch();
    lastShutdownTime = firstLaunchTime;
    hours = 0;
    minutes = 0;
    seconds = 0;
    milliseconds = 0;
}

static MoeMoe *moeMoe;
static long long startTime = QDateTime::currentMSecsSinceEpoch();
static long long lastUpdateTime = startTime;

void Moe::load(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists())
    {
        moeMoe = new MoeMoe;
        moeMoe->firstLaunchTime = QDateTime::currentMSecsSinceEpoch();
        file.open(QIODevice::WriteOnly);
        char *data = (char *)moeMoe;
        file.write(data, sizeof(MoeMoe));
        file.close();
    }
    else if (file.open(QIODevice::ReadOnly))
    {
        moeMoe = (MoeMoe *)malloc(sizeof(MoeMoe));
        char *data = (char *)moeMoe;
        file.read(data, sizeof(MoeMoe));
        file.close();
        moeMoe->lastShutdownTime = QDateTime::currentMSecsSinceEpoch();
    }
}

void Moe::save(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        moeMoe->lastShutdownTime = QDateTime::currentMSecsSinceEpoch();
        update();
        char *data = (char *)moeMoe;
        file.write(data, sizeof(MoeMoe));
        file.close();
    }
    free(moeMoe);
    moeMoe = nullptr;
}

MoeMoe *Moe::getMoeMoe()
{
    return moeMoe;
}

void Moe::update()
{
    const long long currentTime = QDateTime::currentMSecsSinceEpoch();
    const long long duration = currentTime - lastUpdateTime;
    const long long durationInSecs = duration / 1000;
    long long h = durationInSecs / 3600;
    long long m = durationInSecs / 60 - h * 60; 
    long long s = durationInSecs % 60;
    long long ms = duration % 1000;

    moeMoe->hours += h;
    moeMoe->minutes += m;
    moeMoe->seconds += s;
    moeMoe->milliseconds += ms;

    if (moeMoe->milliseconds >= 1000)
    {
        long long i = moeMoe->milliseconds / 1000;
        moeMoe->milliseconds -= i * 1000;
        moeMoe->seconds += i;
    }

    if (moeMoe->seconds >= 60)
    {
        long long i = moeMoe->seconds / 60;
        moeMoe->seconds -= i * 60;
        moeMoe->minutes += i;
    }

    if (moeMoe->minutes >= 60)
    {
        long long i = moeMoe->minutes / 60;
        moeMoe->minutes -= i * 60;
        moeMoe->hours += i;
    }

    lastUpdateTime = currentTime;
}