#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
private:
    Config();

    std::string _host;
    std::string _port;
    std::string _login;
    std::string _password;
    std::string _database;
    std::string _cache_servers;

public:
    static Config &get();

    std::string &port();

    std::string &host();

    std::string &login();

    std::string &password();

    std::string &database();

    std::string &cache_servers();

};

#endif