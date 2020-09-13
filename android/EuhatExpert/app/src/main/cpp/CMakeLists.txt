# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#set(CMAKE_CXX_FLAGS "-D__ANDROID_API__=29")
#set(CMAKE_CXX_FLAGS "-D_DEBUG")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wall -v -Wl,--no-warn-shared-textrel")
SET(EUHAT_SRC_DIR ../../../../../../euhat)

file(GLOB SqliteSrcs
        ${EUHAT_SRC_DIR}/sqlite/sqlite3euhat.c
        )

file(GLOB EuhatDbOp
        ${EUHAT_SRC_DIR}/dbop/*.cpp
        )

file(GLOB EuhatOpenSSL
        ${EUHAT_SRC_DIR}/openssl/*.c
        ${EUHAT_SRC_DIR}/openssl/bn/*.c)

file(GLOB EuhatCommon
        ${EUHAT_SRC_DIR}/common/*.cpp
        )

file(GLOB EuhatOs
        ${EUHAT_SRC_DIR}/os/android/*.cpp
        ${EUHAT_SRC_DIR}/os/*.cpp
        )

file(GLOB EuhatApp
        ${EUHAT_SRC_DIR}/app/FileMan/server/*.cpp
        ${EUHAT_SRC_DIR}/app/FileMan/common/*.cpp
        )

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             native-lib.cpp
             macHdd.cpp
            ${SqliteSrcs}
            ${EuhatCommon}
            ${EuhatOs}
            ${EuhatApp}
            ${EuhatDbOp}
            ${EuhatOpenSSL}
        )

target_include_directories(native-lib
            PRIVATE
            ${CMAKE_SOURCE_DIR}/${EUHAT_SRC_DIR}/os/android
            ${CMAKE_SOURCE_DIR}/${EUHAT_SRC_DIR}
        )

add_library(OpenSSL STATIC IMPORTED)

#set_target_properties(OpenSSL
#            PROPERTIES IMPORTED_LOCATION
#            ${CMAKE_SOURCE_DIR}/jniLibs-16/${CMAKE_ANDROID_ARCH_ABI}/libcrypto.a)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib
                       #OpenSSL

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )