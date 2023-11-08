//
// Created by Sin Denis on 29.10.2023.
//

#ifndef HL_SERVER_USER_DB_H
#define HL_SERVER_USER_DB_H

#pragma once

#include "user.h"
#include "../common/database/database.h"
#include <Poco/JSON/Parser.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace userDb {

    static const std::string INSERT_POST_SQL = "INSERT INTO `User`(id, login, password, first_name, last_name, email) VALUES(?, ?, ?, ?, ?, ?)";
    static const std::string SELECT_USER_POSTS_SQL = "SELECT * FROM `User` WHERE id = ?";
    static const std::string SELECT_NEXT_POST_ID_SQL = "SELECT NEXTVAL(user_id_sequence)";

    long nextUserId() {
        long id = 0;
        database::Database::query([&id](Statement select) {
            select << SELECT_NEXT_POST_ID_SQL << database::Database::getSequenceShardingHint(), into(id), range(0, 1);
        });
        return id;
    }

    User save(User &user) {
        user.id() = nextUserId();
        database::Database::query([&user](Statement insert) {
            insert << INSERT_POST_SQL << database::Database::getShardingHint(user.id()),
                    use(user.id()),
                    use(user.login()),
                    use(user.password()),
                    use(user.firstName()),
                    use(user.lastName()),
                    use(user.email());
        });
        return user;
    }

    User getById(long id) {
        User user;
        database::Database::query([&id, &user](Statement select) {
            select << SELECT_USER_POSTS_SQL << database::Database::getShardingHint(id),
                    into(user.id()),
                    into(user.login()),
                    into(user.firstName()),
                    into(user.lastName()),
                    into(user.email()),
                    use(id),
                    range(0, 1);
        });
        return user;
    }

    long auth(std::string &login, std::string &password) {
        std::vector<std::string> shards = database::Database::getAllShardingHints();
        long id = 0;
        for (const std::string &shard: shards) {
            database::Database::query([&id, &shard, &login, &password](Statement select) {
                select << "SELECT id FROM User where login=? and password=?" << shard,
                        into(id),
                        use(login),
                        use(password),
                        range(0, 1);
            });
        }
        return id;
    }

    std::vector<User> search(User searchParams) {
        std::vector<User> result;
        User user;
        std::ostringstream os;
        os << "select id, login, email, first_name, last_name from User where id > 0";
        std::cout << "hi" << std::endl;

        if (searchParams.login().length() > 0) {
            std::replace(searchParams.login().begin(), searchParams.login().end(), ' ', '%');
            os << " and lower(login) like '%" + searchParams.login() + "%'";
        }

        if (searchParams.email().length() > 0) {
            os << " and lower(email) like '%" + searchParams.email() + "%'";
        }

        if (searchParams.firstName().length() > 0) {
            std::replace(searchParams.firstName().begin(), searchParams.firstName().end(), ' ', '%');
            os << " and lower(first_name) like '%" + searchParams.firstName() + "%'";
        }

        if (searchParams.lastName().length() > 0) {
            std::replace(searchParams.lastName().begin(), searchParams.lastName().end(), ' ', '%');
            os << " and lower(first_name) like '%" + searchParams.lastName() + "%'";
        }

        std::vector<std::string> shards = database::Database::getAllShardingHints();


        for (const std::string &shard: shards) {
            database::Database::query([&user, &shard, &result, &os](Statement select) {
                select << os.str() << shard,
                        into(user.id()),
                        into(user.login()),
                        into(user.email()),
                        into(user.firstName()),
                        into(user.lastName()),
                        range(0, 1);
                while (!select.done()) {
                    if (select.execute())
                        result.push_back(user);
                }
            });
        }

        return result;
    }

}

#endif //HL_SERVER_USER_DB_H
