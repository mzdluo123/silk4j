cmake_minimum_required(VERSION 3.12)
project(native)

set(CMAKE_CXX_STANDARD 14)

#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../build/classes/java/main/io/github/mzdluo123/silk4j)

AUX_SOURCE_DIRECTORY(src SRC_DIR)

#AUX_SOURCE_DIRECTORY(test SRC_DIR)

AUX_SOURCE_DIRECTORY(bind SRC_DIR)

include_directories(src interface)

include_directories(../build/generated/sources/headers/java/main/)

set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_JVM_LIBRARY NotNeeded)
find_package(JNI REQUIRED)


# 加入jni支持
include_directories(${JAVA_INCLUDE_PATH})
include_directories(${JAVA_INCLUDE_PATH2})
include_directories(${JAVA_AWT_INCLUDE_PATH})

add_library(silk SHARED ${SRC_DIR} )



