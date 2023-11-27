
SET(TOOLCHAIN_PREFIX $ENV{CROSS_TOOLCHAIN})
SET(ARCH_OS $ENV{ARCH_OS})
SET(BUILD_MODE $ENV{BUILD_MODE})
SET(CMAKE_C_COMPILER_WORKS TRUE)
SET(CMAKE_CXX_COMPILER_WORKS TRUE)
SET(CURRENT_PATH ./)
SET(PRJ_PATH ${PROJECT_SOURCE_DIR}/../../)
SET(VENUS_PLATFORM $ENV{VENUS_PLATFORM})
SET(ANDROID_NDK_HOME $ENV{ANDROID_NDK_HOME})
SET(OS $ENV{OS})

#MESSAGE("PrjPath is ${PRJ_PATH} If you are not, pelase change... ")

STRING( REGEX REPLACE "/APPS/.*" "/APPS" PRJ_DIR ${CMAKE_SOURCE_DIR} )
#MESSAGE("directory Finded: " ${PRJ_DIR} )


# specify the cross compiler
# the name of the target operating system
#SET(CMAKE_SYSTEM_NAME Generic)

IF (VENUS_PLATFORM STREQUAL H5)
	SET(ARCH H5)
	SET(OS   LINUX)
	SET(INSTALL_PATH $ENV{VIS_PREFIX}/H5)
	SET(GXCFLAGS, "")
	SET(GXCFLAGS, "Werror")
ELSEIF (VENUS_PLATFORM STREQUAL R0)
	SET(ARCH R0)
	SET(OS   LINUX)
	SET(INSTALL_PATH $ENV{VIS_PREFIX}/R0)
	SET(GXCFLAGS, "")
	SET(GXCFLAGS, "Werror")	
ELSEIF (VENUS_PLATFORM STREQUAL M0)
	SET(ARCH M0)
	SET(OS   LINUX)
	SET(INSTALL_PATH $ENV{VIS_PREFIX}/M0)
	SET(GXCFLAGS, "")
	SET(GXCFLAGS, "Werror")
ELSEIF (VENUS_PLATFORM STREQUAL R2)
	SET(ARCH R2)
	SET(OS   ANDROID)
	SET(INSTALL_PATH $ENV{VIS_PREFIX}/R2)
	SET(GXCFLAGS, "")
	SET(GXCFLAGS, "Werror")
ELSEIF (VENUS_PLATFORM STREQUAL A2)
	SET(ARCH A2)
	SET(OS   ANDROID)
	SET(INSTALL_PATH $ENV{VIS_PREFIX}/A2_32)
	SET(GXCFLAGS, "")
	SET(GXCFLAGS, "Werror")
ELSE()
	SET(ARCH x86)
	SET(OS   LINUX)
	SET(INSTALL_PATH $ENV{VIS_PREFIX}/x86-linux)
ENDIF()

IF  (${OS} STREQUAL ANDROID)
	#SET(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}clang)
	#SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}clang++)
	ADD_DEFINITIONS(-D${OS})
ELSE()
	SET(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
	SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)	
ENDIF()
#ADD_DEFINITIONS(-D${ARCH} -D${OS}_OS)

# adjust the default behaviour of hte FIND_XXX() NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODULE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_INCLUDE ONLY)


SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GXCFLAGS} -Wall -Wundef -funsigned-char -fdata-sections -ffunction-sections")

SET( CMAKE_CXX_FLAGS_DEBUG   "$ENV{CXXFLAGS} -g -O0 -Wall -Wundef -funsigned-char -fdata-sections -ffunction-sections")
SET( CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall -Wundef -funsigned-char -fdata-sections -ffunction-sections")

SET( CMAKE_C_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG}" )
SET( CMAKE_C_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE}" )


FUNCTION(ADD_VIS_LIBRARY name)
	ADD_LIBRARY( ${name} ${ARGN} )
	INSTALL( TARGETS ${name} DESTINATION ${INSTALL_PATH}/lib )
ENDFUNCTION(ADD_VIS_LIBRARY name)

FUNCTION(ADD_THIRD_LIBRARY name)
	ADD_LIBRARY( ${name} ${ARGN} )
	INSTALL( TARGETS ${name} DESTINATION ${PRJ_PATH}/APPS/lib/lib_${VENUS_PLATFORM})
ENDFUNCTION(ADD_THIRD_LIBRARY name)
