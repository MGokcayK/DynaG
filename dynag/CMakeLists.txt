cmake_minimum_required(VERSION 3.10)
project(DynaG)

option(BUILD_DYNAMICS "Build DynaG-dynamics API" ON)
option(BUILD_RENDERER "Build DynaG-renderer API" ON)

if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "My multi config types" FORCE)
endif()

if(BUILD_DYNAMICS)
	add_subdirectory(dynamics)
endif(BUILD_DYNAMICS)

if(BUILD_RENDERER)
	add_subdirectory(renderer)
endif(BUILD_RENDERER)