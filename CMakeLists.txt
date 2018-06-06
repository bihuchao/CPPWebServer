cmake_minimum_required(VERSION 3.10)

project(CPPWebServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(
    CPPWebServer
    main.cpp
    config.cpp config.h
    webserver.cpp webserver.h
    request.cpp request.h
)

configure_file(config.ini ${CMAKE_BINARY_DIR}/config.ini)
