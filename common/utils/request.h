//
// Created by Sin Denis on 02.11.2023.
//

#ifndef HL_SERVER_REQUEST_H
#define HL_SERVER_REQUEST_H

#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Object.h>

using Poco::Net::HTTPServerResponse;

void notFoundResponse(HTTPServerResponse &response) {
    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/not_found");
    root->set("title", "Internal exception");
    root->set("status", "404");
    root->set("detail", "user not found");
    root->set("instance", "/user");
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
}

void unathorizedResponse(HTTPServerResponse &response) {
    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/unauthorized");
    root->set("title", "Internal exception");
    root->set("status", "401");
    root->set("detail", "not authorized");
    root->set("instance", "/auth");
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
}

#endif //HL_SERVER_REQUEST_H
