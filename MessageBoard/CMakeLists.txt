add_executable(index index.cpp utilities.cpp utilities.h)
target_link_libraries(index sqlite3)

add_executable(add_message add_message.cpp  utilities.cpp utilities.h)
target_link_libraries(add_message sqlite3)