# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("rtspc")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#add_library( # Sets the name of the library.
             #native-lib

             # Sets the library as a shared library.
             #SHARED

             # Provides a relative path to your source file(s).
             #native-lib.cpp )


# 添加可执行文件
add_executable(rtspc native-lib.cpp)