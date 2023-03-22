cmake_minimum_required(VERSION 3.10)
project(DynaG-renderer)

# if(NOT CMAKE_BUILD_TYPE)
# 	SET(CMAKE_BUILD_TYPE "Release")
# endif()

SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

# Add source and header files
set(RENDERER_FILES 
	src/ren_api.cpp
	src/ren_api.h
    src/base/app.cpp
    src/base/app.h
    src/base/camera.cpp
    src/base/camera.h
    src/base/includes.h
    src/base/images.cpp
    src/base/images.h
    src/base/model.cpp
    src/base/model.h
    src/base/window.cpp
    src/base/window.h
	src/gl/gl_app.cpp
	src/gl/gl_app.h
	src/gl/gl_window.cpp
	src/gl/gl_window.h
    src/gl/gl_mesh.cpp
    src/gl/gl_mesh.h
    src/gl/gl_model.cpp
	src/gl/gl_model.h
	src/gl/gl_shader.h
	src/vk/vk_app.cpp
	src/vk/vk_app.h
	src/vk/vk_buffer.cpp
	src/vk/vk_buffer.h
	src/vk/vk_descriptor.cpp
	src/vk/vk_descriptor.h
	src/vk/vk_device.cpp
	src/vk/vk_device.h
	src/vk/vk_material.cpp
	src/vk/vk_material.h
	src/vk/vk_model.cpp
	src/vk/vk_model.h
	src/vk/vk_pipeline.cpp
	src/vk/vk_pipeline.h
	src/vk/vk_renderer.cpp
	src/vk/vk_renderer.h
	src/vk/vk_swap_chain.cpp
	src/vk/vk_swap_chain.h
	src/vk/vk_texture.cpp
	src/vk/vk_texture.h
	src/vk/vk_utils.cpp
	src/vk/vk_utils.h
	src/vk/vk_window.cpp
	src/vk/vk_window.h
)
	
# Define the executable/shared object or dynamic library
add_library(${PROJECT_NAME} SHARED ${RENDERER_FILES})

# Set C++ version as 17
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Add .lib files
target_link_directories(${PROJECT_NAME} PUBLIC libs)

#target_include_directories(${PROJECT_NAME} PUBLIC src)

if (WIN32)	
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
	endif()
	if(MSVC)
		if (MSVC_VERSION GREATER_EQUAL 1910) #msvc version of visual studio 2017 or higher
			set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG:FASTLINK")
		else()
			set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG")
		endif()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
	endif()
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin
	EXECUTABLE_OUTPUT_PATH   ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# We need a CMAKE_DIR with some code to find external dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

#######################################
# LOOK for the packages that we need! #
#######################################

# GLFW
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

# GLM
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

# OpenGL
find_package(OpenGL REQUIRED)

# ASSIMP
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

# STB
find_package(STB REQUIRED)
message(STATUS "Found STB in ${STB_INCLUDE_DIR}")

# GLAD
set(GLAD_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c"
)    

add_library(glad ${GLAD_FILES})

target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(glad
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/libs"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/libs"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/libs"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/libs"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/libs"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/libs"
	EXECUTABLE_OUTPUT_PATH   "${CMAKE_CURRENT_SOURCE_DIR}/libs"
)

# IMGUI
# Set where the ImGui files are stored
set(IMGUI_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/include/imgui")
    
# Compile as static library 
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 

add_library(imgui ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC ${IMGUI_PATH} ${GLFW3_INCLUDE_DIR})

set_target_properties(imgui
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/libs"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/libs"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/libs"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/libs"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/libs"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/libs"
	EXECUTABLE_OUTPUT_PATH   "${CMAKE_CURRENT_SOURCE_DIR}/libs"
)

# AUTO_LOCATE_VULKAN - accepted value ON or OFF
# ON  - Use CMake to auto locate the Vulkan SDK.
# OFF - Vulkan SDK path can be specified manually. This is helpful to test the build on various Vulkan version.
option(AUTO_LOCATE_VULKAN "AUTO_LOCATE_VULKAN" ON)

if(AUTO_LOCATE_VULKAN)
	message(STATUS "Attempting auto locate Vulkan using CMake......")
	
	# Find Vulkan Path using CMake's Vulkan Module
	# This will return Boolean 'Vulkan_FOUND' indicating the status of find as success(ON) or fail(OFF).
	# Include directory path - 'Vulkan_INCLUDE_DIRS' and 'Vulkan_LIBRARY' with required libraries.
	find_package(Vulkan)
	
	# Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
	if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
		set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
		STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
	endif()
		 
	if(NOT Vulkan_FOUND)
		# CMake may fail to locate the libraries but could be able to 
		# provide some path in Vulkan SDK include directory variable
		# 'Vulkan_INCLUDE_DIRS', try to extract path from this.
		message(STATUS "Failed to locate Vulkan SDK, retrying again...")
		if(EXISTS "${VULKAN_PATH}")
			message(STATUS "Successfully located the Vulkan SDK: ${VULKAN_PATH}")
		else()
			message("Error: Unable to locate Vulkan SDK. Please turn off auto locate option by specifying 'AUTO_LOCATE_VULKAN' as 'OFF'")
			message("and specify manually path using 'VULKAN_SDK' and 'VULKAN_VERSION' variables in the CMakeLists.txt.")
			return()
		endif()
	endif()
else()
	message(STATUS "Attempting to locate Vulkan SDK using manual path......")
	set(VULKAN_SDK "C:/VulkanSDK")
	set(VULKAN_VERSION "1.3.236.0")
	set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")
	message(STATUS "Using manual specified path: ${VULKAN_PATH}")

	# Check if manual set path exists
	if(NOT EXISTS "${VULKAN_PATH}")
		message("Error: Unable to locate this Vulkan SDK path VULKAN_PATH: ${VULKAN_PATH}, please specify correct path.")
	   return()
	endif()
endif()

# Add/Link resources 
file(COPY ${data} DESTINATION resources)

# Put all libraries into a variable
if (WIN32)
	set(LIBS glfw3 glad opengl32 assimp imgui "vulkan-1")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(LIBS gdi32)
	endif()
else()
	set(LIBS glfw glad assimp imgui "vulkan-1")
#set(LIBS X11 Xrandr Xinerama Xxf86vm Xcursor GL dl pthread)
endif()

# Define the include DIRs
target_include_directories(${PROJECT_NAME} PUBLIC
	src
	include
    ${VULKAN_PATH}/Include
)

# Link directory for vulkan-1
target_link_directories(${PROJECT_NAME} PUBLIC ${VULKAN_PATH}/Bin ${VULKAN_PATH}/Lib )

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})
add_dependencies(${PROJECT_NAME} glad imgui)

# Create virtual folders to make it look nicer in VS
if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources(src)
endif()

