cmake_minimum_required(VERSION 3.14)
project(marching-squares)
set(CMAKE_CXX_STANDARD 20)

# for Windows, using vcpkg:
#   set environment variable CMAKE_TOOLCHAIN_FILE
#   to .../vcpkg/scripts/buildsystems/vcpkg.cmake

# packages to find

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)


# all sources

set(GLAD
        include/glad/glad.h
        include/KHR/khrplatform.h
        src/glad/glad.c
)

set(ALL_INCLUDE_DIRS
        ${OPENGL_INCLUDE_DIR}
        include
)

set(ALL_SOURCE_FILES
        ${GLAD}
        src/main.cpp
)

set(ALL_LIBRARIES
        glfw
)

if (NOT WIN32)
        list(APPEND ALL_LIBRARIES
                dl
        )
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             FILES ${ALL_SOURCE_FILES}
)


# executable target(s)

set(EXECUTABLE marching-squares)
add_executable(${EXECUTABLE} ${ALL_SOURCE_FILES})

set_property(TARGET ${EXECUTABLE} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXECUTABLE})

target_include_directories(${EXECUTABLE} PUBLIC ${ALL_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE} ${ALL_LIBRARIES})

