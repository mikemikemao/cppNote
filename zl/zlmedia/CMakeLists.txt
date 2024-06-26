cmake_minimum_required(VERSION 3.1.3)

# 加载自定义模块
# Load custom modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(ZLMediaKit LANGUAGES C CXX)

# 使能 C++11
# Enable C++11
set(CMAKE_CXX_STANDARD 11)

option(ENABLE_SERVER "Enable Server" ON)
option(ENABLE_SERVER_LIB "Enable server as android static library" OFF)

function(update_cached name value)
  set("${name}" "${value}" CACHE INTERNAL "*** Internal ***" FORCE)
endfunction()

function(update_cached_list name)
  set(_tmp_list "${${name}}")
  list(APPEND _tmp_list "${ARGN}")
  list(REMOVE_DUPLICATES _tmp_list)
  update_cached(${name} "${_tmp_list}")
endfunction()

add_subdirectory(3rdpart)

if(ENABLE_SERVER)
  add_subdirectory(server)
endif()

