
@ECHO 先拷贝头文件
xcopy "../visual/*.h" "fm_engine_src/visual/" /Y/F
xcopy "../utils/*.h" "fm_engine_src/utils/" /Y/F
xcopy "../system/*.h" "fm_engine_src/system/" /Y/F
xcopy "../system/*.cpp" fm_engine_src/"system/" /Y/F
xcopy "../public/*.h" "fm_engine_src/public/" /Y/F
xcopy "../math/*.h" "fm_engine_src/math/" /Y/F
@ECHO 拷贝第三方库
xcopy "../extern/glslcompiler/*.exe" "fm_engine_src/extern/glslcompiler/" /Y/F

xcopy "../proj/*.*" "fm_engine_src/proj/" /Y/F


@ECHO 发布逻辑代码
@ECHO fm_demo
xcopy "../fm_demo/*.h" "fm_engine_src/fm_demo/" /Y/F
xcopy "../fm_demo/*.cpp" "fm_engine_src/fm_demo/" /Y/F
xcopy "../fm_demo/*.vcproj" "fm_engine_src/fm_demo/" /Y/F
@ECHO fm_game
xcopy "../fm_game/*.h" "fm_engine_src/fm_game/" /Y/F
xcopy "../fm_game/*.cpp" "fm_engine_src/fm_game/" /Y/F
xcopy "../fm_game/*.vcproj" "fm_engine_src/fm_game/" /Y/F
@ECHO fm_main
xcopy "../fm_main/*.h" "fm_engine_src/fm_main/" /Y/F
xcopy "../fm_main/*.cpp" "fm_engine_src/fm_main/" /Y/F
xcopy "../fm_main/gamelogic/*.cpp" "fm_engine_src/fm_main/gamelogic" /Y/F
xcopy "../fm_main/gamelogic/*.h" "fm_engine_src/fm_main/gamelogic" /Y/F
xcopy "../fm_main/*.vcproj" "fm_engine_src/fm_main/" /Y/F

@ECHO 发布部分引擎库
@ECHO fm_gui
xcopy "../fm_gui/*.h" "fm_engine_src/fm_gui/" /Y/F
xcopy "../fm_gui/*.cpp" "fm_engine_src/fm_gui/" /Y/F
xcopy "../fm_gui/*.vcproj" "fm_engine_src/fm_gui/" /Y/F

@ECHO fm_geometry
xcopy "../fm_geometry/*.h" "fm_engine_src/fm_geometry/" /Y/F
xcopy "../fm_geometry/*.cpp" "fm_engine_src/fm_geometry/" /Y/F
xcopy "../fm_geometry/*.vcproj" "fm_engine_src/fm_geometry/" /Y/F

@ECHO fm_input
xcopy "../fm_input/*.h" "fm_engine_src/fm_input/" /Y/F
xcopy "../fm_input/*.cpp" "fm_engine_src/fm_input/" /Y/F
xcopy "../fm_input/*.vcproj" "fm_engine_src/fm_input/" /Y/F

@ECHO fm_math
xcopy "../fm_math/*.h" "fm_engine_src/fm_math/" /Y/F
xcopy "../fm_math/*.cpp" "fm_engine_src/fm_math/" /Y/F
xcopy "../fm_math/*.vcproj" "fm_engine_src/fm_math/" /Y/F

@ECHO fm_model
xcopy "../fm_model/*.h" "fm_engine_src/fm_model/" /Y/F
xcopy "../fm_model/*.cpp" "fm_engine_src/fm_model/" /Y/F
xcopy "../fm_model/*.vcproj" "fm_engine_src/fm_model/" /Y/F

@ECHO fm_movie
xcopy "../fm_movie/*.h" "fm_engine_src/fm_movie/" /Y/F
xcopy "../fm_movie/*.cpp" "fm_engine_src/fm_movie/" /Y/F
xcopy "../fm_movie/*.vcproj" "fm_engine_src/fm_movie/" /Y/F

@ECHO fm_net
xcopy "../fm_net/*.h" "fm_engine_src/fm_net/" /Y/F
xcopy "../fm_net/*.cpp" "fm_engine_src/fm_net/" /Y/F
xcopy "../fm_net/*.vcproj" "fm_engine_src/fm_net/" /Y/F

@ECHO fm_physics_bullet
xcopy "../fm_physics_bullet/*.h" "fm_engine_src/fm_physics_bullet/" /Y/F
xcopy "../fm_physics_bullet/*.cpp" "fm_engine_src/fm_physics_bullet/" /Y/F
xcopy "../fm_physics_bullet/*.vcproj" "fm_engine_src/fm_physics_bullet/" /Y/F
xcopy "../fm_physics_bullet/bullet/*.h" "fm_engine_src/fm_physics_bullet/bullet/" /Y/F
xcopy "../fm_physics_bullet/bullet/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/" /Y/F
xcopy "../fm_physics_bullet/bullet/*.h" "fm_engine_src/fm_physics_bullet/bullet/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_collision/broadphase_collision/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_collision/broadphase_collision/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_collision/broadphase_collision/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_collision/broadphase_collision/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_collision/collision_dispatch/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_collision/collision_dispatch/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_collision/collision_dispatch/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_collision/collision_dispatch/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_collision/collision_shapes/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_collision/collision_shapes/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_collision/collision_shapes/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_collision/collision_shapes/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_collision/gimpact/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_collision/gimpact/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_collision/gimpact/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_collision/gimpact/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_collision/narrow_phase_collision/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_collision/narrow_phase_collision/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_collision/narrow_phase_collision/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_collision/narrow_phase_collision/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/character/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/character/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/character/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/character/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/constraint_solver/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/constraint_solver/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/constraint_solver/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/constraint_solver/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/dynamics/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/dynamics/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/dynamics/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/dynamics/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/featherstone/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/featherstone/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/featherstone/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/featherstone/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/m_l_c_p_solvers/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/m_l_c_p_solvers/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/m_l_c_p_solvers/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/m_l_c_p_solvers/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/vehicle/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/vehicle/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_dynamics/vehicle/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_dynamics/vehicle/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/d_x_11/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/d_x_11/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/d_x_11/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/d_x_11/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/open_c_l/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/open_c_l/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/open_c_l/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/open_c_l/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/open_c_l/mini_c_l/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/open_c_l/mini_c_l/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/open_c_l/mini_c_l/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/open_c_l/mini_c_l/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/shared/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/gpu_soft_body_solvers/shared/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/spu_narrow_phase_collision_task/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/spu_narrow_phase_collision_task/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/spu_narrow_phase_collision_task/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/spu_narrow_phase_collision_task/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/spu_sample_task/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/spu_sample_task/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_multi_threaded/spu_sample_task/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_multi_threaded/spu_sample_task/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_soft_body/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/bullet_soft_body/" /Y/F
xcopy "../fm_physics_bullet/bullet/bullet_soft_body/*.h" "fm_engine_src/fm_physics_bullet/bullet/bullet_soft_body/" /Y/F
xcopy "../fm_physics_bullet/bullet/linear_math/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/linear_math/" /Y/F
xcopy "../fm_physics_bullet/bullet/linear_math/*.h" "fm_engine_src/fm_physics_bullet/bullet/linear_math/" /Y/F
xcopy "../fm_physics_bullet/bullet/mini_c_l/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/mini_c_l/" /Y/F
xcopy "../fm_physics_bullet/bullet/mini_c_l/*.h" "fm_engine_src/fm_physics_bullet/bullet/mini_c_l/" /Y/F
xcopy "../fm_physics_bullet/bullet/vectormath/neon/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/vectormath/neon/" /Y/F
xcopy "../fm_physics_bullet/bullet/vectormath/neon/*.h" "fm_engine_src/fm_physics_bullet/bullet/vectormath/neon/" /Y/F
xcopy "../fm_physics_bullet/bullet/vectormath/scalar/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/vectormath/scalar/" /Y/F
xcopy "../fm_physics_bullet/bullet/vectormath/scalar/*.h" "fm_engine_src/fm_physics_bullet/bullet/vectormath/scalar/" /Y/F
xcopy "../fm_physics_bullet/bullet/vectormath/sse/*.cpp" "fm_engine_src/fm_physics_bullet/bullet/vectormath/sse/" /Y/F
xcopy "../fm_physics_bullet/bullet/vectormath/sse/*.h" "fm_engine_src/fm_physics_bullet/bullet/vectormath/sse/" /Y/F
xcopy "../fm_physics_bullet/bullet/vectormath/*.h" "fm_engine_src/fm_physics_bullet/bullet/vectormath/" /Y/F

@ECHO fm_shader
xcopy "../fm_shader/*.fsh" "fm_engine_src/fm_shader/" /Y/F
xcopy "../fm_shader/*.vsh" "fm_engine_src/fm_shader/" /Y/F
xcopy "../fm_shader/*.vcproj" "fm_engine_src/fm_shader/" /Y/F

@ECHO fm_sound
xcopy "../fm_sound/*.h" "fm_engine_src/fm_sound/" /Y/F
xcopy "../fm_sound/*.cpp" "fm_engine_src/fm_sound/" /Y/F
xcopy "../fm_sound/*.vcproj" "fm_engine_src/fm_sound/" /Y/F

@ECHO fm_terrain
xcopy "../fm_terrain/*.h" "fm_engine_src/fm_terrain/" /Y/F
xcopy "../fm_terrain/*.cpp" "fm_engine_src/fm_terrain/" /Y/F
xcopy "../fm_terrain/*.vcproj" "fm_engine_src/fm_terrain/" /Y/F

@ECHO fm_ui3d
xcopy "../fm_ui3d/*.h" "fm_engine_src/fm_ui3d/" /Y/F
xcopy "../fm_ui3d/*.cpp" "fm_engine_src/fm_ui3d/" /Y/F
xcopy "../fm_ui3d/*.vcproj" "fm_engine_src/fm_ui3d/" /Y/F

@ECHO fm_world
xcopy "../fm_world/*.h" "fm_engine_src/fm_world/" /Y/F
xcopy "../fm_world/*.cpp" "fm_engine_src/fm_world/" /Y/F
xcopy "../fm_world/*.vcproj" "fm_engine_src/fm_world/" /Y/F

xcopy "../bin/*.dll" "fm_engine_src/bin/" /Y/F
