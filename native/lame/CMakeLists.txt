aux_source_directory(src/libmp3lame LAME_SRC_DIR)

include_directories(src)

add_library(lame SHARED ${LAME_SRC_DIR} src/JNILame.cpp src/JNILame.h)