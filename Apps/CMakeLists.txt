SET(EXECUTABLE_OUTPUT_PATH ${CGoGN_ROOT_DIR}/bin)

# FOR Qt4
#SET(QT_USE_QTOPENGL TRUE)
#INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})

IF (ONELIB)
	SET(CGoGN_LIBS_R cgogn)
ELSE (ONELIB)
	SET(CGoGN_LIBS_R topology algo container utils)
ENDIF (ONELIB)

IF(WIN32)
	# libs have same name but in different place in Visual
	IF (ONELIB)
		SET(CGoGN_LIBS_D cgogn)
	ELSE (ONELIB)
		SET(CGoGN_LIBS_D topology algo container utils)
	ENDIF (ONELIB)
ELSE(WIN32)
	IF (ONELIB)
		SET(CGoGN_LIBS_D cgognD)
	ELSE (ONELIB)
		SET(CGoGN_LIBS_D topologyD algoD containerD utilsD)
	ENDIF (ONELIB)

	IF (WITH_QT)
		add_subdirectory(Examples/Debug)
	ENDIF (WITH_QT)
ENDIF(WIN32)

IF (WITH_QT)
	add_subdirectory(Tuto)
	add_subdirectory(Examples/Release)
ENDIF (WITH_QT)

add_subdirectory(Examples/Tests)

