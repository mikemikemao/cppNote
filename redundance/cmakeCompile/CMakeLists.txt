cmake_minimum_required (VERSION 3.8)

project ("cppNote")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR             "${PROJECT_SOURCE_DIR}/src")
set(THIRDPARTY_LIB      "${PROJECT_SOURCE_DIR}/lib")
message(WARNING ${SIP_THIRDPARTY})

file(GLOB_RECURSE CPP_NOTE_SRC
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.h"
    "${SRC_DIR}/*.hpp"
)
#message(WARNING ${CPP_NOTE_SRC})
include_directories(
    ${SRC_DIR}
    ${SRC_DIR}/cppPrimer/generics
    ${SRC_DIR}/cppPrimer/lambda
    ${SRC_DIR}/alg/graph
    ${SRC_DIR}/app/sip
    ${SRC_DIR}/app/sip/include
    ${SRC_DIR}/app/sip/SipServer
)


#导入第三方so包，并声明为 IMPORTED 属性，指明只是想把 so 导入到项目中
add_library(
        libeXosip2
        SHARED
        IMPORTED
)
add_library(
        libosip2
        SHARED
        IMPORTED
)
add_library(
        libosipparser2
        SHARED
        IMPORTED
)


#指明 so 库的路径，CMAKE_SOURCE_DIR 表示 CMakeLists.txt 的路径
set_target_properties(
        libeXosip2
        PROPERTIES IMPORTED_LOCATION
        ${THIRDPARTY_LIB}/libeXosip2.so)
set_target_properties(
        libosip2
        PROPERTIES IMPORTED_LOCATION
        ${THIRDPARTY_LIB}/libosip2.so)
set_target_properties(
        libosipparser2
        PROPERTIES IMPORTED_LOCATION
        ${THIRDPARTY_LIB}/libosipparser2.so)

add_executable (cppNote "main.cpp" ${CPP_NOTE_SRC} )
if (WIN32)
    target_link_libraries(cppNote wsock32 ws2_32)
elseif (UNIX)
    target_link_libraries(cppNote -pthread)
else()
    message(FATAL_ERROR "Unsupported platform.")
endif()

target_link_libraries(cppNote libeXosip2 libosip2 libosipparser2)

source_group("cppNote"  FILES ${CPP_NOTE_SRC})