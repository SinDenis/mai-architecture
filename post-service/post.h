//
// Created by Sin Denis on 02.11.2023.
//

#ifndef HL_SERVER_POST_H
#define HL_SERVER_POST_H

#include <string>

class Post {
public:
    long &id() {
        return _id;
    }

    std::string &summary() {
        return _summary;
    }

    std::string &body() {
        return _body;
    }

    long &userId() {
        return _userId;
    }

    Poco::JSON::Object::Ptr toJSON() {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", _id);
        if (!summary().empty()) root->set("summary", summary());
        if (!body().empty()) root->set("body", body());
        if (userId() > 0) root->set("user_id", userId());
        return root;
    }

private:
    long _id = 0;
    std::string _summary;
    std::string _body;
    long _userId = 0;
};

Post fromPostJSON(std::string s) {
    Post post;
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result = parser.parse(s);
    const auto& object = result.extract<Poco::JSON::Object::Ptr>();
    post.id() = getOrDefault<long>(object, "id", 0);
    post.summary() = getOrDefault<std::string>(object, "summary", "");
    post.body() = getOrDefault<std::string>(object, "body", "");
    post.userId() = getOrDefault<long>(object, "user_id", 0);
    return post;
}

#endif //HL_SERVER_POST_H
