cmake_minimum_required(VERSION 2.8.12)
project(tankwars)

add_definitions("-std=c++11")
include_directories(
    "ThirdParty/gl3w/include"
    "ThirdParty/bullet3/src"
    "ThirdParty/glfw3/include"
    "ThirdParty/include")
file(GLOB GAME_SRC "Source/Game/*.cpp")
set(GL3W_SRC "ThirdParty/gl3w/src/gl3w.c")

add_subdirectory(ThirdParty/glfw3)
add_subdirectory(ThirdParty/bullet3)

add_executable(tankwars ${GAME_SRC} ${GL3W_SRC})
target_link_libraries(tankwars
    glfw
    BulletDynamics
    BulletCollision
    LinearMath
    ${GLFW_LIBRARIES})

