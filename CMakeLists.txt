cmake_minimum_required(VERSION 3.10)
set(PROJECTNAME mmo-ui)
project(${PROJECTNAME})

add_executable(${PROJECTNAME} src/main.cpp)


find_package(Lua REQUIRED)

find_package(SFML COMPONENTS system window graphics REQUIRED)

include_directories(${LUA_INCLUDE_DIR})

# SFML wasn't setting the include dir for itself, even though it shoud...
# https://en.sfml-dev.org/forums/index.php?topic=24070.0
include_directories(/usr/local/include)

include_directories(${CMAKE_SOURCE_DIR}/lib/lxgui/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/lxgui/sol2/include)

add_subdirectory(lib/lxgui)

target_link_libraries(${PROJECTNAME} lxgui lxgui-luapp lxgui-sfml lxgui-utils)
target_link_libraries(${PROJECTNAME} ${LUA_LIBRARIES})
target_link_libraries(${PROJECTNAME} sfml-system sfml-window sfml-graphics)

