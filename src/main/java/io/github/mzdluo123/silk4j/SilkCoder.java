package io.github.mzdluo123.silk4j;

import java.net.URL;

public class SilkCoder {
    static native void decode(String source, String dest, int fs_Hz, int loss);

    public static void decode(String source, String dest) {
        decode(source, dest, 24000, 0);
    }


    public static native void encode(String source,
                                     String dest,
                                     boolean tencent,
                                     int fs_Hz,
                                     int maxInternalSampleRate,
                                     int packetSize,
                                     int packetLossPercentage,
                                     int useInBandFEC,
                                     int useDTX,
                                     int complexity,
                                     int bitRate);

    public static void encode(String source,
                              String dest, int sampleRate
    ) {
        encode(source,
                dest, true,
                sampleRate,
                24000,
                (20 * sampleRate) / 1000,
                0,
                0,
                0,
                2,
                16000);
    }

}
