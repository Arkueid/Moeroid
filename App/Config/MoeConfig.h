#pragma once
#include <QtCore/QJsonObject>

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
    QString getModelJson(const QString& name, int skin);

    int getCurrentPreferenceInt(const QString& key);
    float getCurrentPreferenceFloat(const QString& key);
    QString getCurrentModelJson();

    QString getCurrentModelDesc();

    void setInt(const QString& key, int value);

    const QJsonObject& getData()
    {
        return moeJson;
    }

    QString getCommand();
    
    void setCurrent(const QString& name, int skin);

signals:
    void stayOnTopChanged(bool);
    void currentModelChanged(void);

public slots:
    void setStayOnTop(bool value);
    void writeFile() const;
};


