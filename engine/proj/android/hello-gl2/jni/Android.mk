# Copyright (C) 2009 The Android Open Source Project
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

# the purpose of this sample is to demonstrate how one can
# generate two distinct shared libraries and have them both
# uploaded in
#

LOCAL_PATH:= $(call my-dir)
LOCAL_CODE_PATH := ../../../../
LOCAL_INCLUDE_PATH := $(LOCAL_PATH)/../../../../
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
$(MY_LOCAL_PATH)pssm.cpp\
$(MY_LOCAL_PATH)post_effect_filter.cpp\
$(MY_LOCAL_PATH)post_effect.cpp\
$(MY_LOCAL_PATH)render_tex.cpp\
$(MY_LOCAL_PATH)saber_arc_manager.cpp\
$(MY_LOCAL_PATH)saber_arc.cpp\
$(MY_LOCAL_PATH)whip_data.cpp\
$(MY_LOCAL_PATH)whip.cpp\
$(MY_LOCAL_PATH)whip_manager.cpp\
$(MY_LOCAL_PATH)scene.cpp\
$(MY_LOCAL_PATH)skin.cpp\
$(MY_LOCAL_PATH)sky_box.cpp\
$(MY_LOCAL_PATH)shadow.cpp\
$(MY_LOCAL_PATH)shadow_manager.cpp\
$(MY_LOCAL_PATH)sun_glow.cpp\
$(MY_LOCAL_PATH)weather.cpp\
$(MY_LOCAL_PATH)world.cpp\
$(MY_LOCAL_PATH)dynamic_weather.cpp
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
$(MY_LOCAL_PATH)block_dynamic_walk.cpp\
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
$(MY_LOCAL_PATH)walk_i_d_map.cpp\
$(MY_LOCAL_PATH)light_map_config.cpp\
$(MY_LOCAL_PATH)dynamic_walk_flag_compress.cpp\
$(MY_LOCAL_PATH)dynamic_walk_generator.cpp\
$(MY_LOCAL_PATH)dynamic_walk_loader.cpp\
$(MY_LOCAL_PATH)dynamic_walkable.cpp\
$(MY_LOCAL_PATH)dynamic_walkable_manager.cpp\
$(MY_LOCAL_PATH)zone_manager.cpp
include $(BUILD_STATIC_LIBRARY)


#fm_render
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_render/
LOCAL_CFLAGS += -DKTX_OPENGL_ES2
LOCAL_C_INCLUDES:= \
$(LOCAL_INCLUDE_PATH)extern/freetype/include\
$(LOCAL_INCLUDE_PATH)fm_render/shader_optimizer/src/glsl\
$(LOCAL_INCLUDE_PATH)fm_render/shader_optimizer/include\
$(LOCAL_INCLUDE_PATH)fm_render/shader_optimizer/src/mesa\
$(LOCAL_INCLUDE_PATH)extern/freeimage/include
LOCAL_MODULE    := libfm_render
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)common_device.cpp\
$(MY_LOCAL_PATH)color_rt.cpp\
$(MY_LOCAL_PATH)context.cpp\
$(MY_LOCAL_PATH)cube_tex_loader.cpp\
$(MY_LOCAL_PATH)depth_rt.cpp\
$(MY_LOCAL_PATH)depthstencil_rt.cpp\
$(MY_LOCAL_PATH)depth_tex.cpp\
$(MY_LOCAL_PATH)color_tex.cpp\
$(MY_LOCAL_PATH)stencil_tex.cpp\
$(MY_LOCAL_PATH)depthstencil_tex.cpp\
$(MY_LOCAL_PATH)device_caps.cpp\
$(MY_LOCAL_PATH)nv_syscaps.cpp\
$(MY_LOCAL_PATH)dynamic_ib.cpp\
$(MY_LOCAL_PATH)dynamic_tex.cpp\
$(MY_LOCAL_PATH)dynamic_vb.cpp\
$(MY_LOCAL_PATH)font_manager.cpp\
$(MY_LOCAL_PATH)font.cpp\
$(MY_LOCAL_PATH)font_outline.cpp\
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
$(MY_LOCAL_PATH)scene_view_draw.cpp\
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
$(MY_LOCAL_PATH)render_draw_op.cpp\
$(MY_LOCAL_PATH)vertex_shader.cpp\
$(MY_LOCAL_PATH)tex_sampler.cpp\
$(MY_LOCAL_PATH)cache_file.cpp\
$(MY_LOCAL_PATH)cache_ini.cpp\
$(MY_LOCAL_PATH)cache_ini_loader.cpp\
$(MY_LOCAL_PATH)cache_loader.cpp\
$(MY_LOCAL_PATH)nv_hhdds.cpp\
$(MY_LOCAL_PATH)nv_images.cpp\
$(MY_LOCAL_PATH)ktx20/checkheader.c\
$(MY_LOCAL_PATH)ktx20/errstr.c\
$(MY_LOCAL_PATH)ktx20/etcdec.cxx\
$(MY_LOCAL_PATH)ktx20/etcunpack.cxx\
$(MY_LOCAL_PATH)ktx20/loader.c\
$(MY_LOCAL_PATH)ktx20/swap.c\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glcpp/glcpp-lex.c\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glcpp/glcpp-parse.c\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glcpp/pp.c\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ast_array_index.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ast_expr.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ast_function.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ast_to_hir.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ast_type.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/builtin_functions.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/builtin_types.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/builtin_variables.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_lexer.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_optimizer.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_parser.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_parser_extras.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_symbol_table.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_types.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/hir_field_selection.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_basic_block.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_builder.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_clone.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_constant_expression.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_equals.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_expression_flattening.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_function.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_function_can_inline.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_function_detect_recursion.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_hierarchical_visitor.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_hv_accept.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_import_prototypes.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_print_glsl_visitor.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_print_visitor.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_rvalue_visitor.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_stats.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_unused_structs.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_validate.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_variable_refcount.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_atomics.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_functions.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_interface_blocks.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_uniform_block_active_visitor.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_uniform_blocks.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_uniform_initializers.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_uniforms.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_varyings.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/linker.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/loop_analysis.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/loop_controls.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/loop_unroll.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_clip_distance.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_discard.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_discard_flow.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_if_to_cond_assign.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_instructions.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_jumps.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_mat_op_to_vec.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_named_interface_blocks.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_noise.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_output_reads.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_packed_varyings.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_packing_builtins.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_ubo_reference.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_variable_index_to_cond_assign.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_vec_index_to_cond_assign.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_vec_index_to_swizzle.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_vector.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_vector_insert.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_algebraic.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_array_splitting.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_constant_folding.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_constant_propagation.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_constant_variable.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_copy_propagation.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_copy_propagation_elements.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_cse.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_dead_builtin_varyings.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_dead_code.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_dead_code_local.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_dead_functions.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_flatten_nested_if_blocks.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_flip_matrices.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_function_inlining.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_if_simplification.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_noop_swizzle.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_redundant_jumps.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_structure_splitting.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_swizzle_swizzle.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_tree_grafting.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_vectorize.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ralloc.c\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/s_expression.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/standalone_scaffolding.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/strtod.c\
$(MY_LOCAL_PATH)shader_optimizer/src/mesa/main/hash_table.c\
$(MY_LOCAL_PATH)shader_optimizer/src/mesa/main/imports.c\
$(MY_LOCAL_PATH)shader_optimizer/src/mesa/program/prog_hash_table.c\
$(MY_LOCAL_PATH)shader_optimizer/src/mesa/program/symbol_table.c
include $(BUILD_STATIC_LIBRARY)

#fm_render_es3
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_render/
LOCAL_CFLAGS += -DRENDER_ES_3_0 -DKTX_OPENGL_ES3
LOCAL_C_INCLUDES:= \
$(LOCAL_INCLUDE_PATH)extern/freetype/include\
$(LOCAL_INCLUDE_PATH)fm_render/shader_optimizer/src/glsl\
$(LOCAL_INCLUDE_PATH)fm_render/shader_optimizer/include\
$(LOCAL_INCLUDE_PATH)fm_render/shader_optimizer/src/mesa\
$(LOCAL_INCLUDE_PATH)extern/freeimage/include
LOCAL_MODULE    := libfm_render_es3
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)common_device.cpp\
$(MY_LOCAL_PATH)color_rt.cpp\
$(MY_LOCAL_PATH)context.cpp\
$(MY_LOCAL_PATH)cube_tex_loader.cpp\
$(MY_LOCAL_PATH)depth_rt.cpp\
$(MY_LOCAL_PATH)depthstencil_rt.cpp\
$(MY_LOCAL_PATH)depth_tex.cpp\
$(MY_LOCAL_PATH)color_tex.cpp\
$(MY_LOCAL_PATH)stencil_tex.cpp\
$(MY_LOCAL_PATH)depthstencil_tex.cpp\
$(MY_LOCAL_PATH)device_caps.cpp\
$(MY_LOCAL_PATH)nv_syscaps.cpp\
$(MY_LOCAL_PATH)dynamic_ib.cpp\
$(MY_LOCAL_PATH)dynamic_tex.cpp\
$(MY_LOCAL_PATH)dynamic_vb.cpp\
$(MY_LOCAL_PATH)font_manager.cpp\
$(MY_LOCAL_PATH)font.cpp\
$(MY_LOCAL_PATH)font_outline.cpp\
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
$(MY_LOCAL_PATH)scene_view_draw.cpp\
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
$(MY_LOCAL_PATH)render_draw_op.cpp\
$(MY_LOCAL_PATH)vertex_shader.cpp\
$(MY_LOCAL_PATH)tex_sampler.cpp\
$(MY_LOCAL_PATH)cache_file.cpp\
$(MY_LOCAL_PATH)cache_ini.cpp\
$(MY_LOCAL_PATH)cache_ini_loader.cpp\
$(MY_LOCAL_PATH)cache_loader.cpp\
$(MY_LOCAL_PATH)nv_hhdds.cpp\
$(MY_LOCAL_PATH)nv_images.cpp\
$(MY_LOCAL_PATH)ktx20/checkheader.c\
$(MY_LOCAL_PATH)ktx20/errstr.c\
$(MY_LOCAL_PATH)ktx20/etcdec.cxx\
$(MY_LOCAL_PATH)ktx20/etcunpack.cxx\
$(MY_LOCAL_PATH)ktx20/loader.c\
$(MY_LOCAL_PATH)ktx20/swap.c\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glcpp/glcpp-lex.c\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glcpp/glcpp-parse.c\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glcpp/pp.c\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ast_array_index.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ast_expr.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ast_function.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ast_to_hir.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ast_type.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/builtin_functions.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/builtin_types.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/builtin_variables.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_lexer.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_optimizer.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_parser.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_parser_extras.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_symbol_table.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/glsl_types.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/hir_field_selection.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_basic_block.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_builder.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_clone.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_constant_expression.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_equals.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_expression_flattening.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_function.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_function_can_inline.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_function_detect_recursion.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_hierarchical_visitor.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_hv_accept.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_import_prototypes.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_print_glsl_visitor.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_print_visitor.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_rvalue_visitor.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_stats.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_unused_structs.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_validate.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ir_variable_refcount.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_atomics.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_functions.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_interface_blocks.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_uniform_block_active_visitor.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_uniform_blocks.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_uniform_initializers.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_uniforms.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/link_varyings.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/linker.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/loop_analysis.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/loop_controls.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/loop_unroll.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_clip_distance.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_discard.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_discard_flow.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_if_to_cond_assign.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_instructions.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_jumps.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_mat_op_to_vec.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_named_interface_blocks.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_noise.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_output_reads.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_packed_varyings.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_packing_builtins.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_ubo_reference.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_variable_index_to_cond_assign.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_vec_index_to_cond_assign.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_vec_index_to_swizzle.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_vector.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/lower_vector_insert.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_algebraic.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_array_splitting.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_constant_folding.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_constant_propagation.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_constant_variable.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_copy_propagation.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_copy_propagation_elements.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_cse.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_dead_builtin_varyings.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_dead_code.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_dead_code_local.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_dead_functions.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_flatten_nested_if_blocks.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_flip_matrices.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_function_inlining.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_if_simplification.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_noop_swizzle.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_redundant_jumps.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_structure_splitting.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_swizzle_swizzle.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_tree_grafting.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/opt_vectorize.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/ralloc.c\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/s_expression.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/standalone_scaffolding.cpp\
$(MY_LOCAL_PATH)shader_optimizer/src/glsl/strtod.c\
$(MY_LOCAL_PATH)shader_optimizer/src/mesa/main/hash_table.c\
$(MY_LOCAL_PATH)shader_optimizer/src/mesa/main/imports.c\
$(MY_LOCAL_PATH)shader_optimizer/src/mesa/program/prog_hash_table.c\
$(MY_LOCAL_PATH)shader_optimizer/src/mesa/program/symbol_table.c
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
$(MY_LOCAL_PATH)test_rt.cpp\
$(MY_LOCAL_PATH)utility.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_gui
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_gui/
#LOCAL_CFLAGS := -DRAPIDXML_NO_EXCEPTIONS
LOCAL_MODULE    := libfm_gui
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)./action_track.cpp\
$(MY_LOCAL_PATH)./adjuster.cpp\
$(MY_LOCAL_PATH)./animation.cpp\
$(MY_LOCAL_PATH)./animation_path.cpp\
$(MY_LOCAL_PATH)./balloon.cpp\
$(MY_LOCAL_PATH)./balloon_set.cpp\
$(MY_LOCAL_PATH)./button.cpp\
$(MY_LOCAL_PATH)./caret.cpp\
$(MY_LOCAL_PATH)./check_button.cpp\
$(MY_LOCAL_PATH)./combo_box.cpp\
$(MY_LOCAL_PATH)./control_box.cpp\
$(MY_LOCAL_PATH)./cursor.cpp\
$(MY_LOCAL_PATH)./edit.cpp\
$(MY_LOCAL_PATH)./float_edit.cpp\
$(MY_LOCAL_PATH)./form.cpp\
$(MY_LOCAL_PATH)./grid.cpp\
$(MY_LOCAL_PATH)./group_box.cpp\
$(MY_LOCAL_PATH)./gui_scroll.cpp\
$(MY_LOCAL_PATH)./i_container.cpp\
$(MY_LOCAL_PATH)./i_control.cpp\
$(MY_LOCAL_PATH)./i_scrollable.cpp\
$(MY_LOCAL_PATH)./image_list.cpp\
$(MY_LOCAL_PATH)./image_player.cpp\
$(MY_LOCAL_PATH)./label.cpp\
$(MY_LOCAL_PATH)./list_box.cpp\
$(MY_LOCAL_PATH)./main_menu.cpp\
$(MY_LOCAL_PATH)./mark_string.cpp\
$(MY_LOCAL_PATH)./mempool2.cpp\
$(MY_LOCAL_PATH)./menu.cpp\
$(MY_LOCAL_PATH)./menu_item.cpp\
$(MY_LOCAL_PATH)./multitext_box.cpp\
$(MY_LOCAL_PATH)./picture.cpp\
$(MY_LOCAL_PATH)./progress_bar.cpp\
$(MY_LOCAL_PATH)./radio_button.cpp\
$(MY_LOCAL_PATH)./rich_edit.cpp\
$(MY_LOCAL_PATH)./rich_input_box.cpp\
$(MY_LOCAL_PATH)./round_box.cpp\
$(MY_LOCAL_PATH)./scene_box.cpp\
$(MY_LOCAL_PATH)./scroll_bar.cpp\
$(MY_LOCAL_PATH)./selector.cpp\
$(MY_LOCAL_PATH)./splitter.cpp\
$(MY_LOCAL_PATH)./text_box.cpp\
$(MY_LOCAL_PATH)./track_bar.cpp\
$(MY_LOCAL_PATH)./track_rect.cpp\
$(MY_LOCAL_PATH)./tree_node.cpp\
$(MY_LOCAL_PATH)./tree_view.cpp\
$(MY_LOCAL_PATH)./color_data.cpp\
$(MY_LOCAL_PATH)./cool_manager.cpp\
$(MY_LOCAL_PATH)./data_image.cpp\
$(MY_LOCAL_PATH)./design_info.cpp\
$(MY_LOCAL_PATH)./design_loader.cpp\
$(MY_LOCAL_PATH)./design_res.cpp\
$(MY_LOCAL_PATH)./design_saver.cpp\
$(MY_LOCAL_PATH)./designer.cpp\
$(MY_LOCAL_PATH)./ellipse_progress_bar.cpp\
$(MY_LOCAL_PATH)./eventmanager.cpp\
$(MY_LOCAL_PATH)./flexstring.cpp\
$(MY_LOCAL_PATH)./font.cpp\
$(MY_LOCAL_PATH)./group_map.cpp\
$(MY_LOCAL_PATH)./group_scrollable_box.cpp\
$(MY_LOCAL_PATH)./gui.cpp\
$(MY_LOCAL_PATH)./gui_msg_proc.cpp\
$(MY_LOCAL_PATH)./gui_utils.cpp\
$(MY_LOCAL_PATH)./hyperlinkstyle_manager.cpp\
$(MY_LOCAL_PATH)./image_control_grid.cpp\
$(MY_LOCAL_PATH)./image_data.cpp\
$(MY_LOCAL_PATH)./image_grid.cpp\
$(MY_LOCAL_PATH)./imageanimation_manager.cpp\
$(MY_LOCAL_PATH)./lib_main.cpp\
$(MY_LOCAL_PATH)./text.cpp\
$(MY_LOCAL_PATH)./tinyhtml.cpp\
$(MY_LOCAL_PATH)./tree_node_ex.cpp\
$(MY_LOCAL_PATH)./string_util.cpp\
$(MY_LOCAL_PATH)./tree_view_ex.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_sound
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_sound/
LOCAL_C_INCLUDES:= $(LOCAL_INCLUDE_PATH)extern/fmod/inc
LOCAL_MODULE    := libfm_sound
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)lib_main.cpp\
$(MY_LOCAL_PATH)regclass.cpp\
$(MY_LOCAL_PATH)soudsystem.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_geometry
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_geometry/
LOCAL_MODULE    := libfm_geometry
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)geometry.cpp\
$(MY_LOCAL_PATH)geometry_builder.cpp\
$(MY_LOCAL_PATH)primitive_3d_drawer.cpp\
$(MY_LOCAL_PATH)lib_main.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_movie
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_movie/
LOCAL_MODULE    := libfm_movie
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)camera_wrapper.cpp\
$(MY_LOCAL_PATH)key_frame_controller.cpp\
$(MY_LOCAL_PATH)object_loader.cpp\
$(MY_LOCAL_PATH)pyramid.cpp\
$(MY_LOCAL_PATH)time_axis.cpp\
$(MY_LOCAL_PATH)time_axis_file.cpp\
$(MY_LOCAL_PATH)helper.cpp\
$(MY_LOCAL_PATH)rapid_xml_helper.cpp\
$(MY_LOCAL_PATH)utility.cpp\
$(MY_LOCAL_PATH)lib_main.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_ui3d
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_ui3d/
LOCAL_MODULE    := libfm_ui3d
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)animation.cpp\
$(MY_LOCAL_PATH)global.cpp\
$(MY_LOCAL_PATH)helper.cpp\
$(MY_LOCAL_PATH)lib_main.cpp\
$(MY_LOCAL_PATH)mesh.cpp\
$(MY_LOCAL_PATH)node.cpp\
$(MY_LOCAL_PATH)render_ui.cpp\
$(MY_LOCAL_PATH)shader.cpp\
$(MY_LOCAL_PATH)ui3d_core.cpp\
$(MY_LOCAL_PATH)ui3d_helper.cpp\
$(MY_LOCAL_PATH)visnode.cpp
include $(BUILD_STATIC_LIBRARY)

#fm_physics_bullet
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_physics_bullet/
LOCAL_C_INCLUDES := $(LOCAL_INCLUDE_PATH)fm_physics_bullet/bullet/
LOCAL_MODULE    := fm_physics_bullet
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)bullet/bullet_collision/broadphase_collision/bt_axis_sweep_3.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/broadphase_collision/bt_broadphase_proxy.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/broadphase_collision/bt_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/broadphase_collision/bt_dbvt.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/broadphase_collision/bt_dbvt_broadphase.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/broadphase_collision/bt_dispatcher.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/broadphase_collision/bt_multi_sap_broadphase.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/broadphase_collision/bt_overlapping_pair_cache.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/broadphase_collision/bt_quantized_bvh.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/broadphase_collision/bt_simple_broadphase.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_activating_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_box_2d_box_2d_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_box_box_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_box_box_detector.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_collision_dispatcher.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_collision_object.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_collision_world.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_compound_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_compound_compound_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_convex_2d_convex_2d_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_convex_concave_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_convex_convex_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_convex_plane_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_default_collision_configuration.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_empty_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_ghost_object.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_hashed_simple_pair_cache.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_internal_edge_utility.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_manifold_result.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_simulation_island_manager.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_sphere_box_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_sphere_sphere_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_sphere_triangle_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/bt_union_find.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_dispatch/sphere_triangle_detector.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_box_2d_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_box_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_bvh_triangle_mesh_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_capsule_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_collision_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_compound_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_concave_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_cone_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_convex_2d_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_convex_hull_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_convex_internal_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_convex_point_cloud_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_convex_polyhedron.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_convex_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_convex_triangle_mesh_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_cylinder_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_empty_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_heightfield_terrain_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_minkowski_sum_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_multimaterial_triangle_mesh_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_multi_sphere_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_optimized_bvh.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_polyhedral_convex_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_scaled_bvh_triangle_mesh_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_shape_hull.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_sphere_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_static_plane_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_striding_mesh_interface.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_tetrahedron_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_triangle_buffer.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_triangle_callback.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_triangle_index_vertex_array.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_triangle_index_vertex_material_array.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_triangle_mesh.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_triangle_mesh_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/collision_shapes/bt_uniform_scaling_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/bt_contact_processing.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/bt_generic_pool_allocator.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/bt_g_impact_bvh.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/bt_g_impact_collision_algorithm.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/bt_g_impact_quantized_bvh.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/bt_g_impact_shape.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/bt_triangle_shape_ex.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/gim_box_set.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/gim_contact.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/gim_memory.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/gimpact/gim_tri_collision.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_continuous_convex_collision.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_convex_cast.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_gjk_convex_cast.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_gjk_epa_2.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_gjk_epa_penetration_depth_solver.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_gjk_pair_detector.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_minkowski_penetration_depth_solver.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_persistent_manifold.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_polyhedral_contact_clipping.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_raycast_callback.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_sub_simplex_convex_cast.cpp\
$(MY_LOCAL_PATH)bullet/bullet_collision/narrow_phase_collision/bt_voronoi_simplex_solver.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/character/bt_kinematic_character_controller.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_cone_twist_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_contact_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_fixed_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_gear_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_generic_6_dof_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_generic_6_dof_spring_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_hinge_2_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_hinge_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_point_2_point_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_sequential_impulse_constraint_solver.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_slider_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_solve_2_linear_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_typed_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/constraint_solver/bt_universal_constraint.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/dynamics/bt_discrete_dynamics_world.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/dynamics/bt_rigid_body.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/dynamics/bt_simple_dynamics_world.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/dynamics/bullet-c-a_p_i.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/vehicle/bt_raycast_vehicle.cpp\
$(MY_LOCAL_PATH)bullet/bullet_dynamics/vehicle/bt_wheel_info.cpp\
$(MY_LOCAL_PATH)bullet/linear_math/bt_aligned_allocator.cpp\
$(MY_LOCAL_PATH)bullet/linear_math/bt_convex_hull.cpp\
$(MY_LOCAL_PATH)bullet/linear_math/bt_convex_hull_computer.cpp\
$(MY_LOCAL_PATH)bullet/linear_math/bt_geometry_util.cpp\
$(MY_LOCAL_PATH)bullet/linear_math/bt_polar_decomposition.cpp\
$(MY_LOCAL_PATH)bullet/linear_math/bt_quickprof.cpp\
$(MY_LOCAL_PATH)bullet/linear_math/bt_serializer.cpp\
$(MY_LOCAL_PATH)bullet/linear_math/bt_vector_3.cpp\
$(MY_LOCAL_PATH)collision_shape.cpp\
$(MY_LOCAL_PATH)constraint.cpp\
$(MY_LOCAL_PATH)debug_drawer.cpp\
$(MY_LOCAL_PATH)my_motion_state.cpp\
$(MY_LOCAL_PATH)physics_bullet.cpp\
$(MY_LOCAL_PATH)ragdoll.cpp\
$(MY_LOCAL_PATH)rigid_body.cpp\
$(MY_LOCAL_PATH)lib_main.cpp
include $(BUILD_STATIC_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE    := libfreeimage
#LOCAL_SRC_FILES := libfreeimage.a
#include $(PREBUILT_STATIC_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE    := libfreetype
#LOCAL_SRC_FILES := libfreetype.a
#include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_CODE_PATH)fm_main/
LOCAL_MODULE    := libapp
LOCAL_SRC_FILES := \
$(MY_LOCAL_PATH)android_app.cpp\
$(MY_LOCAL_PATH)app.cpp\
$(MY_LOCAL_PATH)jnihelper.cpp\
$(MY_LOCAL_PATH)../system/android_wstring.cpp
LOCAL_LDLIBS    := -llog -lGLESv2 -lEGL -landroid
LOCAL_STATIC_LIBRARIES := \
libfm_game\
libfm_net\
libfm_core\
libfm_model\
libfm_world\
libfm_render\
libfm_gui\
libfm_math\
libfm_terrain\
libfm_sound\
libfm_geometry\
libfm_movie\
libfm_ui3d\
libfm_physics_bullet

LOCAL_STATIC_LIBRARIES += libfreeimage
LOCAL_STATIC_LIBRARIES += libfreetype
#LOCAL_WHOLE_STATIC_LIBRARIES += libfreeimage
#LOCAL_WHOLE_STATIC_LIBRARIES += libfreetype


include $(BUILD_SHARED_LIBRARY)



$(call import-add-path,$(LOCAL_INCLUDE_PATH))
 

$(call import-module,extern/freeimage)
$(call import-module,extern/freetype)
