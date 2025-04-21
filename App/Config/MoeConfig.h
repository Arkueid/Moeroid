#pragma once
#include <QJsonObject>
#include "Process/PythonProcess.h"
#include <QStringList>

class MoeConfig : public QObject
{
    Q_OBJECT

    QString jsonPath;
    QJsonObject moeJson;

public:
    MoeConfig();
    ~MoeConfig() override;
    void loadFile(const char* filePath);
    static void initializeConfig(QJsonObject& object);

    bool getBoolean(const QString& key) const;
    QString getString(const QString& key) const;
    int getInt(const QString& key) const;
    
    int getPreferenceInt(const QString& name, const QString& key);
    float getPreferenceFloat(const QString& name, const QString& key);
    QString getPreferenceString(const QString& name, const QString& key);
    void setPreferenceString(const QString& name, const QString& key, const QString& value);

    QString getModelJson(const QString& name, int skin);

    int getCurrentPreferenceInt(const QString& key);
    float getCurrentPreferenceFloat(const QString& key);
    QString getCurrentPreferenceString(const QString& key);
    void setCurrentPreferenceString(const QString& key, const QString& value);

    // current
    QString getCurrentModelJson();
    QString getCurrentModelDesc();
    QString getCurrentName();

    int getCurrentSkin();

    void setInt(const QString& key, int value);

    const QJsonObject& getData()
    {
        return moeJson;
    }

    QString getCommand();
    
    void setCurrent(const QString& name, int skin);

    void setLan(LAN lan);

    QString getLan();

    int getFps();

    QStringList getActiveExpressions();

    void setActiveExpressions(const QStringList&);

    QJsonObject getColorSchemes();

signals:
    void stayOnTopChanged(bool);
    void currentModelChanged(void);
    void lanChanged(LAN);

public slots:
    void setStayOnTop(bool value);
    void writeFile() const;
};


