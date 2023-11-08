#include "database.h"
#include "../config/config.h"

namespace database {

    static const int MAX_SHARD = 2;
    static const int SEQUENCE_SHARD = 2;

    Database::Database() {
        _connection_string += "host=";
        _connection_string += Config::get().host();
        _connection_string += ";user=";
        _connection_string += Config::get().login();
        _connection_string += ";db=";
        _connection_string += Config::get().database();
        _connection_string += ";port=";
        _connection_string += Config::get().port();
        _connection_string += ";password=";
        _connection_string += Config::get().password();

        std::cout << "Connection string:" << _connection_string << std::endl;
        Poco::Data::MySQL::Connector::registerConnector();
        _pool = std::make_unique<Poco::Data::SessionPool>(Poco::Data::MySQL::Connector::KEY, _connection_string);
    }

    Database &Database::get() {
        static Database _instance;
        return _instance;
    }

    Poco::Data::Session Database::createSession() {
        return Poco::Data::Session(_pool->get());
    }

    size_t Database::getMaxShard() {
        return MAX_SHARD;
    }

    std::string Database::getShardingHint(long hash) {
        auto result = "-- sharding:" + std::to_string(hash % (getMaxShard() + 1));
        std::cout << result << std::endl;
        return result;
    }

    std::string Database::getSequenceShardingHint() {
        return "-- sharding:" + std::to_string(SEQUENCE_SHARD);
    }

    std::vector<std::string> Database::getAllShardingHints() {
        std::vector<std::string> res;
        for (size_t i = 0; i <= getMaxShard(); ++i) {
            res.push_back(getShardingHint(i));
        }
        return res;
    }

    void Database::query(std::function<void(Poco::Data::Statement)> queryFunction) {
        Poco::Data::Session session = database::Database::get().createSession();
        session.begin();
        try {
            Poco::Data::Statement select(session);
            queryFunction(select);
            if (!select.done())
                select.execute();
            session.commit();
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            session.rollback();
            std::cout << "connection:" << e.what() << " :: " << e.message() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            session.rollback();
            std::cout << "statement:" << e.what() << " :: " << e.message() << std::endl;
            throw;
        }
    }

}