cmake_minimum_required(VERSION 3.0)

project(test_sqlite)

include_directories(.)
include_directories(./src)

link_directories(./)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wno-write-strings -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -Wno-write-strings -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

set(LIB_SRC
    src/Sqlite.cpp

)

add_library(test_sqlite SHARED ${LIB_SRC})

# 动态链接库也要连接上自己创建的库
set(LIBS
    sqlite3
    test_sqlite
)

add_executable(test_sql "test.cpp")
add_dependencies(test_sql test_sqlite)
target_link_libraries(test_sql ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)