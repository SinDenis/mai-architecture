#include <iostream>
#include "user_request_handler.h"
#include "../common/web_server/web_server.h"
#include "../common/web_server/request_factory.h"

int main(int argc, char *argv[]) {
    auto *userRequestFactory = new WebServerRequestFactory<UserRequestHandler>({"/users", "search", "/auth"}, DateTimeFormat::SORTABLE_FORMAT);
    HTTPWebServer webServer(userRequestFactory);
    webServer.run(argc, argv);
    return 0;
}