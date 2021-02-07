package io.github.mzdluo123.silk4j;

import java.io.*;

public class AudioUtils {
    private static File tempDir;

    /**
     * 初始化
     */
    public static void init() throws IOException {
        init(new File(System.getProperty("java.io.tmpdir")));
    }

    public static void init(File tmpDir) throws IOException {
        AudioUtils.tempDir = tmpDir;
        if (!tempDir.canWrite()) {
            throw new IOException("缓存目录无写入权限，请重试");
        }
    }

    public static File mp3ToSilk(File mp3File) throws IOException {
        if (!mp3File.exists()){
            throw new FileNotFoundException();
        }
        File pcmFile = getTempFile("pcm");
        File silkFile = getTempFile("silk");
        LameCoder.decode(mp3File.getAbsolutePath(), pcmFile.getAbsolutePath());
        SilkCoder.encode(pcmFile.getAbsolutePath(), silkFile.getAbsolutePath());
        pcmFile.delete();
        return silkFile;
    }

    public static File mp3ToSilk(InputStream mp3FileStream) throws IOException {

        File mp3File = getTempFile("mp3");
        FileOutputStream fileOutputStream = new FileOutputStream(mp3File);
        byte data[] = mp3FileStream.readAllBytes();
        fileOutputStream.write(data);
        fileOutputStream.close();
        return mp3ToSilk(mp3File);
    }

    public static File silkToMp3(File silkFile) throws IOException {
        if (!silkFile.exists()){
            throw new FileNotFoundException();
        }
        File pcmFile = getTempFile("pcm");
        File mp3File = getTempFile("mp3");
        SilkCoder.decode(silkFile.getAbsolutePath(), pcmFile.getAbsolutePath());
        LameCoder.encode(pcmFile.getAbsolutePath(), mp3File.getAbsolutePath());
        pcmFile.delete();
        return mp3File;
    }

    public static File silkToMp3(InputStream silkFileStream) throws IOException {
        File mp3File = getTempFile("silk");
        FileOutputStream fileOutputStream = new FileOutputStream(mp3File);
        byte data[] = silkFileStream.readAllBytes();
        fileOutputStream.write(data);
        fileOutputStream.close();
        return silkToMp3(mp3File);
    }


    private static File getTempFile(String type) {
        String fileName = "mirai_audio_" +
                type +
                "_" +
                System.currentTimeMillis() +
                "." +
                type;
        return new File(tempDir, fileName);
    }
}
