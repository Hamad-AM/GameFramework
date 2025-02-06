cmake_minimum_required(VERSION 3.10)

project(Anthology)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_ITERATOR_DEBUG_LEVEL=1")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

find_package(OpenGL REQUIRED)

add_subdirectory(libraries/glfw)
add_subdirectory(libraries/glm)
add_subdirectory(libraries/freetype-2.13.3)
# add_subdirectory(libraries/basis_universal)

include_directories(${CMAKE_SOURCE_DIR}/libraries/glad/include)
add_library(glad ${CMAKE_SOURCE_DIR}/libraries/glad/src/glad.c)

include_directories(${CMAKE_SOURCE_DIR}/libraries/miniaudio/include)
add_library(miniaudio ${CMAKE_SOURCE_DIR}/libraries/miniaudio/src/miniaudio.c)

#include_directories(${CMAKE_SOURCE_DIR}/libraries/stb/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/cgltf/)
include_directories(${CMAKE_SOURCE_DIR}/libraries/tinygltf/)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp)

file (GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

add_executable(Anthology ${SOURCE_FILES})
target_link_libraries(Anthology glfw glm glad miniaudio freetype)
# add_dependencies(Anthology anthology_shaders)
# set_target_properties(Anthology PROPERTIES FOLDER "src/")
set_property(TARGET Anthology PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /MP")
endif()
