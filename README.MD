# Silk4j

silkv3编码器的Java接口，为mirai相关生态提供语音编解码方案


# 导入依赖

```

repositories {
   maven { url 'https://maven.pkg.github.com/mzdluo123/silk4j' }

}
dependencies {
    implementation("io.github.mzdluo123:silk4j:版本号")
}
```
版本号请到https://github.com/mzdluo123/silk4j/packages/ 查看

你也可以到action页面下载AllInOne中的jar来开始使用

# 使用

```java

// 请使用这个类完成编码
// 所有的编码默认都只使用左声道
public class AudioUtils {
    //使用之前需要初始化
    public static void init() throws IOException {
    }
    // 初始化时可选指定缓存目录，未指定则使用系统目录
    public static void init(File tmpDir) throws IOException {
      
    }
    // 以下为编码函数，只可以同时进行一个编码任务，如需pcm文件请看源代码
    // 返回的文件在缓存目录内，使用完毕请删除
    public static File mp3ToSilk(File mp3File) throws IOException {
       
    }
    public static File mp3ToSilk(InputStream mp3FileStream) throws IOException {
        
    }

    public static File silkToMp3(File silkFile) throws IOException {
      
    }

    public static File silkToMp3(InputStream silkFileStream) throws IOException {
        
    }
    
}

```
