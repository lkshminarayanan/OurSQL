set(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++0x")

add_subdirectory(dbengine)

add_executable(oursql Parser.cpp)
target_link_libraries(oursql dbengine)
install(TARGETS oursql RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
