//
// Created by Sin Denis on 02.11.2023.
//

#ifndef HL_SERVER_POST_REQUEST_HANDLER_H
#define HL_SERVER_POST_REQUEST_HANDLER_H

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
#include "../common/utils/request.h"

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

#include "post_db.h"
#include "post.h"

class PostRequestHandler : public HTTPRequestHandler {
public:
    explicit PostRequestHandler() : _format(DateTimeFormat::SORTABLE_FORMAT) {
    };

    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) override {
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
            if (request.getURI().find("/posts") != std::string::npos) {
                std::cout << "HI" << std::endl;
                if (request.getMethod() == "GET") {
                    std::cout << "GET" << std::endl;
                    getPosts(request, response);
                }
                if (request.getMethod() == "POST") {
                    std::cout << "POST" << std::endl;
                    createPost(request, response);
                }
            }
        } catch (...) {
            notFoundResponse(response);
        }
    }


private:
    std::string _format;

    void createPost(HTTPServerRequest &request, HTTPServerResponse &response) {
        long userId = 0;
        std::cout << "hi1" << std::endl;
        if (!auth(request, userId)) {
            std::cout << "hi2" << std::endl;
            unathorizedResponse(response);
        }
        std::cout << "hi3" << std::endl;
        auto &stream = request.stream();
        std::stringstream body_s;
        Poco::StreamCopier::copyStream(stream, body_s);
        Post post = fromPostJSON(body_s.str());
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        postDb::save(post);
        root->set("id", post.id());
        if (!post.summary().empty()) root->set("summary", post.summary());
        if (!post.body().empty()) root->set("body", post.body());
        root->set("user_id", post.userId());
        std::cout << post.userId() << std::endl;
        auto &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    void getPosts(HTTPServerRequest &request, HTTPServerResponse &response) {
        long userId = 0;
        if (!auth(request, userId)) {
            unathorizedResponse(response);
        }
        std::vector<Post> posts = postDb::getAllByUser(userId);
        Poco::JSON::Array arr;
        for (auto& post: posts) {
            arr.add(post.toJSON());
        }
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(arr, ostr);
    }

    bool auth(HTTPServerRequest &request, long& userId) {
        std::string scheme;
        std::string info;
        request.getCredentials(scheme, info);
        return validateToken(scheme, info, userId);
    }

};


#endif //HL_SERVER_POST_REQUEST_HANDLER_H
