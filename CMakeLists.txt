cmake_minimum_required(VERSION 3.10)

project(Anthology)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp)

file (GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

set(LIB_DIR "${CMAKE_SOURCE_DIR}/libraries")

find_package(OpenGL REQUIRED)

set(SDL2_DIR "${LIB_DIR}/SDL2-2.0.12")
set(GLAD_DIR "${LIB_DIR}/glad")
set(GLM_DIR  "${LIB_DIR}/glm")
set(STB_IMAGE_DIR "${LIB_DIR}/stb_image")
set(ASSIMP_DIR "${LIB_DIR}/assimp")
set(FMOD_DIR "${LIB_DIR}/fmod/api/core")
set(PHYSX_DIR "${LIB_DIR}/PhysX/physx")

set(SDL2_LIB "${SDL2_DIR}/lib/x64/SDL2.lib")
set(SDL2_MAIN_LIB "${SDL2_DIR}/lib/x64/SDL2main.lib")
set(SDL2_INCLUDE "${SDL2_DIR}/include")

set(GLAD_LIB "${GLAD_DIR}/src/glad.c")
set(GLAD_INCLUDE "${GLAD_DIR}/include")

set(STB_IMAGE_LIB "${STB_IMAGE_DIR}/stb_image.cpp")
set(STB_IMAGE_INCLUDE "${STB_IMAGE_DIR}")

set(GLM_INCLUDE "${GLM_DIR}")

set(ASSIMP_LIB "${ASSIMP_DIR}/lib/Release/assimp-vc142-mt.lib")
set(ASSIMP_INCLUDE "${ASSIMP_DIR}/include")

set(FMOD_LIB "${FMOD_DIR}/lib/x64/fmod_vc.lib")
set(FMOD_INCLUDE "${FMOD_DIR}/inc")

set(PHYSX_INCLUDE "${PHYSX_DIR}/inlude")
set(PHYSX_COMMON_LIB "${PHYSX_DIR}/bin/win.x86_64.vc142.mt/checked/PhysXCommon_64.lib")
set(PHYSX_LIB "${PHYSX_DIR}/bin/win.x86_64.vc142.mt/checked/PhysX_64.lib")
set(PHYSX_FOUNDATION_LIB "${PHYSX_DIR}/bin/win.x86_64.vc142.mt/checked/PhysXFoundation_64.lib")


include_directories(${PHYSX_INCLUDE} ${SDL2_INCLUDE} ${GLAD_INCLUDE} ${GLM_INCLUDE} ${STB_IMAGE_INCLUDE} ${ASSIMP_INCLUDE} ${FMOD_INCLUDE})

add_executable(${PROJECT_NAME} ${GLAD_LIB} ${STB_IMAGE_LIB} ${HEADER_FILES} ${SOURCE_FILES})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIB_DIR}/dll_files/"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

target_link_libraries(${PROJECT_NAME} ${PHYSX_LIB} ${PHYSX_COMMON_LIB} ${PHYSX_FOUNDATION_LIB} ${SDL2_LIB} ${SDL2_MAIN_LIB} ${ASSIMP_LIB} ${FMOD_LIB}) 
