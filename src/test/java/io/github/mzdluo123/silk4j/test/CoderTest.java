package io.github.mzdluo123.silk4j.test;

import io.github.mzdluo123.silk4j.SilkCoder;
import org.junit.jupiter.api.Test;
import java.io.File;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class CoderTest {
    @Test
    public void testDecode() {
        SilkCoder.decode("src/test/resources/77b18b66-e4d5-4847-8c44-cad582d75ad4.silk", "out.pcm");
        File outFile = new File("out.pcm");
        assertTrue(outFile.exists());
        outFile.delete();
    }
}
