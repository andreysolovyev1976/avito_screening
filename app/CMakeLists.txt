cmake_minimum_required(VERSION 3.21)
project(${PROJECT_NAME}
        VERSION 0.1
        DESCRIPTION "Freq project"
        LANGUAGES CXX)

add_executable(${PROJECT_NAME}
        main.cpp
        files.cpp
        )

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
