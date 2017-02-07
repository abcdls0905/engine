/*
#----------------------------------------------------------------------------------
# File:        tegra/native_detect/jni/nv_syscaps.h
# SDK Version: detect.2.0
# Email:       tegradev@nvidia.com
# Site:        http://developer.nvidia.com/
#
# Copyright (c) 2007-2013, NVIDIA CORPORATION.  All rights reserved.
#
# TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
# *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
# OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
# BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
# WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
# BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
# ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
# BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
#
#
#----------------------------------------------------------------------------------
*/

#ifndef __INCLUDED_NV_SYSCAPS_H
#define __INCLUDED_NV_SYSCAPS_H

typedef enum {
	TEGRA_SOC_ERROR		= -2, // detection method failed, error.
	TEGRA_SOC_NOTNV		= -1, // detect succeeded, but not a tegra
	TEGRA_SOC_UNKNOWN	= 0, // detect succeeded, seems like a tegra, not sure what though.
	TEGRA_SOC_T2, 			// likely a tegra 2
	TEGRA_SOC_T3,			// likely a tegra 3
	TEGRA_SOC_T4i,			// likely a tegra 4i
	TEGRA_SOC_T4,			// likely a tegra 4
	TEGRA_SOC_FUTURE,		// likely a future tegra
} Nv_TegraSocFamily;

typedef enum {
	TEGRA_GPU_ERROR		= -2,
	TEGRA_GPU_NOTNV		= -1,
	TEGRA_GPU_T2T3		= 0,
	TEGRA_GPU_T4T4i		= 1,
	TEGRA_GPU_FUTURE	= 2
} Nv_TegraGpuFamily;

typedef unsigned char	Nv_U8;
typedef unsigned int	Nv_U32;
typedef unsigned int	Nv_S32;

/*
typedef struct _NvSysCaps
{
	// copies of GL strings.  256 should be sufficient for our needs, I believe.
	char gl_vendor[256]; 
	char gl_version[256];
	char gl_renderer[256];

	// EGL/GL/GLES feature check flags
	Nv_U8 has_nvtime; // egl nvgetsystemtime -- all NV, and checks funcs! -- BUT could be removed
	Nv_U8 has_s3tc; // gles s3tc exists -- all NV - FOR NOW (could be competitor licenses..)
	Nv_U8 has_astc; // gles astc exists
	Nv_U8 has_pvrtc; // if exists, we're NOT on tegra.
	Nv_U8 has_atitc; // if exists, we're NOT on tegra.

	Nv_U8 has_nonnv_shaders; // if exists, we're NOT on tegra.
	Nv_U8 has_nlz; // gles depth_nonlinear exists -- T4-
	Nv_U8 has_fbfetch; // gles fb fetch exists -- T4-
	Nv_U8 has_csaa; // gles csaa exists -- T3-

	Nv_U8 has_shadow_cube; // gles -- T4+
	Nv_U8 has_msaa; // T4+
	Nv_U8 has_bindlesstex; // k+
	
	// EGL config feature check flags.
	Nv_U8 config_has_es3; // k+
	Nv_U8 config_has_opengl; // k+

	Nv_U8 config_has_csaa; // T3-
	Nv_U8 config_has_msaa; // T4+
	Nv_U8 config_has_nlz; // T4-
	Nv_U8 config_has_z24; // T4+		

	// cpu check/determination flags
	Nv_U32 mem_total_mb; // total memory exposed to user -- kernel, gpu, etc. can eat up a bunch at boot.
	Nv_U32 cpu_num_cores; // how many cores in the system; not all active atm, on some oses undependable.
	Nv_U32 cpu_max_mhz; // max core0 rating.  note that CPUs that can burst will run less than this in multicore.

	Nv_U32 cpu_vfp_version; // returns v3 vs v4 for now, or 0 for something unknown.
	Nv_U32 cpu_part_version; // lower half of part code.  for tegra, 0xc09 == A9, 0xc0a == A15

	Nv_U8 cpu_has_neon; // some ARM chips have neon, some don't.  t2 didn't, t3+ do for now..
	Nv_U8 cpu_is_x86; // ABI reports chip as x86 based.
	Nv_U8 dev_is_shield; // device labelled as a SHIELD device.
	Nv_U8 egl_was_ready; // whether egl was ready, if false then we skipped a bunch of lookups.. 

	// calculated values, based on analysis of above flags/state.
	Nv_U8 soc_may_be_tegra; // think we're running on a tegra via some basic checks	
	Nv_TegraGpuFamily gpu_likely_family; // what gpu family we think chip is. -1 .. n
	Nv_TegraSocFamily soc_likely_family; // what family we think chip is, WITHOUT sysfs check.  Enum
} NvSysCaps;

const NvSysCaps *nvGetSystemCapabilities(JNIEnv *env);
*/
Nv_TegraSocFamily nvGetTegraSocFamily(); // if available under newer Tegra BSPs.
Nv_TegraGpuFamily nvVerifyTegraGpuFamilyStrict(); // gpu class rating, <0==not tegra, 0==t2/t3, 1==t4/t4i, 2==mobile kepler
//bool nvIsOnShield(); // if running on product ID "SHIELD*"

#endif
