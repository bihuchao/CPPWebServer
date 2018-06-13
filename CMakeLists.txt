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

execute_process(COMMAND cp ${CMAKE_SOURCE_DIR}/config.ini ${CMAKE_BINARY_DIR}/config.ini)
execute_process(COMMAND cp ${CMAKE_SOURCE_DIR}/index.html ${CMAKE_BINARY_DIR}/index.html)
execute_process(COMMAND cp ${CMAKE_SOURCE_DIR}/test.txt ${CMAKE_BINARY_DIR}/test.txt)
execute_process(COMMAND cp ${CMAKE_SOURCE_DIR}/linus.jpg ${CMAKE_BINARY_DIR}/linus.jpg)
execute_process(COMMAND cp ${CMAKE_SOURCE_DIR}/happy.gif ${CMAKE_BINARY_DIR}/happy.gif)
execute_process(COMMAND cp ${CMAKE_SOURCE_DIR}/404.html ${CMAKE_BINARY_DIR}/404.html)

add_executable(cgi_test cgi_test.cpp)
