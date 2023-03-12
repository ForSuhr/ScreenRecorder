#pragma once
#include <map>
#include <string>
#include <vector>
#include <QSettings>

using namespace std;

#define CONFIG_PATH "./Config/config.ini"


class ConfigParser {

public:
    static ConfigParser* getInstance();
    string getConfig(string key);
    void setConfig(string key, string value);

private:
    ConfigParser();
    ~ConfigParser();

    static ConfigParser* m_instance;
    QSettings* settings;
};

