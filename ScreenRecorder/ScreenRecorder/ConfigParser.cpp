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
    static ConfigParser m_instance;
    return m_instance;
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
