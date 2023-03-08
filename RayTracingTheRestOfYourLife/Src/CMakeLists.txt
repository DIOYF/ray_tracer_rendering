cmake_minimum_required( VERSION 2.8 )
set( CMAKE_CXX_FLAGS "-std=c++11" )

PROJECT(RayTracingtheNextWeek_Demo)

# add header file
include_directories(

${PROJECT_SOURCE_DIR}/./external/stb_image.h
${PROJECT_SOURCE_DIR}/./external/stb_image_write.h

${PROJECT_SOURCE_DIR}/./camera.h
${PROJECT_SOURCE_DIR}/./color.h
${PROJECT_SOURCE_DIR}/./material.h
${PROJECT_SOURCE_DIR}/./object.h
${PROJECT_SOURCE_DIR}/./ray.h
${PROJECT_SOURCE_DIR}/./vec3.h
${PROJECT_SOURCE_DIR}/./rtweekend.h
${PROJECT_SOURCE_DIR}/./hittable_list.h

${PROJECT_SOURCE_DIR}/./aabb.h
${PROJECT_SOURCE_DIR}/./bvh.h
${PROJECT_SOURCE_DIR}/./box.h
${PROJECT_SOURCE_DIR}/./constant_medium.h
${PROJECT_SOURCE_DIR}/./perlin.h
${PROJECT_SOURCE_DIR}/./rtw_stb_image.h
${PROJECT_SOURCE_DIR}/./texture.h

${PROJECT_SOURCE_DIR}/./onb.h
${PROJECT_SOURCE_DIR}/./pdf.h

)

# add source file
set(SRC
${PROJECT_SOURCE_DIR}/./main.cpp 
)



set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(LIB_NAME rpt_main_lib)
add_library(${LIB_NAME} STATIC ${SRC})

# no need to linking with other lib


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# executable file dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


add_executable(${PROJECT_NAME} ${SRC})

