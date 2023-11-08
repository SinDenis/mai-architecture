//
// Created by Sin Denis on 29.10.2023.
//

#ifndef HL_SERVER_USER_H
#define HL_SERVER_USER_H

#include <string>
#include <Poco/JSON/Parser.h>
#include "../common/utils/json.h"

class User {
public:
    long &id() {
        return _id;
    }

    std::string &login() {
        return _login;
    }

    std::string &password() {
        return _password;
    }

    std::string &firstName() {
        return _firstName;
    }

    std::string &lastName() {
        return _lastName;
    }

    std::string &email() {
        return _email;
    }

    Poco::JSON::Object::Ptr toJSON() {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", _id);
        if (!_login.empty()) root->set("login", _login);
        if (!_firstName.empty()) root->set("first_name", _firstName);
        if (!_lastName.empty()) root->set("last_name", _lastName);
        if (!_email.empty()) root->set("email", _email);
        return root;
    }

private:
    long _id{};
    std::string _login;
    std::string _password;
    std::string _firstName;
    std::string _lastName;
    std::string _email;
};

User fromJSON(const std::string& userJson) {
    User user;
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result = parser.parse(userJson);
    const auto& object = result.extract<Poco::JSON::Object::Ptr>();
    user.id() = getOrDefault<long>(object, "id", 0);
    user.login() = getOrDefault<std::string>(object, "login", "");
    user.password() = getOrDefault<std::string>(object, "password", "");
    user.email() = getOrDefault<std::string>(object, "email", "");
    user.firstName() = getOrDefault<std::string>(object, "first_name", "");
    user.lastName() = getOrDefault<std::string>(object, "last_name", "");
    return user;
}

#endif //HL_SERVER_USER_H
