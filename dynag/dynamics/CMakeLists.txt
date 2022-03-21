cmake_minimum_required(VERSION 3.10)
project(DynaG-dynamics)

# Add .lib files
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)

if(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
endif()

SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})


# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

# Configure assets header file
include_directories(${CMAKE_BINARY_DIR}/src)
	
# Define the executable/shared object or dynamic library
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
#add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

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
else ()
	set (CMAKE_CXX_FLAGS "-g -rdynamic")
	#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lunwind")
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

# Define the include DIRs
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

#######################################
# LOOK for the packages that we need! #
#######################################

# YAML
find_package(YAML REQUIRED)
message(STATUS "Found YAML in ${YAML_INCLUDE_DIR}")

# STB
find_package(STB REQUIRED)
message(STATUS "Found STB in ${STB_INCLUDE_DIR}")

# DbgHelp
find_package(DBGHELP REQUIRED)

# Put all libraries into a variable
if (WIN32)
	set(LIBS yaml-cpp dbghelp)
else()
	set(LIBS yaml-cpp )
endif()

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})

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
