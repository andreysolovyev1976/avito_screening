cmake_minimum_required(VERSION 3.21)
set (PROJECT_NAME_TEST ${PROJECT_NAME}_tests)
project(${PROJECT_NAME_TEST}
        VERSION 0.1
        DESCRIPTION "Testing, Freq project"
        LANGUAGES CXX)

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
endif (APPLE)

find_package(GTest REQUIRED)
if(GTest_FOUND)
    message(STATUS "TESTS FOUND GTest, including and linking...")
    include_directories(${GTEST_INCLUDE_DIRS})
else()
    message(STATUS "NOT FOUND GTest")
endif()

add_executable(${PROJECT_NAME_TEST}
        ../app/files.cpp

        tests_all.cpp
        tests_end_to_end.cpp
        )

target_link_libraries(${PROJECT_NAME_TEST}
        PRIVATE
        GTest::GTest
        Threads::Threads
        )

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME_TEST})
