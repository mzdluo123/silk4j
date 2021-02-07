aux_source_directory(src/libmp3lame LAME_SRC_DIR)
aux_source_directory(src/mpglib LAME_SRC_DIR)

include_directories(src src/libmp3lame src/mpglib)

add_library(lame SHARED ${LAME_SRC_DIR} src/JNILame.c src/JNILame.h)