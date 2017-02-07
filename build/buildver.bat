
@ECHO 先拷贝头文件
xcopy "../visual/*.h" "window/visual/" /Y/F
xcopy "../utils/*.h" "window/utils/" /Y/window/
xcopy "../system/*.h" "window/system/" /Y/F
xcopy "../system/*.cpp" "window/system/" /Y/F
xcopy "../public/*.h" "window/public/" /Y/F
xcopy "../math/*.h" "window/math/" /Y/F
@ECHO 拷贝第三方库
xcopy "../extern/fmod/inc/*.*" "window/extern/fmod/inc/" /Y/F
xcopy "../extern/fmod/lib/*.*" "window/extern/fmod/lib/" /Y/F
xcopy "../extern/freeimage/include/*.h"  "window/extern/freeimage/include/" /Y/F
xcopy "../extern/freeimage/include/*.*"  "window/extern/freeimage/lib/" /Y/F
xcopy "../extern/freetype/include/*.*"  "window/extern/freetype/include/" /Y/F
xcopy "../extern/freetype/include/freetype/*.*"  "window/extern/freetype/include/freetype/" /Y/F
xcopy "../extern/freetype/include/freetype/config/*.*"  "window/extern/freetype/include/freetype/config/" /Y/F
xcopy "../extern/freetype/include/freetype/internal/*.*"  "window/extern/freetype/include/freetype/internal/" /Y/F
xcopy "../extern/freetype/include/freetype/internal/services/*.*"  "window/extern/freetype/include/freetype/internal/services/" /Y/F
xcopy "../extern/freetype/lib/*.*"  "window/extern/freetype/lib/" /Y/F

xcopy "../extern/ogles2/include/egl/*.h" "window/extern/ogles2/include/egl/" /Y/F
xcopy "../extern/ogles2/include/gles2/*.h" "window/extern/ogles2/include/gles2/" /Y/F
xcopy "../extern/ogles2/include/gles3/*.h" "window/extern/ogles2/include/gles3/" /Y/F
xcopy "../extern/ogles2/include/khr/*.h" "window/extern/ogles2/include/khr/" /Y/F
xcopy "../extern/ogles2/include/*.h" "window/extern/ogles2/include/" /Y/F
xcopy "../extern/ogles2/lib/*.*" "window/extern/ogles2/lib/" /Y/F
xcopy "../extern/quicklz/*.h" "window/extern/quicklz/" /Y/F
xcopy "../extern/quicklz/*.c" "window/extern/quicklz/" /Y/F
xcopy "../extern/rapidxml/*.*" "window/extern/rapidxml/" /Y/F

xcopy "../proj/*.*" "window/proj/" /Y/F

xcopy "mobile.sln" "window/" /Y/F

@ECHO 发布debug版本
xcopy "../debug/*.lib" "window/debug/" /Y/F
xcopy "../debug/*.pdb" "window/debug/" /Y/F
@ECHO 发布relase版本
xcopy "../Release/*.lib" "window/release/" /Y/F
xcopy "../Release/*.pdb" "window/release/" /Y/F
@ECHO 发布性能监控版本
xcopy "../perf_debug/*.lib" "window/perf_debug/" /Y/F
xcopy "../perf_debug/*.pdb" "window/perf_debug/" /Y/F
@ECHO 发布网络消息监控版本
xcopy "../full_msg_perf/*.lib" "window/full_msg_perf/" /Y/F
xcopy "../full_msg_perf/*.pdb" "window/full_msg_perf/" /Y/F

@ECHO 发布逻辑代码
@ECHO fm_demo
xcopy "../fm_demo/*.h" "window/fm_demo/" /Y/F
xcopy "../fm_demo/*.cpp" "window/fm_demo/" /Y/F
xcopy "../fm_demo/*.vcproj" "window/fm_demo/" /Y/F
@ECHO fm_game
xcopy "../fm_game/*.h" "window/fm_game/" /Y/F
xcopy "../fm_game/*.cpp" "window/fm_game/" /Y/F
xcopy "../fm_game/*.vcproj" "window/fm_game/" /Y/F
@ECHO fm_main
xcopy "../fm_main/*.h" "window/fm_main/" /Y/F
xcopy "../fm_main/*.cpp" "window/fm_main/" /Y/F
xcopy "../fm_main/gamelogic/*.cpp" "window/fm_main/gamelogic" /Y/F
xcopy "../fm_main/gamelogic/*.h" "window/fm_main/gamelogic" /Y/F
xcopy "../fm_main/*.vcproj" "window/fm_main/" /Y/F

@ECHO 发布部分引擎库
xcopy "../fm_gui/*.h" "window/fm_gui/" /Y/F
xcopy "../fm_gui/*.cpp" "window/fm_gui/" /Y/F
xcopy "../fm_gui/*.vcproj" "window/fm_gui/" /Y/F


@ECHO 发布资源相关
xcopy "../bin/pro/eff/*.fsh" "window/bin/pro/eff/" /Y/F
xcopy "../bin/pro/eff/*.vsh" "window/bin/pro/eff/" /Y/F

xcopy "../bin/*.dll" "window/bin/" /Y/F

@ECHO 删除网络模块相关资源
del "../window/public/outer_msg.h" /Q
del "../window/public/outer_err.h" /Q
del "../window/visual/i_game_client.h" /Q
del "../window/visual/i_game_receiver.h" /Q
del "../window/visual/i_game_sender.h" /Q
del "../window/visual/i_game_sock.h" /Q
del "../window/visual/i_solo_stub.h" /Q

