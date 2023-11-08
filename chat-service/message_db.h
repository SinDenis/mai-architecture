//
// Created by Sin Denis on 02.11.2023.
//

#ifndef HL_SERVER_MESSAGE_DB_H
#define HL_SERVER_MESSAGE_DB_H

#include "message.h"
#include "../common/database/database.h"
#include <Poco/JSON/Parser.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace messageDb {

    static const std::string INSERT_MESSAGE_SQL = "INSERT INTO Message(id, text, user_from, user_to) VALUES(?, ?, ?, ?)";
    static const std::string SELECT_USER_MESSAGES_SQL = "SELECT * FROM Message WHERE user_to = ?";
    static const std::string SELECT_NEXT_MESSAGE_ID_SQL = "SELECT NEXTVAL(message_id_sequence)";

    long nextMessageId() {
        long id = 0;
        database::Database::query([&id](Statement select) {
            select << SELECT_NEXT_MESSAGE_ID_SQL << database::Database::getSequenceShardingHint(), into(id), range(0,1);
        });
        return id;
    }

    Message save(Message &message) {
        message.id() = nextMessageId();
        database::Database::query([&message](Statement insert) {
            insert << INSERT_MESSAGE_SQL << database::Database::getShardingHint(message.userTo()),
                    use(message.id()),
                    use(message.text()),
                    use(message.userFrom()),
                    use(message.userTo()),
                    range(0, 1);
        });
        return message;
    }

    std::vector<Message> getAllByUser(long &id) {
        Message message;
        std::vector<Message> result;
        std::vector<std::string> shards = database::Database::getAllShardingHints();
        for (const std::string &shard: shards) {
            database::Database::query([&message, &shard, &result, &id](Statement select) {
                select << SELECT_USER_MESSAGES_SQL << shard,
                        use(id),
                        into(message.id()),
                        into(message.text()),
                        into(message.userFrom()),
                        into(message.userTo()),
                        range(0, 1);
                std::cout << "userFrom " << message.userFrom();
                while (!select.done()) {
                    if (select.execute())
                        result.push_back(message);
                }
            });
        }
        return result;
    }

}

#endif //HL_SERVER_MESSAGE_DB_H
