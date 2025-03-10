#include "MoeConfig.h"

#include <QtCore/QFile>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtGui/QScreen>

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
    QJsonArray skins0;
    QJsonObject skin;
    skin["json"] = "liveroid/liveroiD_A-Y01/liveroiD_A-Y01.model3.json";
    skin["name"] = "双马尾";
    skins0.append(skin);
    skin["json"] = "liveroid/liveroiD_A-Y02/liveroiD_A-Y02.model3.json";
    skin["name"] = "初始";
    skins0.append(skin);
    QJsonObject liveroid;
    liveroid["skins"] = skins0;
    QJsonObject models;
    QJsonObject preferences;
    preferences["offsetX"] = 0.0f;
    preferences["offsetY"] = 0.6f;
    preferences["windowWidth"] = 400;
    preferences["windowHeight"] = 500;
    preferences["stickOffset"] = 45;
    preferences["stickRotate"] = 45.0f;
    preferences["lipsyncN"] = 3;
    liveroid["preferences"] = preferences;
    models["liveroid"] = liveroid;

    QJsonObject whitecat;
    preferences["offsetX"] = 0.0f;
    preferences["offsetY"] = 0.0f;
    preferences["stickOffset"] = 10;
    preferences["stickRotate"] = 45;
    preferences["windowHeight"] = 300;
    preferences["windowWidth"] = 300;
    preferences["lipsyncN"] = 2;
    whitecat["preferences"] = preferences;
    QJsonArray skins1;
    skin["name"] = "初始";
    skin["json"] = "whitecat/SDWhite cat free.model3.json";
    skins1.append(skin);
    whitecat["skins"] = skins1;
    models["whitecat"] = whitecat;

    QJsonObject moefans;
    preferences["offsetX"] = 0.0f;
    preferences["offsetY"] = 0.6f;
    preferences["stickOffset"] = 45;
    preferences["stickRotate"] = 45;
    preferences["windowHeight"] = 500;
    preferences["windowWidth"] = 400;
    preferences["lipsyncN"] = 2;
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

bool MoeConfig::getBoolean(const char* key) const
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

QString MoeConfig::getString(const char* key) const
{
    return moeJson[key].toString();
}

int MoeConfig::getInt(const char* key) const
{
    return moeJson[key].toInt();
}

int MoeConfig::getPreferenceInt(const char* name, const char* key)
{
    return moeJson["models"].toObject()[name].toObject()["preferences"].toObject()[key].toInt();
}

int MoeConfig::getCurrentPreferenceInt(const char* key)
{
    const std::string current = moeJson["current"].toObject()["name"].toString().toStdString();
    return getPreferenceInt(current.c_str(), key);
}

float MoeConfig::getCurrentPreferenceFloat(const char* key)
{
    const std::string name = moeJson["current"].toObject()["name"].toString().toStdString();
    return getPreferenceFloat(name.c_str(), key);
}

float MoeConfig::getPreferenceFloat(const char* name, const char* key)
{
    return moeJson["models"].toObject()[name].toObject()["preferences"].toObject()[key].toDouble();
}

QString MoeConfig::getModelJson(const char* name, int skin)
{
    return moeJson["models"].toObject()[name].toObject()["skins"].toArray()[skin].toObject()["json"].toString();
}

QString MoeConfig::getCurrentModelJson()
{
    QJsonObject current = moeJson["current"].toObject();
    const std::string name = current["name"].toString().toStdString();
    const int skin = current["skin"].toInt();
    return getModelJson(name.c_str(), skin);
}

void MoeConfig::setInt(const char* key, int value)
{
    moeJson[key] = value;
}

QString MoeConfig::getCommand()
{
    return moeJson["command"].toString();
}

void MoeConfig::setStayOnTop(bool value)
{
    moeJson["stayOnTop"] = value;
    writeFile();
    emit stayOnTopChanged(value);
}
