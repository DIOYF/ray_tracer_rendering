# you can ignore this file because no other libs added
cmake_minimum_required( VERSION 2.8 )
set( CMAKE_CXX_FLAGS "-std=c++11" )

PROJECT(RayTracingInOneWeekend_Demo)

# add header file
include_directories(
${PROJECT_SOURCE_DIR}/./camera.h
${PROJECT_SOURCE_DIR}/./color.h
${PROJECT_SOURCE_DIR}/./material.h
${PROJECT_SOURCE_DIR}/./object.h
${PROJECT_SOURCE_DIR}/./ray.h
${PROJECT_SOURCE_DIR}/./vec3.h
${PROJECT_SOURCE_DIR}/./rtweekend.h
${PROJECT_SOURCE_DIR}/./hittable_list.h
)

# add source file
set(SRC
${PROJECT_SOURCE_DIR}/./main.cpp 
)



set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# no need to linking with other lib
set(LIB_NAME rpt_main_lib)
add_library(${LIB_NAME} STATIC ${SRC})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# executable file dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} ${SRC})

