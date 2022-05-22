cmake_minimum_required(VERSION 3.16)

project(traffic)

find_package(Threads REQUIRED)

set(${PROJECT_NAME}_sources
    src/application.cpp
    src/arguments.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_sources})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Threads::Threads
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PRIVATE_HEADER "${${PROJECT_NAME}_headers}"
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
