#include <iostream>
#include "../common/web_server/web_server.h"
#include "../common/web_server/request_factory.h"
#include "post_request_handler.h"

int main(int argc, char *argv[]) {
    auto *userRequestFactory = new WebServerRequestFactory<PostRequestHandler>({"/posts"}, DateTimeFormat::SORTABLE_FORMAT);
    HTTPWebServer webServer(userRequestFactory);
    webServer.run(argc, argv);
    return 0;
}