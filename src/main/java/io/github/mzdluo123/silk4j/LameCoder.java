package io.github.mzdluo123.silk4j;

import java.io.*;
import java.util.Arrays;

public class LameCoder {


    public LameCoder() {
        initializeDefault();
    }

    public LameCoder(LameBuilder builder) {
        initialize(builder);
    }


    private void initialize(LameBuilder builder) {
        initialize(builder.inSampleRate, builder.outChannel, builder.outSampleRate,
                builder.outBitrate, builder.scaleInput, getIntForMode(builder.mode), getIntForVbrMode(builder.vbrMode), builder.quality, builder.vbrQuality, builder.abrMeanBitrate,
                builder.lowpassFreq, builder.highpassFreq, builder.id3tagTitle, builder.id3tagArtist,
                builder.id3tagAlbum, builder.id3tagYear, builder.id3tagComment);
    }
//
//    public int encode(short[] buffer_l, short[] buffer_r,
//                      int samples, byte[] mp3buf) {
//
//        return lameEncode(buffer_l, buffer_r, samples, mp3buf);
//    }
//
//    public int encodeBufferInterLeaved(short[] pcm, int samples,
//                                       byte[] mp3buf) {
//        return encodeBufferInterleaved(pcm, samples, mp3buf);
//    }
//
//    public int flush(byte[] mp3buf) {
//        return lameFlush(mp3buf);
//    }

    public void close() {
        lameClose();
    }

    public static void encode(String source, String dest) throws IOException {

        LameCoder coder = new LameBuilder()
                .setInSampleRate(16000)
                .setOutChannels(1)
                .setOutBitrate(48)
                .setOutSampleRate(16000)
                .setQuality(8)
                .build();
        encodeFile(source,dest);
        coder.close();
    }

    private static native void initializeDefault();

    private static native void initialize(int inSamplerate, int outChannel,
                                          int outSamplerate, int outBitrate, float scaleInput, int mode, int vbrMode,
                                          int quality, int vbrQuality, int abrMeanBitrate, int lowpassFreq, int highpassFreq, String id3tagTitle,
                                          String id3tagArtist, String id3tagAlbum, String id3tagYear,
                                          String id3tagComment);

//    private native static int lameEncode(short[] buffer_l, short[] buffer_r,
//                                         int samples, byte[] mp3buf);
//
//
//    private native static int encodeBufferInterleaved(short[] pcm, int samples,
//                                                      byte[] mp3buf);
//
//
//    private native static int lameFlush(byte[] mp3buf);


    private native static void lameClose();

    private native static void encodeFile(String source,String dest);


    ////UTILS
    private static int getIntForMode(LameBuilder.Mode mode) {
        switch (mode) {
            case STEREO:
                return 0;
            case JSTEREO:
                return 1;
            case MONO:
                return 3;
            case DEFAULT:
                return 4;
        }
        return -1;
    }

    private static int getIntForVbrMode(LameBuilder.VbrMode mode) {
        switch (mode) {
            case VBR_OFF:
                return 0;
            case VBR_RH:
                return 2;
            case VBR_ABR:
                return 3;
            case VBR_MTRH:
                return 4;
            case VBR_DEFAUT:
                return 6;
        }
        return -1;
    }

}
