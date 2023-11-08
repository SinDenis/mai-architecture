//
// Created by Sin Denis on 01.11.2023.
//

#ifndef HL_SERVER_USER_STORAGE_H
#define HL_SERVER_USER_STORAGE_H

#include <optional>
#include "user_cache.h"
#include "user_db.h"

namespace userStorage {

    User getUserById(long id) {
        std::optional<User> cacheUser = userCache::getFromCacheById(id);
        if (cacheUser.has_value()) {
            return cacheUser.value();
        }
        User user = userDb::getById(id);
        userCache::saveToCache(user);
        return user;
    }

    User saveUser(User user) {
        user = userDb::save(user);
        try {
            std::cout << "HIdsadfasdf" << std::endl;
            userCache::saveToCache(user);
        } catch (...) {
            std::cout << "failed save to cache" << std::endl;
        }

        return user;
    }
}

#endif //HL_SERVER_USER_STORAGE_H
