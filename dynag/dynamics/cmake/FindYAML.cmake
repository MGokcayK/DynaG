# - Try to find yaml-cpp
# Once done, this will define
#
# YAML_FOUND - system has yaml-cpp
# YAML_INCLUDE_DIR - the yaml-cpp include directories
# YAML_LIBRARIES - link these to use yaml-cpp
FIND_PATH( YAML_INCLUDE_DIR yaml-cpp/yaml.h
	/usr/include
	/usr/local/include
	/opt/local/include
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
FIND_LIBRARY( YAML_LIBRARY yaml-cpp
	/usr/lib64
	/usr/lib
	/usr/local/lib
	/opt/local/lib
	"${CMAKE_CURRENT_SOURCE_DIR}/libs"
)
IF(YAML_INCLUDE_DIR AND YAML_LIBRARY)
	SET( YAML_FOUND TRUE )
	SET( YAML_LIBRARIES ${YAML_LIBRARY} )
ENDIF(YAML_INCLUDE_DIR AND YAML_LIBRARY)
IF(YAML_FOUND)
	IF(NOT YAML_FIND_QUIETLY)
	MESSAGE(STATUS "Found YAML: ${YAML_LIBRARY}")
	ENDIF(NOT YAML_FIND_QUIETLY)
ELSE(YAML_FOUND)
	IF(YAML_FIND_REQUIRED)
	MESSAGE(FATAL_ERROR "Could not find libYAML")
	ENDIF(YAML_FIND_REQUIRED)
ENDIF(YAML_FOUND)