//
// Created by Sin Denis on 02.11.2023.
//

#ifndef HL_SERVER_MESSAGE_H
#define HL_SERVER_MESSAGE_H

#include <string>

class Message {
public:
    long &id() {
        return _id;
    }

    std::string &text() {
        return _text;
    }

    long &userFrom() {
        return _userFrom;
    }

    long &userTo() {
        return _userTo;
    }

    Poco::JSON::Object::Ptr toJSON() {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", _id);
        if (!text().empty()) root->set("text", text());
        root->set("user_from", userFrom());
        root->set("user_to", userTo());
        return root;
    }

private:
    long _id = 0;
    std::string _text;
    long _userFrom = 0;
    long _userTo = 0;
};

Message fromMessageJSON(std::string s) {
    Message message;
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result = parser.parse(s);
    const auto &object = result.extract<Poco::JSON::Object::Ptr>();
    message.id() = getOrDefault<long>(object, "id", 0);
    message.text() = getOrDefault<std::string>(object, "text", "");
    message.userFrom() = getOrDefault<long>(object, "user_from", 0);
    message.userTo() = getOrDefault<long>(object, "user_to", 0);
    return message;
}

#endif //HL_SERVER_MESSAGE_H
