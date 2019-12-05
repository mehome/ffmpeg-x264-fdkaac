# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(MAIN_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-varargs")
set(FFMPEG_LIB_PATH ${MAIN_PATH}/FFDecoder/src/ffmpeg/libs/${ANDROID_ABI})
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
message(${ANDROID_ABI})

include_directories(${MAIN_PATH}/FFDecoder/inc)
include_directories(${MAIN_PATH}/FFDecoder/src)
include_directories(${MAIN_PATH}/FFDecoder/src/ffmpeg/include)

file(GLOB CPP_FFDECOCDER                 ${MAIN_PATH}/FFDecoder/src/*.cpp)

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED
        ${CPP_FFDECOCDER}

        # Provides a relative path to your source file(s).
        native-lib.cpp)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib


        ${FFMPEG_LIB_PATH}/libavformat.a
        ${FFMPEG_LIB_PATH}/libavcodec.a
        ${FFMPEG_LIB_PATH}/libavresample.a
        ${FFMPEG_LIB_PATH}/libswresample.a
        ${FFMPEG_LIB_PATH}/libswscale.a
        ${FFMPEG_LIB_PATH}/libavutil.a

        ${FFMPEG_LIB_PATH}/libx264.a
        ${FFMPEG_LIB_PATH}/libfdk-aac.a

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        z m
        )