# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)
LOCAL_CODE_PATH = ../../../../
#fm_core
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_core/
include $(CLEAR_VARS)
LOCAL_MODULE    := libfm_core
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)core.cpp\
$(MY_LOCAL_PATH)ent_factory.cpp\
$(MY_LOCAL_PATH)ent_info_list.cpp\
$(MY_LOCAL_PATH)ent_info.cpp\
$(MY_LOCAL_PATH)ent_manager.cpp\
$(MY_LOCAL_PATH)file_sys.cpp\
$(MY_LOCAL_PATH)hook_manager.cpp\
$(MY_LOCAL_PATH)ini_file.cpp\
$(MY_LOCAL_PATH)int_manager.cpp\
$(MY_LOCAL_PATH)log_file.cpp\
$(MY_LOCAL_PATH)memory_pool.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_math
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_math/
LOCAL_MODULE    := libfm_math
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)fm_matrix_f.cpp\
$(MY_LOCAL_PATH)fm_matrix_x.cpp \
$(MY_LOCAL_PATH)fm_quaternion_f.cpp \
$(MY_LOCAL_PATH)fm_quaternion_x.cpp \
$(MY_LOCAL_PATH)fm_vector.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_model
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_model/
LOCAL_MODULE    := libfm_model
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)action_set_loader.cpp\
$(MY_LOCAL_PATH)action_data.cpp\
$(MY_LOCAL_PATH)action_player.cpp\
$(MY_LOCAL_PATH)action_set.cpp\
$(MY_LOCAL_PATH)array.cpp\
$(MY_LOCAL_PATH)aux_tool.cpp\
$(MY_LOCAL_PATH)dll_main.cpp\
$(MY_LOCAL_PATH)material_loader.cpp\
$(MY_LOCAL_PATH)math_3d.cpp\
$(MY_LOCAL_PATH)model_data.cpp\
$(MY_LOCAL_PATH)model_loader.cpp\
$(MY_LOCAL_PATH)model_player.cpp\
$(MY_LOCAL_PATH)model_player_draw.cpp\
$(MY_LOCAL_PATH)model_player_material.cpp\
$(MY_LOCAL_PATH)model_player_param.cpp\
$(MY_LOCAL_PATH)model_player_routine.cpp\
$(MY_LOCAL_PATH)model_player_shader.cpp\
$(MY_LOCAL_PATH)model_player_skin.cpp\
$(MY_LOCAL_PATH)model_player_texture.cpp\
$(MY_LOCAL_PATH)model_player_trace.cpp\
$(MY_LOCAL_PATH)model_player_update.cpp\
$(MY_LOCAL_PATH)model_system.cpp\
$(MY_LOCAL_PATH)oct_creator.cpp\
$(MY_LOCAL_PATH)oct_tree.cpp\
$(MY_LOCAL_PATH)res_manager.cpp\
$(MY_LOCAL_PATH)res_model.cpp\
$(MY_LOCAL_PATH)skeleton.cpp\
$(MY_LOCAL_PATH)skeleton_data.cpp\
$(MY_LOCAL_PATH)skeleton_loader.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_world
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_world/
LOCAL_MODULE    := libfm_world
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)action_control_info.cpp\
$(MY_LOCAL_PATH)action_control.cpp\
$(MY_LOCAL_PATH)actor_action.cpp\
$(MY_LOCAL_PATH)actor_foot.cpp\
$(MY_LOCAL_PATH)actor_look.cpp\
$(MY_LOCAL_PATH)actor_node.cpp\
$(MY_LOCAL_PATH)actor.cpp\
$(MY_LOCAL_PATH)broken_shadow.cpp\
$(MY_LOCAL_PATH)camera.cpp\
$(MY_LOCAL_PATH)effect_data.cpp\
$(MY_LOCAL_PATH)effect_model.cpp\
$(MY_LOCAL_PATH)lib_main.cpp\
$(MY_LOCAL_PATH)light_manager.cpp\
$(MY_LOCAL_PATH)light_source.cpp\
$(MY_LOCAL_PATH)model.cpp\
$(MY_LOCAL_PATH)particle_data.cpp\
$(MY_LOCAL_PATH)particle_info.cpp\
$(MY_LOCAL_PATH)particle_manager.cpp\
$(MY_LOCAL_PATH)particle_node.cpp\
$(MY_LOCAL_PATH)particle.cpp\
$(MY_LOCAL_PATH)post_debug.cpp\
$(MY_LOCAL_PATH)post_effect_filter.cpp\
$(MY_LOCAL_PATH)post_effect.cpp\
$(MY_LOCAL_PATH)render_tex.cpp\
$(MY_LOCAL_PATH)saber_arc_manager.cpp\
$(MY_LOCAL_PATH)saber_arc.cpp\
$(MY_LOCAL_PATH)scene.cpp\
$(MY_LOCAL_PATH)skin.cpp\
$(MY_LOCAL_PATH)sky_box.cpp\
$(MY_LOCAL_PATH)sun_glow.cpp\
$(MY_LOCAL_PATH)weather.cpp\
$(MY_LOCAL_PATH)world.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_terrain
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_terrain/
LOCAL_MODULE    := libfm_terrain
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)alpha_loader.cpp\
$(MY_LOCAL_PATH)alpha_map_compress.cpp\
$(MY_LOCAL_PATH)blend_tex_set.cpp\
$(MY_LOCAL_PATH)blend_tex.cpp\
$(MY_LOCAL_PATH)block_alpha_map.cpp\
$(MY_LOCAL_PATH)block_area.cpp\
$(MY_LOCAL_PATH)block_base.cpp\
$(MY_LOCAL_PATH)block_chunk.cpp\
$(MY_LOCAL_PATH)block_file.cpp\
$(MY_LOCAL_PATH)block_grass.cpp\
$(MY_LOCAL_PATH)block_height.cpp\
$(MY_LOCAL_PATH)block_height2.cpp\
$(MY_LOCAL_PATH)block_region.cpp\
$(MY_LOCAL_PATH)block_walk.cpp\
$(MY_LOCAL_PATH)block_water.cpp\
$(MY_LOCAL_PATH)chunk_style.cpp\
$(MY_LOCAL_PATH)chunk_vb.cpp\
$(MY_LOCAL_PATH)grass_type.cpp\
$(MY_LOCAL_PATH)height_compress.cpp\
$(MY_LOCAL_PATH)height_loader.cpp\
$(MY_LOCAL_PATH)lib_main.cpp\
$(MY_LOCAL_PATH)light_geometry.cpp\
$(MY_LOCAL_PATH)light_info.cpp\
$(MY_LOCAL_PATH)light_loader.cpp\
$(MY_LOCAL_PATH)light_oct_tree.cpp\
$(MY_LOCAL_PATH)light_processor.cpp\
$(MY_LOCAL_PATH)light_set.cpp\
$(MY_LOCAL_PATH)lighter.cpp\
$(MY_LOCAL_PATH)math_3d.cpp\
$(MY_LOCAL_PATH)quad_collide.cpp\
$(MY_LOCAL_PATH)quad_ground.cpp\
$(MY_LOCAL_PATH)quad_node.cpp\
$(MY_LOCAL_PATH)quad_tree.cpp\
$(MY_LOCAL_PATH)quad_visual.cpp\
$(MY_LOCAL_PATH)region_loader.cpp\
$(MY_LOCAL_PATH)rigid_loader.cpp\
$(MY_LOCAL_PATH)sea_base.cpp\
$(MY_LOCAL_PATH)sea_water.cpp\
$(MY_LOCAL_PATH)terrain_attr.cpp\
$(MY_LOCAL_PATH)terrain_base.cpp\
$(MY_LOCAL_PATH)terrain_brush.cpp\
$(MY_LOCAL_PATH)terrain_chunk.cpp\
$(MY_LOCAL_PATH)terrain_copy.cpp\
$(MY_LOCAL_PATH)terrain_culling.cpp\
$(MY_LOCAL_PATH)terrain_dump.cpp\
$(MY_LOCAL_PATH)terrain_editor.cpp\
$(MY_LOCAL_PATH)terrain_factory.cpp\
$(MY_LOCAL_PATH)terrain_grass.cpp\
$(MY_LOCAL_PATH)terrain_height.cpp\
$(MY_LOCAL_PATH)terrain_light.cpp\
$(MY_LOCAL_PATH)terrain_material.cpp\
$(MY_LOCAL_PATH)terrain_painter.cpp\
$(MY_LOCAL_PATH)terrain_region.cpp\
$(MY_LOCAL_PATH)terrain_rigid.cpp\
$(MY_LOCAL_PATH)terrain_visuals.cpp\
$(MY_LOCAL_PATH)terrain_walkable.cpp\
$(MY_LOCAL_PATH)terrain_water.cpp\
$(MY_LOCAL_PATH)terrain_zone.cpp\
$(MY_LOCAL_PATH)terrain.cpp\
$(MY_LOCAL_PATH)tex_alpha_map.cpp\
$(MY_LOCAL_PATH)tex_light_map.cpp\
$(MY_LOCAL_PATH)trigger.cpp\
$(MY_LOCAL_PATH)visual_loader.cpp\
$(MY_LOCAL_PATH)visual_manager.cpp\
$(MY_LOCAL_PATH)walk_bit_compress.cpp\
$(MY_LOCAL_PATH)walk_bit_map.cpp\
$(MY_LOCAL_PATH)walk_flag_compress.cpp\
$(MY_LOCAL_PATH)walk_flag_map.cpp\
$(MY_LOCAL_PATH)walk_generator_trace.cpp\
$(MY_LOCAL_PATH)walk_generator.cpp\
$(MY_LOCAL_PATH)walk_height_compress.cpp\
$(MY_LOCAL_PATH)walk_height_map.cpp\
$(MY_LOCAL_PATH)walk_loader.cpp\
$(MY_LOCAL_PATH)zone_manager.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_render
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_render/
LOCAL_C_INCLUDES:= \
$(LOCAL_CODE_PATH)extern/freetype/include\
$(LOCAL_CODE_PATH)extern/freeimage/include
LOCAL_MODULE    := libfm_render
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)color_rt.cpp\
$(MY_LOCAL_PATH)context.cpp\
$(MY_LOCAL_PATH)cube_tex_loader.cpp\
$(MY_LOCAL_PATH)depth_rt.cpp\
$(MY_LOCAL_PATH)device_caps.cpp\
$(MY_LOCAL_PATH)dynamic_ib.cpp\
$(MY_LOCAL_PATH)dynamic_tex.cpp\
$(MY_LOCAL_PATH)dynamic_vb.cpp\
$(MY_LOCAL_PATH)font_manager.cpp\
$(MY_LOCAL_PATH)font.cpp\
$(MY_LOCAL_PATH)frame_rt.cpp\
$(MY_LOCAL_PATH)helper.cpp\
$(MY_LOCAL_PATH)image_access.cpp\
$(MY_LOCAL_PATH)lib_main.cpp\
$(MY_LOCAL_PATH)libpvr.cpp\
$(MY_LOCAL_PATH)mempool2.cpp\
$(MY_LOCAL_PATH)paint_batch.cpp\
$(MY_LOCAL_PATH)painter.cpp\
$(MY_LOCAL_PATH)pixel_shader.cpp\
$(MY_LOCAL_PATH)render_state_op.cpp\
$(MY_LOCAL_PATH)render_resource.cpp\
$(MY_LOCAL_PATH)render_shader.cpp\
$(MY_LOCAL_PATH)render_texture.cpp\
$(MY_LOCAL_PATH)render_tool.cpp\
$(MY_LOCAL_PATH)render.cpp\
$(MY_LOCAL_PATH)res_thread.cpp\
$(MY_LOCAL_PATH)scene_view.cpp\
$(MY_LOCAL_PATH)shader_param_op.cpp\
$(MY_LOCAL_PATH)shader_program.cpp\
$(MY_LOCAL_PATH)static_cube.cpp\
$(MY_LOCAL_PATH)static_ib.cpp\
$(MY_LOCAL_PATH)static_tex.cpp\
$(MY_LOCAL_PATH)static_vb.cpp\
$(MY_LOCAL_PATH)tex_factory.cpp\
$(MY_LOCAL_PATH)tex_font.cpp\
$(MY_LOCAL_PATH)tex_loader.cpp\
$(MY_LOCAL_PATH)tex_tools.cpp\
$(MY_LOCAL_PATH)texture.cpp\
$(MY_LOCAL_PATH)vertex_shader.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_gui
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_net/
#LOCAL_CFLAGS := -DRAPIDXML_NO_EXCEPTIONS
LOCAL_MODULE    := libfm_net
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)custom_handler.cpp\
$(MY_LOCAL_PATH)game_client.cpp\
$(MY_LOCAL_PATH)game_encoder.cpp\
$(MY_LOCAL_PATH)game_object_set.cpp\
$(MY_LOCAL_PATH)game_object.cpp\
$(MY_LOCAL_PATH)game_prop_set.cpp\
$(MY_LOCAL_PATH)game_rec_client.cpp\
$(MY_LOCAL_PATH)game_rec_common.cpp\
$(MY_LOCAL_PATH)game_receiver.cpp\
$(MY_LOCAL_PATH)game_record_set.cpp\
$(MY_LOCAL_PATH)game_record.cpp\
$(MY_LOCAL_PATH)game_scene_obj.cpp\
$(MY_LOCAL_PATH)game_scene.cpp\
$(MY_LOCAL_PATH)game_sender.cpp\
$(MY_LOCAL_PATH)game_sock.cpp\
$(MY_LOCAL_PATH)game_view_obj.cpp\
$(MY_LOCAL_PATH)game_view.cpp\
$(MY_LOCAL_PATH)lib_main.cpp\
$(MY_LOCAL_PATH)sock_buf.cpp\
$(MY_LOCAL_PATH)win_socket.cpp\
$(LOCAL_CODE_PATH)extern/quicklz/quicklz.c
include $(BUILD_STATIC_LIBRARY)

#fm_game
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_game/
LOCAL_MODULE    := libfm_game
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)camera_3rd.cpp\
$(MY_LOCAL_PATH)camera_control.cpp\
$(MY_LOCAL_PATH)fight_net.cpp\
$(MY_LOCAL_PATH)form_gmcommand.cpp\
$(MY_LOCAL_PATH)form_login.cpp\
$(MY_LOCAL_PATH)form_mbox.cpp\
$(MY_LOCAL_PATH)form_qbox.cpp\
$(MY_LOCAL_PATH)game_scene.cpp\
$(MY_LOCAL_PATH)game.cpp\
$(MY_LOCAL_PATH)global.cpp\
$(MY_LOCAL_PATH)gui_utility.cpp\
$(MY_LOCAL_PATH)helper.cpp\
$(MY_LOCAL_PATH)lib_main.cpp\
$(MY_LOCAL_PATH)net.cpp\
$(MY_LOCAL_PATH)object_motion.cpp\
$(MY_LOCAL_PATH)role_composite.cpp\
$(MY_LOCAL_PATH)stage_client.cpp\
$(MY_LOCAL_PATH)stage_login.cpp\
$(MY_LOCAL_PATH)stage_main.cpp\
$(MY_LOCAL_PATH)stage_manager.cpp\
$(MY_LOCAL_PATH)stage_switchscene.cpp\
$(MY_LOCAL_PATH)stage_testmodel.cpp\
$(MY_LOCAL_PATH)utility.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_gui
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_gui/
#LOCAL_CFLAGS := -DRAPIDXML_NO_EXCEPTIONS
LOCAL_MODULE    := libfm_gui
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)adjuster.cpp\
$(MY_LOCAL_PATH)balloon_set.cpp\
$(MY_LOCAL_PATH)balloon.cpp\
$(MY_LOCAL_PATH)button.cpp\
$(MY_LOCAL_PATH)caret.cpp\
$(MY_LOCAL_PATH)check_button.cpp\
$(MY_LOCAL_PATH)color_data.cpp\
$(MY_LOCAL_PATH)combo_box.cpp\
$(MY_LOCAL_PATH)control_box.cpp\
$(MY_LOCAL_PATH)cursor.cpp\
$(MY_LOCAL_PATH)design_info.cpp\
$(MY_LOCAL_PATH)design_loader.cpp\
$(MY_LOCAL_PATH)design_res.cpp\
$(MY_LOCAL_PATH)design_saver.cpp\
$(MY_LOCAL_PATH)designer.cpp\
$(MY_LOCAL_PATH)edit.cpp\
$(MY_LOCAL_PATH)eventmanager.cpp\
$(MY_LOCAL_PATH)float_edit.cpp\
$(MY_LOCAL_PATH)font.cpp\
$(MY_LOCAL_PATH)form.cpp\
$(MY_LOCAL_PATH)grid.cpp\
$(MY_LOCAL_PATH)group_box.cpp\
$(MY_LOCAL_PATH)gui_msg_proc.cpp\
$(MY_LOCAL_PATH)gui_scroll.cpp\
$(MY_LOCAL_PATH)gui_utils.cpp\
$(MY_LOCAL_PATH)gui.cpp\
$(MY_LOCAL_PATH)i_container.cpp\
$(MY_LOCAL_PATH)i_control.cpp\
$(MY_LOCAL_PATH)i_scrollable.cpp\
$(MY_LOCAL_PATH)image_list.cpp\
$(MY_LOCAL_PATH)image_player.cpp\
$(MY_LOCAL_PATH)label.cpp\
$(MY_LOCAL_PATH)list_box.cpp\
$(MY_LOCAL_PATH)main_menu.cpp\
$(MY_LOCAL_PATH)mark_string.cpp\
$(MY_LOCAL_PATH)mem_pool.cpp\
$(MY_LOCAL_PATH)menu_item.cpp\
$(MY_LOCAL_PATH)menu.cpp\
$(MY_LOCAL_PATH)picture.cpp\
$(MY_LOCAL_PATH)progress_bar.cpp\
$(MY_LOCAL_PATH)radio_button.cpp\
$(MY_LOCAL_PATH)round_box.cpp\
$(MY_LOCAL_PATH)scroll_bar.cpp\
$(MY_LOCAL_PATH)selector.cpp\
$(MY_LOCAL_PATH)splitter.cpp\
$(MY_LOCAL_PATH)text_box.cpp\
$(MY_LOCAL_PATH)track_bar.cpp\
$(MY_LOCAL_PATH)track_rect.cpp\
$(MY_LOCAL_PATH)tree_node.cpp\
$(MY_LOCAL_PATH)tree_view.cpp
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := libfreeimage
LOCAL_SRC_FILES := libfreeimage.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libfreetype
LOCAL_SRC_FILES := libfreetype.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_main/
LOCAL_MODULE    := native-activity
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)app.cpp\
$(MY_LOCAL_PATH)android_native.cpp\
$(MY_LOCAL_PATH)../system/android_wstring.cpp
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2
LOCAL_STATIC_LIBRARIES := \
android_native_app_glue\
libfm_game\
libfm_net\
libfm_core\
libfm_math\
libfm_model\
libfm_world\
libfm_render\
libfm_gui\
libfm_terrain\
libfreeimage\
libfreetype
include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
