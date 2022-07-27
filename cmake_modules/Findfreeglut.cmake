set( FREEGLUT_HOME ${OGLW_DEPS_ROOT}/freeglut-3.2.1 )

set( freeglut_INCLUDE_DIR ${FREEGLUT_HOME}/include )
set( ENV{freeglut_INCLUDE_DIR} ${freeglut_INCLUDE_DIR} )

set( freeglut_LIBRARY_TEMP freeglut )
set( freeglut_LIBRARY freeglut} )

message( "FREEGLUT DEBUG: freeglut_INCLUDE_DIR: ${freeglut_INCLUDE_DIR}" )

set( freeglut_FOUND true )