#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <string>
#include <iostream>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/StreamCopier.h>

using Poco::Net::HTTPServerRequest;

template<typename T>
T getOrDefault(Poco::JSON::Object::Ptr object, std::string field, T defaultValue) {
    if (object->has(field)) {
        return object->getValue<T>(field);
    }
    return defaultValue;
};

Poco::Dynamic::Var parse(HTTPServerRequest &request) {
    auto &stream = request.stream();
    std::stringstream body_s;
    Poco::StreamCopier::copyStream(stream, body_s);
    std::string body = body_s.str();
    Poco::JSON::Parser parser;
    return parser.parse(body);
}

#endif