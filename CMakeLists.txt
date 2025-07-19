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

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries")

find_package(OpenGL REQUIRED)

add_subdirectory(libraries/glfw)
# include_directories(libraries/glfw/include)
# add_subdirectory(libraries/glm)
add_subdirectory(libraries/freetype-2.13.3)
# include_directories(libraries/freetype-2.13.3/include)
# add_subdirectory(libraries/KTX-Software)
# add_subdirectory(libraries/gli)

add_subdirectory(libraries/DirectXTex)
add_subdirectory(libraries/assimp)
#
# # add_subdirectory(libraries/basis_universal)
# include_directories(${CMAKE_SOURCE_DIR}/libraries/basis_universal/)
# # set(basisu "${CMAKE_SOURCE_DIR}/libraries/basis_universal/build/Release/")
# add_subdirectory(libraries/basis_universal)

# include_directories(${CMAKE_SOURCE_DIR}/libraries/KTX/include)




# include_directories(${CMAKE_SOURCE_DIR}/libraries/fpng/src/)


# include_directories(${CMAKE_SOURCE_DIR}/libraries/cgltf/)

#include_directories(${CMAKE_SOURCE_DIR}/libraries/stb/)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp)

file (GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

file (GLOB_RECURSE ASSET_CONVERTER_SOURCE
        ${CMAKE_SOURCE_DIR}/tools/AssetConverter/*.c
        ${CMAKE_SOURCE_DIR}/tools/AssetConverter/*.cpp)

file (GLOB_RECURSE ASSET_CONVERTER_HEADER
        ${CMAKE_SOURCE_DIR}/src/common.h
        ${CMAKE_SOURCE_DIR}/src/asset_types.h
        ${CMAKE_SOURCE_DIR}/tools/AssetConverter/*.h
        ${CMAKE_SOURCE_DIR}/tools/AssetConverter/*.hpp)

add_executable(AssetConverter ${ASSET_CONVERTER_HEADER} ${ASSET_CONVERTER_SOURCE})
target_link_libraries(AssetConverter assimp)

add_executable(Anthology ${SOURCE_FILES})

target_include_directories(Anthology PRIVATE ${CMAKE_SOURCE_DIR}/libraries/glm/)
target_include_directories(Anthology PRIVATE ${CMAKE_SOURCE_DIR}/libraries/miniaudio/)
target_include_directories(Anthology PRIVATE ${CMAKE_SOURCE_DIR}/libraries/stb/)
target_include_directories(Anthology PRIVATE ${CMAKE_SOURCE_DIR}/libraries/tinygltf/)
add_library(glad ${CMAKE_SOURCE_DIR}/libraries/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/libraries/glad/include)
target_link_libraries(Anthology glfw glad freetype)
# target_link_libraries(Anthology glfw glm glad freetype ktx)
# add_dependencies(Anthology anthology_shaders)
# set_target_properties(Anthology PROPERTIES FOLDER "src/")
set_property(TARGET Anthology PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_property(TARGET AssetConverter PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /MP")
endif()
