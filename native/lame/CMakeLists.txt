aux_source_directory(src/libmp3lame LAME_SRC_DIR)
aux_source_directory(src/libmpg123 LAME_SRC_DIR)

include_directories(src src/libmp3lame src/libmpg123)

add_library(lame SHARED ${LAME_SRC_DIR} src/JNILame.cpp src/JNILame.h)