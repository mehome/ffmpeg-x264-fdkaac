# ffmpeg-x264-fdkaac
so 库的版本介绍
ffmpeg 3.3.9
libx264  2019 . 11. 1
libfdk-aac 0.1.6
ndk   r15c   
android platform level 21

ffmpeg 编译支持选项可以参考build_ffmpeg_with_x264_fdkaac.sh

由于编译的第三方库libfdk-aac 对android platform level 要求，编译中用的是21，而项目中普遍用的 是16，所以编译so库

注意：由于libx264  libfdk-aac 在编译so的时候 会出现 版本号后缀， 如libx264.so.158,
android studio 无法加载这种so ，所以需要将so的soname修改 为libx264.so（修改  patchelf  --set-soname libx264.so  ./libx264.so.158 ） 
然后再编译ffmpeg
