package io.github.mzdluo123.silk4j;

import java.net.URL;

public class SilkCoder {
    static native void decode(String source, String dest, int fs_Hz, int loss);

    public static void decode(String source, String dest) {
        decode(source, dest, 24000, 0);
    }

}
