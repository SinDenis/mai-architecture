cmake_minimum_required(VERSION 3.2)

project(hl_server C CXX)

SET (USER_SERVICE "user-service")
SET (POST_SERVICE "post-service")
SET (CHAT_SERVICE "chat-service")
set (STD_CXX "c++17")
set (DLLVM_CXX_STD, ${STD_CXX})
set (REDISCPP_FLAGS "-DREDISCPP_HEADER_ONLY=ON")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=${STD_CXX} ${REDISCPP_FLAGS}")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -std=${STD_CXX} -Wall -DNDEBUG ${REDISCPP_FLAGS}")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${USER_SERVICE} user-service.cpp
        common/config/config.cpp
        common/database/cache.cpp
        common/database/database.cpp
        common/utils/auth.h
        common/utils/request.h
)
target_include_directories(${USER_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${USER_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${USER_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${USER_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${USER_SERVICE} PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${Poco_LIBRARIES}
        "PocoData"
        "PocoJWT"
        "PocoDataMySQL"
        "mysqlclient"
        ZLIB::ZLIB)

add_executable(${POST_SERVICE}
        common/config/config.cpp
        common/database/database.cpp
        common/utils/auth.h
        post-service/main.cpp
        post-service/post_request_handler.h
        post-service/post.h
        post-service/post_db.h
        common/utils/request.h
)

target_include_directories(${POST_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${POST_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${POST_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${POST_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(${POST_SERVICE} PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${Poco_LIBRARIES}
        "PocoData"
        "PocoDataMySQL"
        "mysqlclient"
        ZLIB::ZLIB)

add_executable(${CHAT_SERVICE}
        common/config/config.cpp
        common/database/database.cpp
        common/utils/auth.h
        chat-service/main.cpp
        chat-service/message_request_handler.h
        chat-service/message.h
        chat-service/message_db.h
        common/utils/request.h
)
target_include_directories(${CHAT_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${CHAT_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${CHAT_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${CHAT_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${CHAT_SERVICE} PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${Poco_LIBRARIES}
        "PocoData"
        "PocoDataMySQL"
        "mysqlclient"
        ZLIB::ZLIB)