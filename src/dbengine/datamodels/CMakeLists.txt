set(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++0x")

add_library(datamodels OBJECT
            Cache.cpp
            Page.cpp
            DataPage.cpp
            DirPage.cpp
            Record.cpp
            RecordSet.cpp
            DataBaseHeader.cpp)
