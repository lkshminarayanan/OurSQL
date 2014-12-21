set(CMAKE_CXX_FLAGS "-g -Wno-write-strings -Wall -Werror -std=c++0x")

add_subdirectory(datamodels)
add_library(dbengine STATIC
            $<TARGET_OBJECTS:datamodels>
            Select.cpp
            Where.cpp
            Modify.cpp
            Alter.cpp
            Table.cpp
            DataBaseServer.cpp)
