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
    ThreadPool.cpp ThreadPool.h
    HTTPVersion.h
)

target_link_libraries(
    CPPWebServer
    pthread
)

add_dependencies(CPPWebServer cgi_test index add_message)

execute_process(COMMAND cp ${CMAKE_SOURCE_DIR}/config.ini ${CMAKE_BINARY_DIR}/config.ini)
execute_process(COMMAND cp -r ${CMAKE_SOURCE_DIR}/HelloWorld ${CMAKE_BINARY_DIR}/HelloWorld)
execute_process(COMMAND cp -r ${CMAKE_SOURCE_DIR}/MessageBoard ${CMAKE_BINARY_DIR}/MessageBoard)

add_subdirectory(HelloWorld)
add_subdirectory(MessageBoard)

add_executable(testThreadPool testThreadPool.cpp ThreadPool.cpp ThreadPool.h)
target_link_libraries(testThreadPool pthread)
