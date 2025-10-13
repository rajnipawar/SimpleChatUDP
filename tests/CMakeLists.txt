cmake_minimum_required(VERSION 3.16)

find_package(Qt6 COMPONENTS Test)
if(NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Test)
endif()

enable_testing()

set(TEST_SOURCES
    test_basic.cpp
    ../src/message.cpp
)

if(QT_VERSION EQUAL 6)
    qt_add_executable(test_basic ${TEST_SOURCES})
    target_link_libraries(test_basic
        PRIVATE
        Qt6::Core
        Qt6::Test)
    target_include_directories(test_basic PRIVATE ../src)
else()
    add_executable(test_basic ${TEST_SOURCES})
    target_link_libraries(test_basic Qt5::Core Qt5::Test)
    target_include_directories(test_basic PRIVATE ../src)
endif()

add_test(NAME BasicTests COMMAND test_basic)
