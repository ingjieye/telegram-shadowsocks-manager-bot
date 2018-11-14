cmake_minimum_required(VERSION 2.8.12)
project(telegram-shadowsocks-manager)

set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
set(Boost_USE_STATIC_LIBS ON)

add_subdirectory(SQLiteCpp)
add_subdirectory(tgbot-cpp)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

set(SOURCE_FILES src/server.cc)

add_executable(server ${SOURCE_FILES})
add_executable(database src/database.cc)

target_link_libraries(server TgBot ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})
target_link_libraries(database SQLiteCpp sqlite3 pthread dl)