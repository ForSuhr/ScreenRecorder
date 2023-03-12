#include "ConfigParser.h"


ConfigParser::ConfigParser()
{
    m_instance = nullptr;
    settings = new QSettings(CONFIG_PATH, QSettings::IniFormat);
}

ConfigParser::~ConfigParser()
{
    delete m_instance;
    delete settings;
}

ConfigParser* ConfigParser::getInstance()
{
    if (m_instance == nullptr) {
        m_instance = new ConfigParser();
    }
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
