#include <Win/About.h>
#include <Moe/Moe.hpp>
#include <QDateTime>

About::About(QWidget *parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

    setWindowModality(Qt::ApplicationModal);

    updateTimer.setInterval(1000);
    updateTimer.setSingleShot(false);
    connect(&updateTimer, &QTimer::timeout, this, &About::onUpdate);
}

void About::showEvent(QShowEvent *event)
{
    onUpdate();
    updateTimer.start();
}

void About::onUpdate()
{
    Moe::update();
    MoeMoe *moe = Moe::getMoeMoe();
    ui.firstLaunchAt->setText(QDateTime::fromMSecsSinceEpoch(moe->firstLaunchTime).toString("yyyy-MM-dd HH:mm:ss"));
    ui.lastShutdownAt->setText(QDateTime::fromMSecsSinceEpoch(moe->lastShutdownTime).toString("yyyy-MM-dd HH:mm:ss"));
    ui.totalHours->setText(QString::number(moe->hours));
    ui.totalMins->setText(QString::number(moe->minutes));
    ui.totalSecs->setText(QString::number(moe->seconds));
}

void About::hideEvent(QHideEvent *event)
{
    updateTimer.stop();
}
