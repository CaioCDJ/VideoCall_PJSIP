cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(video_call LANGUAGES CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PJSIP REQUIRED libpjproject)

add_definitions(${PJSIP_CFLAGS_OTHER})
include_directories(${PJSIP_INCUDE_DIRS})
link_directories(${PJSIP_LIBRARY_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} env.h main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE 
  pjsua2
  pjsua
  pj
  pjmedia
)
