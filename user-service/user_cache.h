//
// Created by Sin Denis on 31.10.2023.
//

#ifndef HL_SERVER_USER_CACHE_H
#define HL_SERVER_USER_CACHE_H

#include "user.h"
#include <Poco/JSON/Parser.h>
#include "../common/database/cache.h"

namespace userCache {


    std::optional<User> getFromCacheById(long id) {
        try {
            std::string result;
            if (database::Cache::get().get(id, result))
                return (fromJSON(result));
            else
                return {};
        } catch (std::exception &err) {
            return {};
        }
    }

    void saveToCache(User user) {
        std::stringstream ss;
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", user.id());
        if (!user.login().empty()) root->set("login", user.login());
        if (!user.password().empty()) root->set("password", user.password());
        if (!user.firstName().empty()) root->set("first_name", user.firstName());
        if (!user.lastName().empty()) root->set("last_name", user.lastName());
        if (!user.email().empty()) root->set("email", user.email());
        Poco::JSON::Stringifier::stringify(root, ss);
        std::string message = ss.str();
        std::cout << "HIHIHI" << std::endl;
        std::cout << message << std::endl;
        try {
            database::Cache::get().put(user.id(), message);
        } catch (const std::string& e) {
            std::cout << e << std::endl;
        } catch (...) {
            std::cout << "errror" << std::endl;
        }

    }

}

#endif //HL_SERVER_USER_CACHE_H
