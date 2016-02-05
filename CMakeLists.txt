cmake_minimum_required(VERSION 2.8.9)
project(tankwars)

add_definitions("-std=c++14")
include_directories("ThirdParty/bullet3/src" "Source/Game" "ThirdParty/glfw3/include" "ThirdParty/include")
file(GLOB GAME_SRC "Source/Game/*.cpp")

add_subdirectory(ThirdParty/glfw3)
add_subdirectory(ThirdParty/bullet3)

add_executable(tankwars ${GAME_SRC})
target_link_libraries(tankwars glfw ${GLFW_LIBRARIES} BulletDynamics BulletCollision LinearMath)

