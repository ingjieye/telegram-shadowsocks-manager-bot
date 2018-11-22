cmake_minimum_required(VERSION 2.8.12)
project(telegram-shadowsocks-manager)

set(Boost_USE_STATIC_LIBS ON)

add_subdirectory(lib/SQLiteCpp)
add_subdirectory(lib/tgbot-cpp)

set(SOURCE_FILES src/server.cc src/database.cc)

add_executable(server ${SOURCE_FILES})
add_executable(test_database src/test_database.cc src/database.cc)

target_link_libraries(server TgBot SQLiteCpp sqlite3 pthread dl)
target_link_libraries(test_database SQLiteCpp sqlite3 pthread dl)