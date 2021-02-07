cmake_minimum_required(VERSION 3.12)
project(native)


IF (WIN32)
   include_directories(jni_include/inc_win)
ELSEIF (APPLE)
    include_directories(jni_include/inc_mac)
ELSEIF (UNIX)
    include_directories(jni_include/inc_linux)
ELSE()
    set(JAVA_AWT_LIBRARY NotNeeded)
    set(JAVA_JVM_LIBRARY NotNeeded)
    find_package(JNI REQUIRED)

    # 加入jni支持
    include_directories(${JAVA_INCLUDE_PATH})
    include_directories(${JAVA_INCLUDE_PATH2})
ENDIF ()


include_directories(../build/generated/sources/headers/java/main/)
add_subdirectory(lame)

#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../build/classes/java/main/io/github/mzdluo123/silk4j)

AUX_SOURCE_DIRECTORY(src SRC_DIR)

#AUX_SOURCE_DIRECTORY(test SRC_DIR)

AUX_SOURCE_DIRECTORY(bind SRC_DIR)

include_directories(src interface)

set(CMAKE_ANDROID_ARM_MODE ON)
set(CMAKE_ANDROID_ARM_NEON ON)
set(CMAKE_ANDROID_STL_TYPE system )

add_library(silk SHARED ${SRC_DIR} bind/JNIEncoder.c bind/coder.h bind/coder.c)

install(TARGETS silk lame
        CONFIGURATIONS Debug
        LIBRARY  DESTINATION  ${CMAKE_CACHEFILE_DIR}/Debug
        RUNTIME  DESTINATION  ${CMAKE_CACHEFILE_DIR}/Debug
        )
install(TARGETS silk lame
        CONFIGURATIONS Release
        LIBRARY DESTINATION ${CMAKE_CACHEFILE_DIR}/Release/
        RUNTIME DESTINATION ${CMAKE_CACHEFILE_DIR}/Release/
        )


