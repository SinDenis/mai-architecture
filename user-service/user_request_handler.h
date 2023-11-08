#ifndef USER_REST_H
#define USER_REST_H

#pragma once

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/JSON/Object.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/SharedPtr.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <Poco/URIStreamFactory.h>
#include <Poco/URIStreamOpener.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Base64Encoder.h>
#include <Poco/Base64Decoder.h>

#include "../common/utils/json.h"
#include "../common/utils/auth.h"
#include "user_storage.h"
#include "../common/utils/request.h"
#include "../common/config/config.h"

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;
using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::NameValueCollection;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <cppkafka/cppkafka.h>

class UserRequestHandler : public HTTPRequestHandler {
public:
    explicit UserRequestHandler() : _format(DateTimeFormat::SORTABLE_FORMAT) {
        _handlers["/users"]["POST"] = [&](HTTPServerRequest &req, HTTPServerResponse &resp) { createUser(req, resp); };
        _handlers["/search"]["GET"] = [&](HTTPServerRequest &req, HTTPServerResponse &resp) { searchUser(req, resp); };
        _handlers["/users-by-id"]["GET"] = [&](HTTPServerRequest &req, HTTPServerResponse &resp) {
            getById(req, resp);
        };
    };

    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) override {
        std::cout << "fasdfasdf" << std::endl;
        response.add("Access-Control-Allow-Origin", "*");
        response.add("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        response.add("Access-Control-Allow-Headers", "Content-Type, api_key, Authorization");
        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_OPTIONS) {
            response.setContentType("application/json");
            response.setKeepAlive(true);
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
            response.send();
            return;
        }
        try {
            std::string uri = request.getURI();
            if (uri.find("/auth") != std::string::npos && request.getMethod() == "POST") {
                auth(request, response);
                return;
            }
            if (uri.find("/users-by-id") != std::string::npos) {
                uri = "/users-by-id";
            }
            if (uri.find("/search") != std::string::npos) {
                uri = "/search";
            }
            _handlers[uri][request.getMethod()](request, response);
        } catch (...) {
            notFoundResponse(response);
        }
    }


private:
    std::string _format;
    std::unordered_map<std::string, std::unordered_map<std::string, std::function<void(HTTPServerRequest &,
                                                                                       HTTPServerResponse &)>>> _handlers;

    void auth(HTTPServerRequest &request, HTTPServerResponse &response) {
        std::string scheme;
        std::string info;
        std::cout << "HELLO AUTH";
        request.getCredentials(scheme, info);
        std::cout << "scheme: " << scheme << " identity: " << info << std::endl;

        std::string login, password;
        if (scheme == "Basic") {
            getIdentity(info, login, password);
            if (auto id = userDb::auth(login, password)) {
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                ostr << "{ \"id\" : \"" << id << "\"}" << std::endl;
                return;
            }
        }
        unathorizedResponse(response);
    }

    void createUser(HTTPServerRequest &request, HTTPServerResponse &response) {
        auto &stream = request.stream();
        std::stringstream body_s;
        Poco::StreamCopier::copyStream(stream, body_s);
        User user = fromJSON(body_s.str());
        std::cout << user.id() << " " << user.login() << std::endl;
        user = userStorage::saveUser(user);
        sendToKafka(user);
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", user.id());
        if (!user.login().empty()) root->set("login", user.login());
        if (!user.firstName().empty()) root->set("first_name", user.firstName());
        if (!user.lastName().empty()) root->set("last_name", user.lastName());
        if (!user.email().empty()) root->set("email", user.email());
        auto &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    void searchUser(HTTPServerRequest &request, HTTPServerResponse &response) {
        User searchParams;
        const Poco::URI uri(request.getURI());
        const Poco::URI::QueryParameters params = uri.getQueryParameters();
        for (std::pair<std::string, std::string> key_value: params) {
            if (key_value.first == "login") {
                searchParams.login() = key_value.second;
            } else if (key_value.first == "first_name") {
                searchParams.firstName() = key_value.second;
            } else if (key_value.first == "last_name") {
                searchParams.lastName() = key_value.second;
            }
        }
        std::vector<User> users = userDb::search(searchParams);
        Poco::JSON::Array arr;
        for (User user: users) {
            arr.add(user.toJSON());
        }
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(arr, ostr);
    }

    void getById(HTTPServerRequest &request, HTTPServerResponse &response) {
        const Poco::URI uri(request.getURI());
        const Poco::URI::QueryParameters params = uri.getQueryParameters();
        int userId;
        for (const std::pair<std::string, std::string> &key_value: params) {
            if (key_value.first == "id") {
                userId = stoi(key_value.second);
            }
        }
        std::cout << userId << std::endl;
        User user = userStorage::getUserById(userId);
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(user.toJSON(), ostr);
    }

    void sendToKafka(User& user) {
        static cppkafka::Configuration config ={
                {"metadata.broker.list", Config::get().queue_host()},
                {"acks","all"}};
        static cppkafka::Producer producer(config);
        static std::mutex mtx;
        static int message_key{0};
        using Hdr = cppkafka::MessageBuilder::HeaderType;

        std::lock_guard<std::mutex> lock(mtx);
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(user.toJSON(), ss);
        std::string message = ss.str();
        bool not_sent = true;

        cppkafka::MessageBuilder builder(Config::get().queue_topic());
        std::string mk=std::to_string(++message_key);
        builder.key(mk); // set some key
        builder.header(Hdr{"producer_type","author writer"}); // set some custom header
        builder.payload(message); // set message

        while (not_sent)
        {
            try
            {
                producer.produce(builder);
                not_sent = false;
            }
            catch (...)
            {
            }
        }
    }
};


#endif