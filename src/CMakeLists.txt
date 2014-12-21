set(CMAKE_CXX_FLAGS "-g -Wno-write-strings -Wall -Werror -std=c++0x")

add_subdirectory(dbengine)

add_executable(oursql Parser.cpp main.cpp)
target_link_libraries(oursql dbengine readline)
install(TARGETS oursql RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
