set(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++0x")

add_subdirectory(datamodels)
add_library(dbengine STATIC
            $<TARGET_OBJECTS:datamodels>
            Select.cpp
            Where.cpp
            Modify.cpp
            Table.cpp
            DataBaseServer.cpp)
