cmake_minimum_required(VERSION 3.10)

project(Anthology)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp)

file (GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

set(LIB_DIR "${CMAKE_SOURCE_DIR}/libraries")

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
# find_package(SDL2 REQUIRED)
        

set(SDL2_DIR "${LIB_DIR}/SDL2-2.0.16")
set(GLAD_DIR "${LIB_DIR}/glad")
set(GLM_DIR  "${LIB_DIR}/glm")
set(GLFW_DIR "${LIB_DIR}/glfw")
set(STB_IMAGE_DIR "${LIB_DIR}/stb_image")
# set(ASSIMP_DIR "${LIB_DIR}/assimp")
# set(FMOD_DIR "${LIB_DIR}/fmod/api/core")
# set(PHYSX_DIR "${LIB_DIR}/PhysX/physx")

# set(SDL2_LIB "${SDL2_DIR}/lib/x64/SDL2.lib")
# set(SDL2_MAIN_LIB "${SDL2_DIR}/lib/x64/SDL2main.lib")
# set(SDL2_INCLUDE "${SDL2_DIR}/include")
# set(GLFW_LIB "${GLFW_DIR}/build/src/RelWithDebInfo/glfw3.lib")
set(GLFW_LIB_LINUX "${GLFW_DIR}/build/src/libglfw3.a")
set(GLFW_INCLUDE "${GLFW_DIR}/include/")

set(BOX2D_LIB_LINUX "${LIB_DIR}/box2d/build/bin/libbox2d.a")
set(BOX2D_INCLUDE "${LIB_DIR}/box2d/include/")

set(GLAD_LIB "${GLAD_DIR}/src/glad.c")
set(GLAD_INCLUDE "${GLAD_DIR}/include")

set(STB_IMAGE_LIB "${STB_IMAGE_DIR}/stb_image.cpp")
set(STB_IMAGE_INCLUDE "${STB_IMAGE_DIR}")

set(GLM_INCLUDE "${GLM_DIR}")
set(ENTT_INCLUDE "${LIB_DIR}/entt/single_include/entt.hpp")

set(FT2_INCLUDE "${LIB_DIR}/freetype-2.11.1/include")
set(FT2_LIB "${LIB_DIR}/freetype-2.11.1/build/libfreetype.a")

# set(ASSIMP_LIB "${ASSIMP_DIR}/lib/Release/assimp-vc142-mt.lib")
# set(ASSIMP_INCLUDE "${ASSIMP_DIR}/include")

# set(FMOD_LIB "${FMOD_DIR}/lib/x64/fmod_vc.lib")
# set(FMOD_INCLUDE "${FMOD_DIR}/inc")

# set(PHYSX_INCLUDE "${PHYSX_DIR}/inlude")
# set(PHYSX_COMMON_LIB "${PHYSX_DIR}/bin/win.x86_64.vc142.mt/checked/PhysXCommon_64.lib")
# set(PHYSX_LIB "${PHYSX_DIR}/bin/win.x86_64.vc142.mt/checked/PhysX_64.lib")
# set(PHYSX_FOUNDATION_LIB "${PHYSX_DIR}/bin/win.x86_64.vc142.mt/checked/PhysXFoundation_64.lib")


# add_library(AnthologyLib )

include_directories(${GLFW_INCLUDE} ${SDL2_INCLUDE_DIRS} ${PHYSX_INCLUDE} ${SDL2_INCLUDE} ${GLAD_INCLUDE} ${GLM_INCLUDE} ${STB_IMAGE_INCLUDE} ${ASSIMP_INCLUDE} ${FMOD_INCLUDE} ${BOX2D_INCLUDE} ${ENTT_INCLUDE} ${FT2_INCLUDE})

add_executable(Anthology ${GLAD_LIB} ${STB_IMAGE_LIB} ${SOURCE_FILES})
target_link_libraries(${GLFW_LIB} ${SDL2_LIBRARIES} ${PROJECT_NAME} ${PHYSX_LIB} ${PHYSX_COMMON_LIB} ${PHYSX_FOUNDATION_LIB} ${ASSIMP_LIB} ${FMOD_LIB} ${GLFW_LIB_LINUX} ${CMAKE_DL_LIBS} ${BOX2D_LIB_LINUX} ${FT2_LIB}) 
