#include "config.h"

Config::Config() {
    _host = std::getenv("DB_HOST");
    _port = std::getenv("DB_PORT");
    _login = std::getenv("DB_LOGIN");
    _password = std::getenv("DB_PASSWORD");
    _database = std::getenv("DB_DATABASE");
    _cache_servers = std::getenv("CACHE");
}

Config &Config::get() {
    static Config _instance;
    return _instance;
}

std::string &Config::port() {
    return _port;
}

std::string &Config::host() {
    return _host;
}

std::string &Config::login() {
    return _login;
}

std::string &Config::password() {
    return _password;
}

std::string &Config::database() {
    return _database;
}

std::string &Config::cache_servers() {
    return _cache_servers;
}
