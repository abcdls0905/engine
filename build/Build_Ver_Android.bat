@ECHO 发布安卓资源
xcopy "../proj/android/hello-gl2/obj/local/armeabi/*.*" "android/armeabi/" /Y/F
xcopy "../proj/android/hello-gl2/obj/local/armeabi-v7a/*.*" "android/armeabi-v7a/" /Y/F
xcopy "../proj/android/hello-gl2/obj/local/x86/*.*" "android/x86/" /Y/F

@ECHO 删除移动引擎相关模块
del "../android/x86/libfm_gui.a" /Q
del "../android/x86/libapp.so" /Q
del "../android/x86/libgame.a" /Q

del "../android/armeabi-v7a/libfm_gui.a" /Q
del "../android/armeabi-v7a/libapp.so" /Q
del "../android/armeabi-v7a/libgame.a" /Q

del "../android/armeabi/libfm_gui.a" /Q
del "../android/armeabi/libapp.so" /Q
del "../android/armeabi/libgame.a" /Q