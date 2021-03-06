cmake_minimum_required(VERSION 2.6)

project(Numerical)


SUBDIRS(C_BLAS_LAPACK)

include_directories(METIS UFconfig AMD CAMD COLAMD CCOLAMD CHOLMOD UMFPACK)

# specific mac
IF(APPLE)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_MACOSX -ObjC++ -D__PLACEMENT_NEW_INLINE")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_MACOSX -O3 -fno-strict-aliasing")
	SET(PATHSDK "/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks" CACHE STRING "SDK framework")
ELSE(APPLE)
	IF(UNIX)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fno-strict-aliasing -D_UNIX -D__PLACEMENT_NEW_INLINE ")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fno-strict-aliasing -D_UNIX")
	ENDIF(UNIX)
ENDIF(APPLE)

ADD_DEFINITIONS(-DNSUPERNODAL)

AUX_SOURCE_DIRECTORY(METIS SUB_SOURCES)
AUX_SOURCE_DIRECTORY(AMD SUB_SOURCES)
AUX_SOURCE_DIRECTORY(CAMD SUB_SOURCES)
AUX_SOURCE_DIRECTORY(COLAMD SUB_SOURCES)
AUX_SOURCE_DIRECTORY(CCOLAMD SUB_SOURCES)
AUX_SOURCE_DIRECTORY(CHOLMOD SUB_SOURCES)
AUX_SOURCE_DIRECTORY(UMFPACK SUB_SOURCES)

SET(SOURCES ${SOURCES} ${SUB_SOURCES})

ADD_LIBRARY(numerical ${SOURCES})

