cmake_minimum_required(VERSION 3.10)
project(DynaG-renderer)

if(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
endif()

SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

# Add source and header files
set(RENDERER_FILES 
	src/camera.cpp
	src/camera.h
	src/gWindow.cpp
	src/gWindow.h
	src/mesh.cpp
	src/mesh.h
	src/model.cpp
	src/model.h
	src/ren_api.cpp
	src/ren_api.h
	src/shader.h
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

# Add/Link resources 
file(COPY ${data} DESTINATION resources)

# Put all libraries into a variable
if (WIN32)
	set(LIBS glfw3 glad opengl32 assimp imgui )
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(LIBS gdi32)
	endif()
else()
	set(LIBS glfw glad assimp imgui)
#set(LIBS X11 Xrandr Xinerama Xxf86vm Xcursor GL dl pthread)
endif()

# Define the include DIRs
target_include_directories(${PROJECT_NAME} PUBLIC
	src
	include
)

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

