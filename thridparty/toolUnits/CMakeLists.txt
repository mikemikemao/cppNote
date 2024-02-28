project(ToolUnits)
cmake_minimum_required(VERSION 3.1.3)
#这里会include本地的宏函数和函数，如果本地找不到，那就是cmake系统内置的宏函数和函数
include(CheckStructHasMember)
include(CheckSymbolExists)
#APPEND添加新元素到列表中
#CMAKE_REQUIRED_DEFINITIONS 设置预编译宏
list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
check_struct_has_member("struct mmsghdr" msg_hdr sys/socket.h HAVE_MMSG_HDR)
#这个函数检测文件socket.h中是否定义了符号sendmmsg。如果定义了，设置变量HAVE_SENDMMSG_API。
check_symbol_exists(sendmmsg sys/socket.h HAVE_SENDMMSG_API)
check_symbol_exists(recvmmsg sys/socket.h HAVE_RECVMMSG_API)
if(HAVE_MMSG_HDR)
    #添加宏定义
    add_definitions(-DHAVE_MMSG_HDR)
endif()
if(HAVE_SENDMMSG_API)
    add_definitions(-DHAVE_SENDMMSG_API)
endif()
if(HAVE_RECVMMSG_API)
    add_definitions(-DHAVE_RECVMMSG_API)
endif()

