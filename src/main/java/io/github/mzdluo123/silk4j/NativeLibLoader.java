package io.github.mzdluo123.silk4j;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;


public class NativeLibLoader {

    public static void load() {
        try {
            // 尝试直接加载
            System.loadLibrary("lame");
            System.loadLibrary("silk");
        } catch (UnsatisfiedLinkError t) {
            // 失败后
            loadFromResources();
        }


    }

    public static void loadFromResources() {
        String os = System.getProperty("os.name");
        String arch = System.getProperty("os.arch");
        System.out.println(os);
        System.out.println(arch);
    }


    public String copyFileToTemp(InputStream file) throws IOException {
        File temp = AudioUtils.getTempFile("lib");
        byte[] data = file.readAllBytes();
        file.close();
        FileOutputStream fileOutputStream = new FileOutputStream(temp);
        fileOutputStream.write(data);
        fileOutputStream.close();
        return temp.getAbsolutePath();
    }
}
