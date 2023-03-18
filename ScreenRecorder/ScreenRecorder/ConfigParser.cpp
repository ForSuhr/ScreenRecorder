#include "ConfigParser.h"


ConfigParser::ConfigParser()
{
    settings = new QSettings(CONFIG_PATH, QSettings::IniFormat);
}

ConfigParser::~ConfigParser()
{
    delete settings;
}

ConfigParser& ConfigParser::getInstance()
{
    static ConfigParser instance;
    return instance;
}

string ConfigParser::getConfig(string key)
{
    string value = settings->value(QString::fromStdString(key)).toString().toStdString();
    return value;
}

void ConfigParser::setConfig(string key, string value)
{
    settings->setValue(QString::fromStdString(key), QString::fromStdString(value));
}

void ConfigParser::checkConfig()
{
    QFile file(CONFIG_PATH);
    if (!file.exists()) {
        string userDir = getenv("USERPROFILE");
        QString outputDir = QString::fromStdString(userDir + R"(\Videos\Captures\)");
        QDir f(outputDir);
        if (!f.exists()) QDir().mkpath(outputDir);
        setConfig("General/OutputPath", outputDir.toStdString());
    }
}


