cmake_minimum_required(VERSION 2.8)

project(CGoGNRelease)

SET(CMAKE_BUILD_TYPE Release)

IF(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib)
ELSE(WIN32)
	SET(LIBRARY_OUTPUT_PATH ${CGoGN_ROOT_DIR}/lib/Release)
	ADD_DEFINITIONS(-DNOTOPOWARNING)
ENDIF(WIN32)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CGoGN_ROOT_DIR}/include
	${CGoGN_EXT_INCLUDES}
)

file(
	GLOB_RECURSE
	files_topology
	${CGoGN_ROOT_DIR}/src/Topology/*.cpp
	${CGoGN_ROOT_DIR}/include/Topology/*.hpp
	${CGoGN_ROOT_DIR}/include/Topology/*.h
)

file(
	GLOB_RECURSE
	files_container
	${CGoGN_ROOT_DIR}/src/Container/*.cpp
	${CGoGN_ROOT_DIR}/include/Container/*.hpp
	${CGoGN_ROOT_DIR}/include/Container/*.h
)

file(
	GLOB_RECURSE
	files_algo
	${CGoGN_ROOT_DIR}/src/Algo/*.cpp
	${CGoGN_ROOT_DIR}/src/Algo/*.c
	${CGoGN_ROOT_DIR}/include/Algo/*.hpp
	${CGoGN_ROOT_DIR}/include/Algo/*.h
)


file(
#	GLOB_RECURSE
	GLOB
	shaders_src
	${CGoGN_ROOT_DIR}/include/Utils/*.frag
	${CGoGN_ROOT_DIR}/include/Utils/*.vert
	${CGoGN_ROOT_DIR}/include/Utils/*.geom	
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.frag
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.vert
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.geom
)


IF(WIN32)
	add_custom_target(shader_target ${CGoGN_ROOT_DIR}/ThirdParty/bin/Release/shader_to_h ${shaders_src}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	SOURCES ${shaders_src} )
ELSE(WIN32)
	add_custom_target(shader_target ${CGoGN_ROOT_DIR}/ThirdParty/bin/shader_to_h ${shaders_src}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	SOURCES ${shaders_src} )
ENDIF(WIN32)

file(
	GLOB		# WARNING NO MORE RECURSE TO AVOID TAKING QT FILES
	files_utils
	${CGoGN_ROOT_DIR}/src/Utils/*.cpp
	${CGoGN_ROOT_DIR}/include/Utils/*.hpp
	${CGoGN_ROOT_DIR}/include/Utils/*.h

	${CGoGN_ROOT_DIR}/src/Utils/Shaders/*.cpp
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.hpp
	${CGoGN_ROOT_DIR}/include/Utils/Shaders/*.h
)

IF(WITH_QT)

	file(
		GLOB
		files_utils_qt
		${CGoGN_ROOT_DIR}/src/Utils/Qt/*.cpp
		${CGoGN_ROOT_DIR}/include/Utils/Qt/*.hpp
		${CGoGN_ROOT_DIR}/include/Utils/Qt/*.h )
	file(
		GLOB
		utils_qt_headers
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qtgl.h
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qtSimple.h
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qtpopup.h 
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qthistodraw.h 
		${CGoGN_ROOT_DIR}/include/Utils/Qt/qtcolorschooser.h )	
		
	QT4_WRAP_CPP(UTILS_QT_HEADERS_MOC ${utils_qt_headers})
	SET (files_utils_withQt ${files_utils} ${files_utils_qt} ${UTILS_QT_HEADERS_MOC})
ENDIF (WITH_QT)




IF (ONELIB)
	IF(WITH_QT)
		add_library( cgogn ${files_topology} ${files_container} ${files_algo} ${files_utils_withQt}) 
	ELSE(WITH_QT)
		add_library( cgogn ${files_topology} ${files_container} ${files_algo} ${files_utils} )
	ENDIF (WITH_QT)
	add_dependencies(cgogn shader_target )
ELSE (ONELIB)
	add_library( container	${files_container})
	add_library( topology	${files_topology})
	add_library( algo		${files_algo})
	IF(WITH_QT)
		add_library( utils ${files_utils_withQt} )
	ELSE(WITH_QT)
		add_library( utils ${files_utils} )
	ENDIF (WITH_QT)
	add_dependencies(utils shader_target )
ENDIF (ONELIB)
