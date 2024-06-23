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


if(ENABLE_SERVER)
  add_subdirectory(server)
endif()