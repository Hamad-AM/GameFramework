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
set(DONUT_SHADERS_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/framework")

add_subdirectory(donut)

set(LIB_DIR "${CMAKE_SORUCE_DIR}/libraries")

add_subdirectory(libraries/glfw)
add_subdirectory(libraries/glm)
add_subdirectory(libraries/stb_image)
add_subdirectory(libraries/box2d)
add_subdirectory(libraries/)
add_subdirectory(libraries/)
add_subdirectory(libraries/)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/anthology.cpp)

file (GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

add_executable(Anthology WIN32 ${SOURCE_FILES})
target_link_libraries(Anthology donut_render donut_engine)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /MP")
endif()


set(LIB_DIR "${CMAKE_SOURCE_DIR}/libraries")

if(unix)
set(OpenGL_GL_PREFERENCE GLVND)
endif(unix)

find_package(OpenGL REQUIRED)
# find_package(SDL2 REQUIRED)
        

# set(SDL2_DIR "${LIB_DIR}/SDL2-2.0.16")
set(GLAD_DIR "${LIB_DIR}/glad")
set(GLM_DIR  "${LIB_DIR}/glm")
set(GLFW_DIR "${LIB_DIR}/glfw")
set(STB_IMAGE_DIR "${LIB_DIR}/stb_image")
set(PX_DIR "${LIB_DIR}/PhysX")
set(RAYLIB_DIR "${LIB_DIR}/raylib")
set(BOX2D_DIR "${LIB_DIR}/box2d")
set(FT2_DIR "${LIB_DIR}/freetype-2.12.1")
set(FMOD_DIR "${LIB_DIR}/fmod/api/core")

# INCLUDE DIRECTORS

set(GLFW_INCLUDE "${GLFW_DIR}/include/")
set(GLAD_INCLUDE "${GLAD_DIR}/include/")
set(BOX2D_INCLUDE "${BOX2D_DIR}/include/")
set(STB_IMAGE_INCLUDE "${STB_IMAGE_DIR}/")
set(GLM_INCLUDE "${GLM_DIR}/")
set(FT2_INCLUDE "${FT2_DIR}/include/")
set(PX_INCLUDE "${PX_DIR}/physx/include/")
set(PX_SHARED_INCLUDE "${PX_DIR}/pxshared/include/")
set(RAYLIB_INCLUDE "${RAYLIB_DIR}/src/")
set(FMOD_INCLUDE "${FMOD_DIR}/inc/")
# set(SDL2_INCLUDE "${SDL2_DIR}/include")

*set(GLAD_LIB "${GLAD_DIR}/src/glad.c")
set(STB_IMAGE_LIB "${STB_IMAGE_DIR}/stb_image.cpp")


if(WIN32)
        set(GLFW_LIB "${GLFW_DIR}/build/src/Debug/glfw3.lib")
        set(BOX2D_LIB "${BOX2D_DIR}/build/bin/Debug/box2d.lib")
        set(FT2_LIB "${FT2_DIR}/objs/freetype.lib")
        set(RAYLIB_LIB "${RAYLIB_DIR}/build/raylib/RelWithDebInfo/raylib.lib")

        # PHYSX
        set(PX_LIB "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/PhysXExtensions_static_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/PhysXFoundation_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/PhysXPvdSDK_static_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/PhysXTask_static_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/PhysXVehicle_static_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/SceneQuery_static_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/SimulationController_static_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/LowLevel_static_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/LowLevelAABB_static_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/LowLevelDynamics_static_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/PhysX_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/PhysXCharacterKinematic_static_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/PhysXCommon_64.lib"
        "${PX_DIR}/physx/bin/win.x86_64.vc142.mt/checked/PhysXCooking_64.lib")
endif(WIN32)

if(UNIX)
        set(GLFW_LIB "${GLFW_DIR}/build/src/libglfw3.a")
        set(FT2_LIB "${FT2_DIR}/build/libfreetype.a")
        set(BOX2D_LIB "${LIB_DIR}/box2d/build/bin/libbox2d.a")

        set (FMODL_LIB "${FMOD_DIR}/lib/x86_64/libfmodL.so")
        set (FMOD_LIB "${FMOD_DIR}/lib/x86_64/libfmod.so")
        set (FMOD13_LIB "${FMOD_DIR}/lib/x86_64/libfmod.so.13")
endif(UNIX)




# set(ASSIMP_LIB "${ASSIMP_DIR}/lib/Release/assimp-vc142-mt.lib")

# set(FMOD_LIB "${FMOD_DIR}/lib/x64/fmod_vc.lib")
# set(FMOD_INCLUDE "${FMOD_DIR}/inc")

# set(PHYSX_INCLUDE "${PHYSX_DIR}/inlude")
# set(PHYSX_COMMON_LIB "${PHYSX_DIR}/bin/win.x86_64.vc142.mt/checked/PhysXCommon_64.lib")
# set(PHYSX_LIB "${PHYSX_DIR}/bin/win.x86_64.vc142.mt/checked/PhysX_64.lib")
# set(PHYSX_FOUNDATION_LIB "${PHYSX_DIR}/bin/win.x86_64.vc142.mt/checked/PhysXFoundation_64.lib")


# add_library(AnthologyLib )

include_directories(${GLFW_INCLUDE} ${GLAD_INCLUDE} ${BOX2D_INCLUDE} ${FT2_INCLUDE} ${PX_INCLUDE} ${GLM_INCLUDE} ${PX_SHARED_INCLUDE} ${STB_IMAGE_INCLUDE} ${RAYLIB_INCLUDE} ${FMOD_INCLUDE})

add_executable(Anthology ${SOURCE_FILES} ${GLAD_LIB} ${STB_IMAGE_LIB} ${HEADER_FILES})

if(WIN32)
        set_target_properties(Anthology PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
endif(WIN32)

if(WIN32)
        target_link_libraries(Anthology "${GLFW_LIB}" "${BOX2D_LIB}" "${FT2_LIB}")
endif(WIN32)

if(UNIX)
        target_link_libraries(Anthology ${GLFW_LIB} ${BOX2D_LIB} ${FT2_LIB} ${CMAKE_DL_LIBS} ${FMOD_LIB} ${FMODL_LIB} ${FMOD13_LIB})
endif(UNIX)
#"${PX_LIB}" "${RAYLIB_LIB}" 
