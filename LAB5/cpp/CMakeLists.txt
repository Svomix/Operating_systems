#Cmake для sqlite на сервере 
cmake_minimum_required(VERSION 3.10)
project(lab5)

set(CMAKE_CXX_STANDARD 20)

set(SQLite3_INCLUDE_DIR E:/lab5/pls)
set(SQLite3_SOURCES E:/lab5/pls/sqlite3.c)


add_executable(lab5 main.cpp ${SQLite3_SOURCES})
target_link_libraries(lab5 ws2_32)
target_include_directories(lab5 PRIVATE ${SQLite3_INCLUDE_DIR})