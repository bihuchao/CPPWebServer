cmake_minimum_required(VERSION 3.10)

project(CPPWebServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(
    CPPWebServer
    main.cpp
    config.cpp config.h
    webserver.cpp webserver.h
    request.cpp request.h
    response.cpp response.h
    HTTPVersion.h
)

configure_file(config.ini ${CMAKE_BINARY_DIR}/config.ini)
configure_file(index.html ${CMAKE_BINARY_DIR}/index.html)
configure_file(404.html ${CMAKE_BINARY_DIR}/404.html)

add_executable(cgi_test cgi_test.cpp)
