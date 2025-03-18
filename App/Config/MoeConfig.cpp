#include "MoeConfig.h"

#include <QFile>

#include <QJsonDocument>
#include <QJsonArray>
#include <QScreen>

MoeConfig::MoeConfig()
= default;

MoeConfig::~MoeConfig()
= default;

void MoeConfig::loadFile(const char* filePath)
{
    jsonPath = filePath;
    QFile file(filePath);
    if (!file.exists())
    {
        initializeConfig(moeJson);
        QJsonDocument doc;
        doc.setObject(moeJson);
        file.open(QFile::WriteOnly);
        file.write(doc.toJson());
        file.close();
        return;
    }

    file.open(QFile::ReadOnly);
    const QByteArray bytes = file.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    moeJson = doc.object();
}

void MoeConfig::initializeConfig(QJsonObject& object)
{
    object["stayOnTop"] = true;
    QJsonObject current;
    current["name"] = "猫耳幻丝";
    current["skin"] = 0;
    object["current"] = current;
    object["language"] = "CN";
    object["fps"] = 60;

    QJsonObject moefans, preferences, skin, models;
    preferences["offsetX"] = 0.0f;
    preferences["offsetY"] = 0.6f;
    preferences["stickOffset"] = 45;
    preferences["stickRotate"] = 45;
    preferences["windowHeight"] = 500;
    preferences["windowWidth"] = 400;
    preferences["lipsyncN"] = 2;
    preferences["scale"] = 1.0f;
    preferences["expression"] = "";
    moefans["preferences"] = preferences;
    QJsonArray skins2;
    skin["name"] = "初始";
    skin["json"] = "猫耳幻丝/momo.model3.json";
    skins2.append(skin);
    moefans["skins"] = skins2;
    models["猫耳幻丝"] = moefans;
    
    object["models"] = models;
    object["modelDir"] = "../../Resources";

    QRect rect = QGuiApplication::primaryScreen()->geometry();
    object["windowX"] = rect.width() / 3;
    object["windowY"] = rect.height() / 3;

    object["command"] = "../../Runtime/pythonw.exe ../../Scripts/main_subprocess.py";
}

bool MoeConfig::getBoolean(const QString& key) const
{
    return moeJson[key].toBool();
}

void MoeConfig::writeFile() const
{
    QFile file(jsonPath);
    file.open(QFile::WriteOnly);
    QJsonDocument doc(moeJson);
    file.write(doc.toJson());
    file.close();
}

QString MoeConfig::getString(const QString& key) const
{
    return moeJson[key].toString();
}

int MoeConfig::getInt(const QString& key) const
{
    return moeJson[key].toInt();
}

int MoeConfig::getPreferenceInt(const QString& name, const QString& key)
{
    return moeJson["models"].toObject()[name].toObject()["preferences"].toObject()[key].toInt();
}

int MoeConfig::getCurrentPreferenceInt(const QString& key)
{
    const QString& current = moeJson["current"].toObject()["name"].toString();
    return getPreferenceInt(current, key);
}

float MoeConfig::getCurrentPreferenceFloat(const QString& key)
{
    const QString& name = moeJson["current"].toObject()["name"].toString();
    return getPreferenceFloat(name, key);
}

QString MoeConfig::getCurrentPreferenceString(const QString &key)
{
    const QString& name = moeJson["current"].toObject()["name"].toString();
    return getPreferenceString(name, key);
}

void MoeConfig::setCurrentPreferenceString(const QString &key, const QString &value)
{
    const QString& name = moeJson["current"].toObject()["name"].toString();
    setPreferenceString(name, key, value);
}

float MoeConfig::getPreferenceFloat(const QString& name, const QString& key)
{
    return moeJson["models"].toObject()[name].toObject()["preferences"].toObject()[key].toDouble();
}

QString MoeConfig::getPreferenceString(const QString &name, const QString &key)
{
    return moeJson["models"].toObject()[name].toObject()["preferences"].toObject()[key].toString();
}

void MoeConfig::setPreferenceString(const QString &name, const QString &key, const QString &value)
{
    QJsonObject models = moeJson["models"].toObject();
    QJsonObject model = models[name].toObject();
    QJsonObject preferences = model["preferences"].toObject();
    preferences[key] = value;
    model["preferences"] = preferences;
    models[name] = model;
    moeJson["models"] = models;
}

QString MoeConfig::getModelJson(const QString& name, int skin)
{
    return getString("modelDir").append("/") + moeJson["models"].toObject()[name].toObject()["skins"].toArray()[skin].toObject()["json"].toString();
}

QString MoeConfig::getCurrentModelJson()
{
    QJsonObject current = moeJson["current"].toObject();
    const QString& name = current["name"].toString();
    const int skin = current["skin"].toInt();
    return getModelJson(name, skin);
}

QString MoeConfig::getCurrentModelDesc()
{
    const QJsonObject& obj = moeJson["current"].toObject();
    const QString& name = obj["name"].toString();
    const int skin = obj["skin"].toInt();
    return name + ":" + moeJson["models"].toObject()[name].toObject()["skins"].toArray()[skin].toObject()["name"].toString();
}

QString MoeConfig::getCurrentName()
{
    return moeJson["current"].toObject()["name"].toString();
}

int MoeConfig::getCurrentSkin()
{
    return moeJson["current"].toObject()["skin"].toInt();
}

void MoeConfig::setInt(const QString& key, int value)
{
    moeJson[key] = value;
}

QString MoeConfig::getCommand()
{
    return moeJson["command"].toString();
}

void MoeConfig::setCurrent(const QString &name, const int skin)
{
    QJsonObject obj = moeJson["current"].toObject();
    obj["name"] = name;
    obj["skin"] = skin;
    moeJson["current"] = obj;
    emit currentModelChanged();
}

void MoeConfig::setLan(LAN lan)
{
    if (lan == CN)
    {
        moeJson["language"] = "CN";
    }
    else if (lan == JP)
    {
        moeJson["language"] = "JP";
    }
    else if (lan == EN)
    {
        moeJson["language"] = "EN";
    }

    emit lanChanged(lan);
}

QString MoeConfig::getLan()
{
    return moeJson["language"].toString();
}

int MoeConfig::getFps()
{
    return moeJson["fps"].toInt();
}

void MoeConfig::setStayOnTop(bool value)
{
    moeJson["stayOnTop"] = value;
    writeFile();
    emit stayOnTopChanged(value);
}
