cmake_minimum_required(VERSION 3.0)
project(talk)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wuninitialized -Winit-self -Wmaybe-uninitialized")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

aux_source_directory(./src TALK_SRC)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/apps/main.cpp ${TALK_SRC})