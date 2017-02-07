更换模拟器的方法


1》
先将
adreno
mali
pvr
等下面的指定版本的模拟器解压覆盖到
extern\ogles2\
覆盖其中的
include,lib


2》
将lib中的dll，覆盖到客户端目录下的
libGLESv2.dll
libEGL.dll

重现编译器客户端和引擎版本