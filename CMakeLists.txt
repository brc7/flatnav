CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)

PROJECT( FlatNav CXX )

SET( CMAKE_CXX_STANDARD 11 )
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Ofast -DHAVE_CXX0X -DNDEBUG -openmp -fpic -w -ffast-math -funroll-loops -ftree-vectorize -g" )

INCLUDE( ExternalProject )

FIND_PACKAGE( Git REQUIRED )

OPTION( USE_GIT_PROTOCOL "If behind a firewall turn this off to use https instead." OFF )

function(functionInstallExternalCMakeProject ep_name)
  ExternalProject_Get_Property(${ep_name} binary_dir)
  install(SCRIPT ${binary_dir}/cmake_install.cmake)
endfunction()

ExternalProject_Add( 
  ZLIB
  DEPENDS ""
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG v1.2.11
  SOURCE_DIR ZLIB-source
  BINARY_DIR ZLIB-build
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  # INSTALL_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:STRING=${PROJECT_BINARY_DIR}/ep
    -DINSTALL_BIN_DIR:STRING=${PROJECT_BINARY_DIR}/ep/bin
    -DINSTALL_INC_DIR:STRING=${PROJECT_BINARY_DIR}/ep/include
    -DINSTALL_LIB_DIR:STRING=${PROJECT_BINARY_DIR}/ep/lib
    -DINSTALL_MAN_DIR:STRING=${PROJECT_BINARY_DIR}/ep/share/man
    -DINSTALL_PKGCONFIG_DIR:STRING=${PROJECT_BINARY_DIR}/ep/share/pkgconfig
    -DCMAKE_BUILD_TYPE:STRING=Release
)
functionInstallExternalCMakeProject(ZLIB)

SET( ZLIB_LIB_DEBUG ${PROJECT_BINARY_DIR}/ep/lib/libz.a )
SET( ZLIB_LIB_RELEASE ${PROJECT_BINARY_DIR}/ep/lib/libz.a )

ExternalProject_Add( 
  CNPY
  DEPENDS ZLIB
  GIT_REPOSITORY https://github.com/sarthakpati/cnpy.git
  # GIT_TAG v1.2.11
  SOURCE_DIR CNPY-source
  BINARY_DIR CNPY-build
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  # INSTALL_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DZLIB_INCLUDE_DIR:STRING=${PROJECT_BINARY_DIR}/ep/include
    -DZLIB_LIBRARY_DEBUG:STRING=${ZLIB_LIB_DEBUG}
    -DZLIB_LIBRARY_RELEASE:STRING=${ZLIB_LIB_RELEASE}
    -DCMAKE_INSTALL_PREFIX:STRING=${PROJECT_BINARY_DIR}/ep
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS} 
    -DCMAKE_BUILD_TYPE:STRING=Release
)
functionInstallExternalCMakeProject(CNPY)

INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR}/ep/include )

SET( CNPY_LIB ${PROJECT_BINARY_DIR}/ep/lib/libcnpy.a )

FILE( GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/flatnav/*.h" )

ADD_LIBRARY( FLAT_NAV_LIB ${HEADERS} ${PROJECT_SOURCE_DIR}/tools/construct_npy.cpp )
ADD_DEPENDENCIES( FLAT_NAV_LIB CNPY )

TARGET_LINK_LIBRARIES( FLAT_NAV_LIB ${CNPY_LIB} )
set_target_properties( FLAT_NAV_LIB PROPERTIES LINKER_LANGUAGE CXX)

foreach(CONSTRUCT_EXEC construct_npy query_npy reorder_npy construct_float32 reorder_float32 construct_uint8)
  ADD_EXECUTABLE( ${CONSTRUCT_EXEC} ${PROJECT_SOURCE_DIR}/tools/${CONSTRUCT_EXEC}.cpp )
  ADD_DEPENDENCIES( ${CONSTRUCT_EXEC} FLAT_NAV_LIB )
  TARGET_LINK_LIBRARIES( 
    ${CONSTRUCT_EXEC} 
    FLAT_NAV_LIB 
    ${CNPY_LIB} 
    ${ZLIB_LIB_RELEASE} )
  INSTALL( TARGETS ${CONSTRUCT_EXEC} DESTINATION bin )
endforeach(CONSTRUCT_EXEC)
