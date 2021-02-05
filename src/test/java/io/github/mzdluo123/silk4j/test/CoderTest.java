package io.github.mzdluo123.silk4j.test;

import io.github.mzdluo123.silk4j.LameCoder;
import io.github.mzdluo123.silk4j.SilkCoder;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.io.File;
import java.io.IOException;

import static org.junit.jupiter.api.Assertions.assertTrue;

public class CoderTest {
    @BeforeEach
    public void init() {
        File dll = new File("native/cmake-build-debug/libsilk.dll");
        if (!dll.exists()) {
            dll = new File("native/cmake-build-debug/libsilk.so");
        }

        System.load(dll.getAbsolutePath());

         dll = new File("native/cmake-build-debug/lame/liblame.dll");
        if (!dll.exists()) {
            dll = new File("native/cmake-build-debug/liblame.so");
        }

        System.load(dll.getAbsolutePath());
    }

    @Test
    public void testDecode() {
        SilkCoder.decode("src/test/resources/77b18b66-e4d5-4847-8c44-cad582d75ad4.silk", "out.pcm");
        File outFile = new File("out.pcm");
        assertTrue(outFile.exists());
    }

    @Test
    public void testEncode() {
        SilkCoder.encode("out.pcm", "silk.amr");
        File outFile = new File("silk.amr");
        assertTrue(outFile.exists());
        outFile.delete();

    }

    @Test
    public void testEncodeMp3() throws IOException {
        LameCoder.encode("out.pcm", "silk.mp3");
        File outFile = new File("silk.mp3");
        assertTrue(outFile.exists());

    }
    @Test
    public void testDecodeMp3() throws IOException {
        LameCoder.decode("silk.mp3", "decode.pcm");
        File outFile = new File("decode.pcm");
        assertTrue(outFile.exists());
    }
}
