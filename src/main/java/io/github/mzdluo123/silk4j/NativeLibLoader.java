package io.github.mzdluo123.silk4j;

import java.io.*;
import java.util.List;
import java.util.stream.Collectors;


public class NativeLibLoader {

    public static void load() throws IOException {
        try {
            // 尝试直接加载
            System.loadLibrary("lame");
            System.loadLibrary("silk");
        } catch (UnsatisfiedLinkError ignore) {

            try {
                System.loadLibrary("liblame");
                System.loadLibrary("libsilk");
            } catch (UnsatisfiedLinkError i) {
                // 失败后
                loadFromResources();
            }
        }


    }

    private static void loadFromResources() throws IOException {
        InputStream fileListInputStream = NativeLibLoader.class.getResourceAsStream("/silk4j_libs/filelist.txt");
        if (fileListInputStream == null) {
            throw new FileNotFoundException("未找到文件列表，请尝试从GitHub下载最新的AllInOneJar");
        }
        InputStreamReader inputStreamReader = new InputStreamReader(fileListInputStream);
        BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
        List<String> fileList = bufferedReader.lines().collect(Collectors.toList());
        int successCount = 0;
        for (String libFile :
                fileList) {
            if (successCount >= 2) {
                return;
            }
            String path = copyFileToTemp(NativeLibLoader.class.getResourceAsStream(libFile.substring(1)));
            try {
                System.load(path);
                successCount++;
            } catch (UnsatisfiedLinkError ignored) {
                new File(path).delete();
            }
        }
        if (successCount < 2) {
            throw new UnsatisfiedLinkError("未找到适用于当前操作系统的动态链接库，请联系作者");
        }
    }


    private static String copyFileToTemp(InputStream file) throws IOException {
        File temp = AudioUtils.getTempFile("lib");
        AudioUtils.streamToTempFile(file, temp);
        return temp.getAbsolutePath();
    }
}
