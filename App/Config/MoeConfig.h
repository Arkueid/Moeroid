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

    bool getBoolean(const char* key) const;
    QString getString(const char* key) const;
    int getInt(const char* key) const;
    
    int getPreferenceInt(const char* name, const char* key);
    float getPreferenceFloat(const char* name, const char* key);
    QString getModelJson(const char* name, int skin);

    int getCurrentPreferenceInt(const char* key);
    float getCurrentPreferenceFloat(const char* key);
    QString getCurrentModelJson();

    void setInt(const char* key, int value);

    QString getCommand();

signals:
    void stayOnTopChanged(bool);

public slots:
    void setStayOnTop(bool value);
    void writeFile() const;
};


