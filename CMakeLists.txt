cmake_minimum_required(VERSION 3.10)

project(game2d)

include_directories(vendor/glad/include vendor/glfw/include vendor/libsoundio/soundio vendor/stb vendor/zlib vendor/glm/glm)

add_subdirectory(vendor/glfw)
set(BUILD_DYNAMIC_LIBS OFF CACHE BOOL "build sound shared")
set(BUILD_EXAMPLE_PROGRAMS OFF CACHE BOOL "build sound example")
set(BUILD_TESTS OFF CACHE BOOL "build sound tests")
add_subdirectory(vendor/libsoundio)
add_subdirectory(vendor/zlib)
add_subdirectory(vendor/glm)
add_library(glad STATIC vendor/glad/src/glad.c)

add_library(game2d src/core.cc src/render.cc src/window.cc src/util.cc src/resource.cc src/resource_dec.cc src/lib.cc)
target_link_libraries(game2d glad glfw libsoundio_static zlibstatic glm_static)
if (MSVC)
    target_compile_options(game2d PRIVATE /W4)
else()
    target_compile_options(game2d PRIVATE -Wall -Wextra -Wconversion -pedantic)
endif()

add_executable(packer src/packer.cc src/resource.cc src/resource_enc.cc src/util.cc)
target_link_libraries(packer zlibstatic)

function(make_package name)
    foreach(file ${ARGN}) 
        list(APPEND package_files ${CMAKE_CURRENT_SOURCE_DIR}/${file})
    endforeach()
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.c ${CMAKE_CURRENT_BINARY_DIR}/${name}.map COMMAND packer ${name} ${package_files} DEPENDS ${package_files} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

include_directories(src/)
add_subdirectory(games)