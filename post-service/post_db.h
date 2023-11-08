//
// Created by Sin Denis on 02.11.2023.
//

#ifndef HL_SERVER_POST_DB_H
#define HL_SERVER_POST_DB_H

#include "post.h"
#include "../common/database/database.h"
#include <Poco/JSON/Parser.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace postDb {

    static const std::string INSERT_POST_SQL = "INSERT INTO Post(id, summary, body, user_id) VALUES(?, ?, ?, ?)";
    static const std::string SELECT_USER_POSTS_SQL = "SELECT * FROM Post WHERE user_id = ?";
    static const std::string SELECT_NEXT_POST_ID_SQL = "SELECT NEXTVAL(post_id_sequence)";

    long nextPostId() {
        long id = 0;
        database::Database::query([&id](Statement select) {
            select << SELECT_NEXT_POST_ID_SQL << database::Database::getSequenceShardingHint(), into(id), range(0, 1);
        });
        std::cout << "seq " << id << std::endl;
        return id;
    }

    Post save(Post &post) {
        std::cout << "save post with userId " << post.userId() << std::endl;
        post.id() = nextPostId();
        std::cout << "post" << post.id() << std::endl;
        database::Database::query([&post](Statement insert) {
            insert << INSERT_POST_SQL << database::Database::getShardingHint(post.userId()),
                    use(post.id()),
                    use(post.summary()),
                    use(post.body()),
                    use(post.userId()),
                    range(0, 1);
        });
        return post;
    }

    std::vector<Post> getAllByUser(long &id) {
        std::cout << "getAllBYUser " << id << std::endl;
        Post post;
        std::vector<Post> result;
        std::vector<std::string> shards = database::Database::getAllShardingHints();
        for (const std::string &shard: shards) {
            database::Database::query([&post, &shard, &result, &id](Statement select) {
                select << SELECT_USER_POSTS_SQL << shard,
                        use(id),
                        into(post.id()),
                        into(post.summary()),
                        into(post.body()),
                        into(post.userId()),
                        range(0, 1);
                while (!select.done()) {
                    if (select.execute())
                        result.push_back(post);
                }
            });
        }
        std::cout << result.size() << std::endl;
        return result;
    }

}

#endif //HL_SERVER_POST_DB_H
