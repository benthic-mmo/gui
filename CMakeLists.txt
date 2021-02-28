cmake_minimum_required(VERSION 3.10)
set(PROJECTNAME mmo-ui) 
project(${PROJECTNAME})

add_executable(${PROJECTNAME} src/main.cpp)


include_directories(${LUA_INCLUDE_DIR})           
include_directories(${CMAKE_SOURCE_DIR}/lib/lxgui/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/lxgui/sol2/include)

target_link_libraries(${PROJECTNAME} lxgui)
target_link_libraries(${PROJECTNAME} sol2)

