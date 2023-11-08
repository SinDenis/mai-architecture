//
// Created by Sin Denis on 02.11.2023.
//

#ifndef HL_SERVER_AUTH_H
#define HL_SERVER_AUTH_H

#include <string>
#include "Poco/Base64Decoder.h"
#include <istream>
#include <ostream>

bool getIdentity(const std::string identity, std::string &login, std::string &password)
{
    std::istringstream istr(identity);
    std::ostringstream ostr;
    Poco::Base64Decoder b64in(istr);
    copy(std::istreambuf_iterator<char>(b64in),
         std::istreambuf_iterator<char>(),
         std::ostreambuf_iterator<char>(ostr));
    std::string decoded = ostr.str();

    size_t pos = decoded.find(':');
    login = decoded.substr(0, pos);
    password = decoded.substr(pos + 1);
    return true;
}

bool validateToken(std::string scheme, std::string token, long &id) {
    if (scheme.length() == 0 || token.length() == 0) {
        return false;
    }

    std::string host = "localhost";
    std::string authPort = "8080";
    if (std::getenv("USER_SERVICE_HOST") != nullptr)
        host = std::getenv("USER_SERVICE_HOST");
    if (std::getenv("USER_SERVICE_PORT") != nullptr) {
        authPort = std::getenv("USER_SERVICE_PORT");
    }
    std::string url = "http://" + host + ":" + authPort + "/auth";
    std::cout << url << std::endl;
    std::cout << scheme << " " << token << std::endl;
    try {
        Poco::URI uri(url);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.toString());
        request.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
        request.setKeepAlive(true);
        request.setCredentials(scheme, token);
        session.sendRequest(request);
        Poco::Net::HTTPResponse response;
        std::istream &rs = session.receiveResponse(response);
        std::string response_body;

        while (rs) {
            char c{};
            rs.read(&c, 1);
            if (rs)
                response_body += c;
        }

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(response_body);
        const auto& object = result.extract<Poco::JSON::Object::Ptr>();
        id = getOrDefault<long>(object, "id", 0);

        if (response.getStatus() != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK) {
            std::cout << "Failed to validate token [" << response.getStatus() << "] " << response_body << std::endl;
            return false;
        }
    } catch (Poco::Exception &ex) {
        std::cout << "Failed to validate token " << ex.what() << " :: " << ex.message() << std::endl;
        return false;
    }
    return true;
}

#endif //HL_SERVER_AUTH_H
