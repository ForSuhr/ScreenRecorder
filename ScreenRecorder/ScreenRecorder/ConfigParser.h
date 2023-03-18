#pragma once
#include <map>
#include <string>
#include <vector>
#include <QSettings>
#include <QFile>
#include <QDir>

using namespace std;

#define CONFIG_PATH "./Config/config.ini"


class ConfigParser 
{
public:
    static ConfigParser& getInstance();
    string getConfig(string key);
    void setConfig(string key, string value);
    void checkConfig();

private:
    ConfigParser();
    ~ConfigParser();

    QSettings* settings;
};
