//
// Created by mzdlu on 2021-02-05.
//

#include <cstring>
#include "JNILame.h"

#define BUFFER_SIZE 8192
#define be_short(s) ((short) ((unsigned short) (s) << 8) | ((unsigned short) (s) >> 8))

lame_global_flags *glf;
hip_t hip;


int read_samples(FILE *input_file, short *input) {
    int nb_read;
    nb_read = fread(input, 1, sizeof(short), input_file) / sizeof(short);

//    int i = 0;
//    while (i < nb_read) {
//        input[i] = be_short(input[i]);
//        i++;
//    }

    return nb_read;
}

JNIEXPORT void JNICALL Java_io_github_mzdluo123_silk4j_LameCoder_initializeDefault(
        JNIEnv *env, jclass cls) {

    glf = initializeDefault(env);
}

JNIEXPORT void JNICALL Java_io_github_mzdluo123_silk4j_LameCoder_initialize(
        JNIEnv *env, jclass cls, jint inSamplerate, jint outChannel,
        jint outSamplerate, jint outBitrate, jfloat scaleInput, jint mode, jint vbrMode,
        jint quality, jint vbrQuality, jint abrMeanBitrate, jint lowpassFreq, jint highpassFreq,
        jstring id3tagTitle, jstring id3tagArtist, jstring id3tagAlbum,
        jstring id3tagYear, jstring id3tagComment) {

    glf = initialize(env, inSamplerate, outChannel, outSamplerate, outBitrate, scaleInput, mode,
                     vbrMode,
                     quality, vbrQuality, abrMeanBitrate, lowpassFreq, highpassFreq, id3tagTitle,
                     id3tagArtist, id3tagAlbum,
                     id3tagYear,
                     id3tagComment);
}
//
//JNIEXPORT jint JNICALL Java_io_github_mzdluo123_silk4j_LameCoder_lameEncode(
//        JNIEnv *env, jclass cls, jshortArray buffer_l,
//        jshortArray buffer_r, jint samples, jbyteArray mp3buf) {
//    return encode(env, glf, buffer_l, buffer_r, samples, mp3buf);
//}
//
//JNIEXPORT jint JNICALL Java_io_github_mzdluo123_silk4j_LameCoder_encodeBufferInterleaved(
//        JNIEnv *env, jclass cls, jshortArray pcm,
//        jint samples, jbyteArray mp3buf) {
//    return encodeBufferInterleaved(env, glf, pcm, samples, mp3buf);
//}
//
//JNIEXPORT jint JNICALL Java_io_github_mzdluo123_silk4j_LameCoder_lameFlush(
//        JNIEnv *env, jclass cls, jbyteArray mp3buf) {
//    return flush(env, glf, mp3buf);
//}

JNIEXPORT void JNICALL Java_io_github_mzdluo123_silk4j_LameCoder_lameClose(
        JNIEnv *env, jclass cls) {
    close(glf);
}

JNIEXPORT void JNICALL Java_io_github_mzdluo123_silk4j_LameCoder_encodeFile
        (JNIEnv *env,
         jclass cls, jstring in_source_path, jstring in_target_path) {

    const char *source_path, *target_path;
    source_path = env->GetStringUTFChars(in_source_path, NULL);
    target_path = env->GetStringUTFChars(in_target_path, NULL);

    FILE *input_file, *output_file;
    input_file = fopen(source_path, "rb");
    output_file = fopen(target_path, "wb");

    short input[BUFFER_SIZE];
    unsigned char output[BUFFER_SIZE];
    int nb_read = 0;
    int nb_write = 0;
    int nb_total = 0;

//	LOGD("Encoding started");
    nb_read = read_samples(input_file, input);
    while (nb_read) {
        nb_write = lame_encode_buffer(glf, input, input, nb_read, output,
                                      BUFFER_SIZE);
        fwrite(output, nb_write, 1, output_file);
        nb_total += nb_write;
        nb_read = read_samples(input_file, input);
    }
//	LOGD("Encoded %d bytes", nb_total);

    nb_write = lame_encode_flush(glf, output, BUFFER_SIZE);
    fwrite(output, nb_write, 1, output_file);
//	LOGD("Flushed %d bytes", nb_write);

    fclose(input_file);
    fclose(output_file);
}


lame_global_flags *initializeDefault(JNIEnv *env) {
    lame_global_flags *glf = lame_init();
    lame_init_params(glf);
    return glf;
}

lame_global_flags *initialize(
        JNIEnv *env,
        jint inSamplerate, jint outChannel,
        jint outSamplerate, jint outBitrate, jfloat scaleInput, jint mode, jint vbrMode,
        jint quality, jint vbrQuality, jint abrMeanBitrate, jint lowpassFreq, jint highpassFreq,
        jstring id3tagTitle, jstring id3tagArtist, jstring id3tagAlbum,
        jstring id3tagYear, jstring id3tagComment) {

    lame_global_flags *glf = lame_init();
    lame_set_in_samplerate(glf, inSamplerate);
    lame_set_num_channels(glf, outChannel);
    lame_set_out_samplerate(glf, outSamplerate);
    lame_set_brate(glf, outBitrate);
    lame_set_quality(glf, quality);
    lame_set_scale(glf, scaleInput);
    lame_set_VBR_q(glf, vbrQuality);
    lame_set_VBR_mean_bitrate_kbps(glf, abrMeanBitrate);
    lame_set_lowpassfreq(glf, lowpassFreq);
    lame_set_highpassfreq(glf, highpassFreq);

    switch (mode) {
        case 0:
            lame_set_mode(glf, STEREO);
            break;
        case 1:
            lame_set_mode(glf, JOINT_STEREO);
            break;
        case 3:
            lame_set_mode(glf, MONO);
            break;
        case 4:
            lame_set_mode(glf, NOT_SET);
            break;
        default:
            lame_set_mode(glf, NOT_SET);
            break;
    }

    switch (vbrMode) {
        case 0:
            lame_set_VBR(glf, vbr_off);
            break;
        case 2:
            lame_set_VBR(glf, vbr_rh);
            break;
        case 3:
            lame_set_VBR(glf, vbr_abr);
            break;
        case 4:
            lame_set_VBR(glf, vbr_mtrh);
            break;
        case 6:
            lame_set_VBR(glf, vbr_default);
            break;
        default:
            lame_set_VBR(glf, vbr_off);
            break;

    }


    const jchar *title = NULL;
    const jchar *artist = NULL;
    const jchar *album = NULL;
    const jchar *year = NULL;
    const jchar *comment = NULL;
    if (id3tagTitle) {
        title = env->GetStringChars(id3tagTitle, NULL);
    }
    if (id3tagArtist) {
        artist = env->GetStringChars(id3tagArtist, NULL);
    }
    if (id3tagAlbum) {
        album = env->GetStringChars(id3tagAlbum, NULL);
    }
    if (id3tagYear) {
        year = env->GetStringChars(id3tagYear, NULL);
    }
    if (id3tagComment) {
        comment = env->GetStringChars(id3tagComment, NULL);
    }

    if (title || artist || album || year || comment) {
        id3tag_init(glf);

        if (title) {
            id3tag_set_title(glf, (const char *) title);
            env->ReleaseStringChars(id3tagTitle, title);
        }
        if (artist) {
            id3tag_set_artist(glf, (const char *) artist);
            env->ReleaseStringChars(id3tagArtist, artist);
        }
        if (album) {
            id3tag_set_album(glf, (const char *) album);
            env->ReleaseStringChars(id3tagAlbum, album);
        }
        if (year) {
            id3tag_set_year(glf, (const char *) year);
            env->ReleaseStringChars(id3tagYear, year);
        }
        if (comment) {
            id3tag_set_comment(glf, (const char *) comment);
            env->ReleaseStringChars(id3tagComment, comment);
        }
    }

    lame_init_params(glf);


    return glf;
}

JNIEXPORT void JNICALL Java_io_github_mzdluo123_silk4j_LameCoder_initDecoder
        (JNIEnv *, jclass) {
    hip = hip_decode_init();
}

JNIEXPORT void JNICALL Java_io_github_mzdluo123_silk4j_LameCoder_closeDecoder
        (JNIEnv *, jclass) {
    hip_decode_exit(hip);
}

JNIEXPORT void JNICALL Java_io_github_mzdluo123_silk4j_LameCoder_decodeFile
        (JNIEnv *env, jclass cls, jstring source, jstring dest) {
    const char *source_path, *target_path;
    source_path = env->GetStringUTFChars(source, NULL);
    target_path = env->GetStringUTFChars(dest, NULL);

    FILE *input_file, *output_file;
    input_file = fopen(source_path, "rb");
    output_file = fopen(target_path, "wb");

    unsigned char input[BUFFER_SIZE];
    short output_l[BUFFER_SIZE *10];
    short output_r[BUFFER_SIZE *10];
    memset(output_l,0,BUFFER_SIZE*10);
    memset(output_r,0,BUFFER_SIZE*10);
    int nb_read = 0;
    int nb_write = 0;
    int nb_total = 0;

//	LOGD("Encoding started");

    nb_read = fread(input, 1, BUFFER_SIZE, input_file);
    while (nb_read) {
//        nb_write = lame_encode_buffer(glf, input, input, nb_read, output,
//                                      BUFFER_SIZE);
        nb_write = hip_decode(hip, input, nb_read, output_l, output_r);
        fwrite(output_l, nb_write, sizeof(short), output_file);
        nb_total += nb_write;
        nb_read = fread(input, 1, BUFFER_SIZE, input_file);
    }

//	LOGD("Encoded %d bytes", nb_total);
//
//    nb_write = lame_encode_flush(glf, output, BUFFER_SIZE);
//    fwrite(output, nb_write, 1, output_file);
//	LOGD("Flushed %d bytes", nb_write);

    fclose(input_file);
    fclose(output_file);
}


//
//jint encode(
//        JNIEnv *env, lame_global_flags *glf,
//        jshortArray buffer_l, jshortArray buffer_r,
//        jint samples, jbyteArray mp3buf) {
//    jshort *j_buffer_l = env->GetShortArrayElements(buffer_l, NULL);
//
//    jshort *j_buffer_r = env->GetShortArrayElements(buffer_r, NULL);
//
//    const jsize mp3buf_size = env->GetArrayLength(mp3buf);
//    jbyte *j_mp3buf = env->GetByteArrayElements(mp3buf, NULL);
//
//    int result = lame_encode_buffer(glf, j_buffer_l,
//                                    j_buffer_r,
//                                    samples, reinterpret_cast<unsigned char *>(j_mp3buf), mp3buf_size);
//
//    env->ReleaseShortArrayElements(buffer_l, j_buffer_l, 0);
//    env->ReleaseShortArrayElements(buffer_r, j_buffer_r, 0);
//    env->ReleaseByteArrayElements(mp3buf, j_mp3buf, 0);
//
//    return result;
//}
//
//jint encodeBufferInterleaved(
//        JNIEnv *env, lame_global_flags *glf,
//        jshortArray pcm, jint samples, jbyteArray mp3buf) {
//    jshort *j_pcm = env->GetShortArrayElements(pcm, NULL);
//
//    const jsize mp3buf_size = env->GetArrayLength(mp3buf);
//    jbyte *j_mp3buf = env->GetByteArrayElements(mp3buf, NULL);
//
//    int result = lame_encode_buffer_interleaved(glf, j_pcm,
//                                                samples, reinterpret_cast<unsigned char *>(j_mp3buf), mp3buf_size);
//
//    env->ReleaseShortArrayElements(pcm, j_pcm, 0);
//    env->ReleaseByteArrayElements(mp3buf, j_mp3buf, 0);
//
//    return result;
//}
//
//jint flush(
//        JNIEnv *env, lame_global_flags *glf,
//        jbyteArray mp3buf) {
//    const jsize mp3buf_size = env->GetArrayLength(mp3buf);
//    jbyte *j_mp3buf = env->GetByteArrayElements(mp3buf, NULL);
//
//    int result = lame_encode_flush(glf, reinterpret_cast<unsigned char *>(j_mp3buf), mp3buf_size);
//
//    env->ReleaseByteArrayElements(mp3buf, j_mp3buf, 0);
//
//    return result;
//}

void close(
        lame_global_flags *glf) {
    lame_close(glf);
    glf = NULL;

}