cmake_minimum_required(VERSION 3.15)

project(web-app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Crow CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

add_executable(app src/main.cpp)

target_link_libraries(app PRIVATE Crow::Crow libpqxx::pqxx)
