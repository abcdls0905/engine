/*
#----------------------------------------------------------------------------------
# File:        tegra/native_detect/jni/nv_syscaps.cpp
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

#include <jni.h>

#include "nv_syscaps.h"

#include <GLES2/gl2.h>

// for now, we'll include the time proc decls/defs inline here.
#include <EGL/eglplatform.h>
#include <EGL/egl.h>

#ifndef EGL_OPENGL_ES3_BIT
#define EGL_OPENGL_ES3_BIT (1<<6)
#endif

#ifndef EGL_NV_system_time
#define EGL_NV_system_time 1
typedef khronos_uint64_t EGLuint64NV;
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLuint64NV EGLAPIENTRY eglGetSystemTimeFrequencyNV(void);
EGLAPI EGLuint64NV EGLAPIENTRY eglGetSystemTimeNV(void);
#endif
typedef EGLuint64NV (EGLAPIENTRYP PFNEGLGETSYSTEMTIMEFREQUENCYNVPROC)(void);
typedef EGLuint64NV (EGLAPIENTRYP PFNEGLGETSYSTEMTIMENVPROC)(void);
#endif

#include <android/log.h>
#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE,  \
											 "syscaps", \
											 __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,  \
											 "syscaps", \
											 __VA_ARGS__))
// force debugging/logging.
#ifndef _DEBUG
//#define _DEBUG
#endif

#ifdef _DEBUG
//#define LOG_DATA_READS
#define LOGDV(...) LOGV(__VA_ARGS__)
#define LOGEV(...) LOGE(__VA_ARGS__)
#else
#define LOGDV(...) 
#define LOGEV(...) 
#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <errno.h>
#include <stdio.h>

#include <zlib.h>

// internal funcs.
static bool queryEGLConfigFeatures();
static bool isCpuX86(JNIEnv *env);
static bool isDeviceShield(JNIEnv *env);


#define MAX_DATA_LEN	4096

// we keep a static copy of the detect caps here for debugging,
// as it's small enough to do so.
//static NvSysCaps s_sysCaps;

static char findStringTempData[MAX_DATA_LEN];

#define MAX_SEPARATORS	6
char separators[MAX_SEPARATORS] = {' ', '\t', '\n', '\r', '\0', ':'};

bool charIsSeparator(char c)
{
	for (int i=0; i<MAX_SEPARATORS; i++)
	{
		if (c==separators[i])
			return true;
	}
	return false;
}

#define MAX_NEXT_TOKEN	1024 // should be more than sufficient for scanning lines.
static char foundToken[MAX_NEXT_TOKEN];
const char *findDataNextToken(const char *data, int dataLen)
{
	if (data==NULL || dataLen<=0) return NULL;
	
	const char *s = data;
	const char *outstr = NULL;
	int len = 0;

	// we presume start of the data is a sep of some sort, but skip extra seps until chars.
	while (charIsSeparator(*s) && (s < data+dataLen))
		s++;

	outstr = s; // cache start of string.  len will give us end.
	while (!charIsSeparator(*s) && (s < data+dataLen))
	{
		s++;
		len++;
		// check if out of room in dst buffer
		if (len==MAX_NEXT_TOKEN-1)
			break;
	}

	if (len>0)
		strncpy(foundToken, outstr, len);
	foundToken[len] = 0; // null terminate the token.

	return foundToken;
}


const char *findDataString(const char *data, const char *str, const int maxlen)
{
	if (data==NULL || str==NULL) return NULL;
	const char *start = data;
	const char *outstr = NULL;
	int slen = strlen(str);
	int len = 0;
	char pc, nc;

	//LOGV("!!> findstring = %s", str);
	while (len + slen <= maxlen)
	{
		// easiest is to use strstr.
		// doesn't guarantee whitespace or separators, but good enough for now
		outstr = strstr(start, str);
		if (NULL==outstr)
			return NULL;

		// then look ahead and behind to ensure both are terminators.
		// first, behind.  if we're at start of data, that's our term.
		if ( (outstr == data) || (charIsSeparator(*(outstr-1))) )
		{
			// then, ahead
			nc = *(outstr+slen);
			if (charIsSeparator(nc))
			{ // fine, done.
				return outstr;
			}
		}

		// else, not bracketed, no match
		start = outstr+slen;
		len = (int)(start-data);
	}

	return outstr;
}


const char *findDataStringEOL(const char *data, const char *str)
{
	if (data==NULL || str==NULL) return NULL;
	
	// quickly find EOL and use that length for search termination.
	int len = 0;
	const char *d = data;
	while (*d && *d!='\n' && *d!='\r') // just EOL variants: null, lf, cr
	{
		d++;
		len++;
	}

	// copy to temp string
	strncpy(findStringTempData, data, len);
	findStringTempData[len] = 0;

	return findDataString(findStringTempData, str, len);
}


int readDataFile(const char *fname, char *outbuf, size_t buflen)
{
	if (buflen==0)
		return -1;

	if (NULL==outbuf)
		return -1;

	if (NULL==fname)
		return -1;

    FILE *f = fopen(fname, "r");
    if (NULL == f)
    {
        int errsv = errno;
    	LOGDV("##> Unable to read from: %s  \n##>> errno: %d", fname, errsv);
    	return -1;
    }

	int count = fread(outbuf, 1, buflen, f);
	fclose(f);

	// sanity null-term the buffer.
	outbuf[buflen-1] = 0;

#ifdef LOG_DATA_READS
	LOGV(outbuf);
#endif
	
	return count;
}


static void logExtensionsStrings(const char *exts)
{
#ifdef _DEBUG
	if (exts==NULL)
		return;
	
	// for debugging...  print out the list of extensions supported.
	char databuf[MAX_DATA_LEN]; // something more than reasonable.
	const char *efinish = exts;
	const char *estart = efinish;
	int len = 0;
	while (*efinish)
	{
		const char c = *efinish;
		efinish++;
		len++;
		if (charIsSeparator(c))
		{
			if (len>0)
			{
				strncpy(databuf, estart, len);
				databuf[len] = 0;
				LOGDV("..> %s", databuf);
			}
			estart = efinish;
			len = 0;
		}
	}

	if (len>0)
	{
		strncpy(databuf, estart, len);
		databuf[len] = 0;
		LOGDV("..> %s", databuf);
	}
#endif
}

/*
const NvSysCaps *nvGetSystemCapabilities(JNIEnv *env)
{
	char databuf[MAX_DATA_LEN]; // something more than reasonable.
	int dataLen;
	const GLubyte *tmps;
    const char *exts;
    char *offs;

    // clear static buffer.
    memset(&s_sysCaps, 0, sizeof(NvSysCaps));

	if (EGL_NO_CONTEXT == eglGetCurrentContext())
	{
		LOGE("!!> GetSystemCapabilities called with invalid EGL context.");
		LOGE("!!> All EGL/GLES feature queries will be skipped.");
	}
	else
	{
		// egl context is okay, good to go.
		s_sysCaps.egl_was_ready = true;
    	
		// then we're safe to go poking around EGL/GLES features to
		// detect what kind/class/family of chip we're running on.
		
		EGLDisplay disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	
		LOGDV("!!> EGL_EXTENSIONS:");
		exts = (const char*) eglQueryString(disp, EGL_EXTENSIONS);
		logExtensionsStrings(exts);
	
		// cache the extensions string for the next bunch of tests...
		LOGDV("!!> GL_EXTENSIONS:");
		exts = (const char*) glGetString(GL_EXTENSIONS);
		logExtensionsStrings(exts);
	
		// check for interesting GL strings..
		tmps = glGetString(GL_VENDOR);
		LOGV("##> GL_VENDOR: %s", tmps);
		strncpy(s_sysCaps.gl_vendor, (char*)tmps, 255);
		s_sysCaps.gl_vendor[255]=0;
	
		tmps = glGetString(GL_RENDERER);
		LOGV("##> GL_RENDERER: %s", tmps);
		strncpy(s_sysCaps.gl_renderer, (char*)tmps, 255);
		s_sysCaps.gl_renderer[255]=0;
	
		tmps = glGetString(GL_VERSION);
		LOGV("##> GL_VERSION: %s", tmps);
		strncpy(s_sysCaps.gl_version, (char*)tmps, 255);
		s_sysCaps.gl_version[255]=0;
	
		// check time extension by using EGL direct.
		PFNEGLGETSYSTEMTIMEFREQUENCYNVPROC m_eglGetSystemTimeFrequencyNVProc;
		PFNEGLGETSYSTEMTIMENVPROC m_eglGetSystemTimeNVProc;
		EGLuint64NV m_eglSystemTimeFrequency;
		m_eglGetSystemTimeFrequencyNVProc = (PFNEGLGETSYSTEMTIMEFREQUENCYNVPROC) eglGetProcAddress("eglGetSystemTimeFrequencyNV");
		m_eglGetSystemTimeNVProc = (PFNEGLGETSYSTEMTIMENVPROC) eglGetProcAddress("eglGetSystemTimeNV");
		// now, we'll proceed through a series of sanity checking.
		// if they all succeed, we're good and set true
		// if any fail, we fall out of conditional tests and leave false.
		if (m_eglGetSystemTimeFrequencyNVProc &&
			m_eglGetSystemTimeNVProc)
		{
			m_eglSystemTimeFrequency = m_eglGetSystemTimeFrequencyNVProc();
			if (m_eglSystemTimeFrequency>0) // assume okay.  quick-check it works.
			{
				EGLuint64NV time1, time2;
				time1 = m_eglGetSystemTimeNVProc();
				usleep(2000); // 2ms should be MORE than sufficient, right?
				time2 = m_eglGetSystemTimeNVProc();
				if (time1 != time2) // quick sanity only...
				{
					// we've sanity checked:
					// - fn pointers non-null
					// - freq non-zero
					// - two calls to time sep'd by sleep non-equal
					// safe to set true now.
					s_sysCaps.has_nvtime = true;
				}
			}
		}
		LOGV("##> Has NvTime: %s", s_sysCaps.has_nvtime ? "true" : "false");
	
		// look for extension matches.
		offs = strstr(exts, "GL_EXT_texture_compression_s3tc");
		if (NULL != offs)
			s_sysCaps.has_s3tc = true;
		LOGV("##> Has S3TC: %s", s_sysCaps.has_s3tc ? "true" : "false");
	
		offs = strstr(exts, "GL_KHR_texture_compression_astc_ldr");
		if (NULL != offs)
			s_sysCaps.has_astc = true;
		LOGV("##> Has ASTC: %s", s_sysCaps.has_astc ? "true" : "false");
	
		offs = strstr(exts, "GL_IMG_texture_compression_pvrtc");
		if (NULL != offs)
			s_sysCaps.has_pvrtc = true;
		LOGV("##> Has PVRTC: %s", s_sysCaps.has_pvrtc ? "true" : "false");
	
		// trying some older variants for ATITC just for yuks.
		offs = strstr(exts, "GL_AMD_compressed_ATC_texture");
		if (NULL != offs)
			s_sysCaps.has_atitc = true;
		offs = strstr(exts, "GL_ATI_texture_compression_atitc");
		if (NULL != offs)
			s_sysCaps.has_atitc = true;
		offs = strstr(exts, "GL_ATI_compressed_texture_atitc");
		if (NULL != offs)
			s_sysCaps.has_atitc = true;
		LOGV("##> Has ATITC: %s", s_sysCaps.has_atitc ? "true" : "false");
		
		offs = strstr(exts, "GL_NV_depth_nonlinear");
		if (NULL != offs)
			s_sysCaps.has_nlz = true;
		LOGV("##> Has NLZ: %s", s_sysCaps.has_nlz ? "true" : "false");
	
		offs = strstr(exts, "GL_NV_coverage_sample");
		if (NULL != offs)
			s_sysCaps.has_csaa = true;
		LOGV("##> Has CSAA: %s", s_sysCaps.has_csaa ? "true" : "false");
	
		offs = strstr(exts, "GL_NV_shader_framebuffer_fetch");
		if (NULL != offs)
			s_sysCaps.has_fbfetch = true;
		LOGV("##> Has fbfetch: %s", s_sysCaps.has_fbfetch ? "true" : "false");
	
		offs = strstr(exts, "GL_NV_framebuffer_multisample");
		if (NULL != offs)
			s_sysCaps.has_msaa = true;
		LOGV("##> Has MSAA: %s", s_sysCaps.has_msaa ? "true" : "false");
	
		offs = strstr(exts, "GL_NV_shadow_samplers_cube");
		if (NULL != offs)
			s_sysCaps.has_shadow_cube = true;
		LOGV("##> Has shadow samplers cube: %s", s_sysCaps.has_shadow_cube ? "true" : "false");
	
		offs = strstr(exts, "GL_NV_bindless_texture");
		if (NULL != offs)
			s_sysCaps.has_bindlesstex = true;
		LOGV("##> Has bindless: %s", s_sysCaps.has_bindlesstex ? "true" : "false");
	
		// catch competitor program binaries...
		if (NULL != strstr(exts, "GL_IMG_program_binary")) {
			LOGV("##> Has img program binary.");			
			s_sysCaps.has_nonnv_shaders = true;
		} else if (NULL != strstr(exts, "GL_AMD_program_binary")) { // normally _Z400 in modern chips. this is a shorter catch-all.
			LOGV("##> Has amd program binary.");			
			s_sysCaps.has_nonnv_shaders = true;
		} else if (NULL != strstr(exts, "GL_ARM_mali_program_binary")) {
			LOGV("##> Has arm program binary.");			
			s_sysCaps.has_nonnv_shaders = true;
		} else if (NULL != strstr(exts, "GL_VIV_shader_binary")) {
			LOGV("##> Has viv shader binary.");			
			s_sysCaps.has_nonnv_shaders = true;
		}
		//LOGDV("##> Has non-NV program/shader binary: %s", s_sysCaps.has_nonnv_shaders ? "true" : "false");
	
		// and let's call helper fn to fill in config exposed features...
		queryEGLConfigFeatures();
	}
	
    // find number of cpu cores.
    int cpuCount = 0;
    // approaches:
    // 1) sysconf isn't working for K32, K36 it seems to, so for HC+ app might be ok.
    //    cpuCount = sysconf(_SC_NPROCESSORS_CONF);
    // 2) can't use num_possible_cpus() or other cpumask.h function under android ndk,
    //    as headers won't compile.
    // 3) leaves us with reading sysfs entry for possible or present cpu string.
    dataLen = readDataFile("/sys/devices/system/cpu/present", databuf, MAX_DATA_LEN);
	if (dataLen>1 && dataLen<16) // some range that sounds decent to sanity clamp.
	{
		// in case of a LF or other termination, use our token function.
		const char *findstr = findDataNextToken(databuf, dataLen);
		if (findstr!=NULL)
		{
			int fcount = strnlen(findstr, dataLen);
			// process the string.
			// format is '0' for one core, '0-1' for two, '0-3' for four.
			if (fcount==1) // if single character, there's only one core.
				cpuCount = 1;
			else
			{
				findstr += 2; // that jumps over "0-".
				cpuCount = 1 + atoi(findstr); // since indices are zero-based.
				if (cpuCount<1)
					cpuCount = 1;
			}
		}
	}
	else
	{
		LOGE("##> Unable to read CPU cores present.");
	}
    if (cpuCount>0)
    {
    	s_sysCaps.cpu_num_cores = cpuCount;
    	LOGDV("##> CPU Cores: %d", s_sysCaps.cpu_num_cores);
    }

	// look up cpu frequency info, max hz.
    dataLen = readDataFile("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", databuf, MAX_DATA_LEN);
	if (dataLen>3 && dataLen<16) // some range that sounds decent to sanity clamp.
	{
		int mhz = atoi(databuf) / 1000;
		if (mhz<1)
			mhz = 0; // min clamp
		s_sysCaps.cpu_max_mhz = mhz;
		LOGV("##> CPU0 max freq: %dMHz", s_sysCaps.cpu_max_mhz);
	}
	else
	{
		LOGE("##> Unable to read CPU0 max freq");
	}
	
	// look up further cpu info, arch level, neon, etc.
	dataLen = readDataFile("/proc/cpuinfo", databuf, MAX_DATA_LEN);
	databuf[dataLen] = 0; // null terminate just in case.
	if (dataLen>10) // should be 100s of chars, just some min clamp.
	{
		LOGDV("##> cpuinfo: %s", databuf);
		// find Features strings
		const char *features = findDataString(databuf, "Features", dataLen);
		const char *findstr;
		if (NULL!=features)
		{
			findstr = findDataStringEOL(features, "neon");
			s_sysCaps.cpu_has_neon = (NULL!=findstr);
			LOGV("##> CPU %s NEON", s_sysCaps.cpu_has_neon ? "has" : "doesn't have");

			findstr = findDataStringEOL(features, "vfpv4");
			if (NULL!=findstr)
				s_sysCaps.cpu_vfp_version = 4;
			else
			{
				findstr = findDataStringEOL(features, "vfpv3");
				if (NULL!=findstr)
					s_sysCaps.cpu_vfp_version = 3;
			}
			LOGV("##> CPU is VFPv%d", s_sysCaps.cpu_vfp_version);
		}
		else
		{
			features = strstr(databuf, "Features");
			if (NULL!=features)
			{
				LOGE("!!> CPU features string found with strstr only, len %d", strlen(features));
			}
		}

		// find cpu part
#if 0
		char arch[32]
		const char *tmparch = findDataString(databuf, "CPU architecture", dataLen);
		tmparch = findDataNextToken(tmparch+strlen("CPU architecture"));
		tmparch = findDataNextToken(tmparch);
		strcpy(arch, tmparch);
		if (NULL!=arch)
		{
			s_sysCaps.cpu_arch_version = strtol(arch, NULL, 0);
			LOGDV("##> CPU arch = %d", s_sysCaps.cpu_arch_version);
		}
		else
		{
			LOGE("!!> CPU arch string not found.");
		}
#endif
		
		char part[32];
		const char *cpupart = "CPU part";
		const char *tmppart = findDataString(databuf, cpupart, dataLen);
		if (tmppart!=NULL)
		{
			tmppart += strlen(cpupart);
			tmppart = findDataNextToken(tmppart, dataLen - ((unsigned long)tmppart - (unsigned long)databuf));
		}
		if (tmppart!=NULL)
			tmppart = findDataNextToken(tmppart, dataLen - ((unsigned long)tmppart - (unsigned long)databuf));
		if (tmppart!=NULL)
		{
			s_sysCaps.cpu_part_version = strtol(tmppart, NULL, 0);
			LOGDV("##> CPU part = 0x%x", s_sysCaps.cpu_part_version);
		}		
		else
		{
			LOGE("!!> CPU part string not found.");
		}
	}

	// look up some memory info.
	dataLen = readDataFile("/proc/meminfo", databuf, MAX_DATA_LEN);
	if (dataLen>10) // should be 100s of chars, just some min clamp.
	{
		// find total memory for now
		const char *mts = "MemTotal:";
		const char *findstr = findDataString(databuf, mts, dataLen);
		if (NULL!=findstr)
		{
			findstr += strlen(mts);
			const char *tok = findDataNextToken(findstr, dataLen - ((unsigned long)findstr - (unsigned long)databuf));
			if (NULL!=tok)
			{
				s_sysCaps.mem_total_mb = atoi(tok) / 1024; // kB->MB.
				LOGDV("##> Total RAM: %dMB", s_sysCaps.mem_total_mb);
			}
			else
			{
				LOGE("!!> MemTotal not found.");
			}
		}
	}
	else
	{
		LOGE("!!> cpuinfo not found.");
	}

	// environment lookup fns.
	if (env==NULL) {
		LOGE("!!> Couldn't query some device caps as null JNIEnv provided.");
	} else {
		s_sysCaps.cpu_is_x86 = isCpuX86(env);
		s_sysCaps.dev_is_shield = isDeviceShield(env);
	}

	// then, we cross check some of the config flags against the extensions.
	if (s_sysCaps.config_has_csaa != s_sysCaps.has_csaa)
		s_sysCaps.has_csaa = false;
	if (s_sysCaps.config_has_nlz != s_sysCaps.has_nlz)
		s_sysCaps.has_nlz = false;

	// while we clear to zero, explicitly set these to desired values.
	s_sysCaps.soc_may_be_tegra = false;
	s_sysCaps.gpu_likely_family = TEGRA_GPU_ERROR; // error value
	s_sysCaps.soc_likely_family = TEGRA_SOC_ERROR; // error value

	//===============================================================
	// now, for combination/deduced variables...
	
	// first, if have competitor TC or shaders, or on X86, definitely NOT tegra, so we punt.
	if (s_sysCaps.has_pvrtc || s_sysCaps.has_atitc || s_sysCaps.has_nonnv_shaders
			|| s_sysCaps.cpu_is_x86 )
	{
		s_sysCaps.gpu_likely_family = TEGRA_GPU_NOTNV; // not a tegra
		s_sysCaps.soc_likely_family = TEGRA_SOC_NOTNV; // not a tegra
	}
	else
	{ // pretty high probability of it being a tegra in absence of those things
		// but we want enforced positive result here.
		
		// check sets of gles/cpu features of each level. 
		// if we have NLZ, fbfetch, or drawpath, that's t2-t4...
		if (s_sysCaps.has_nlz
			&& (s_sysCaps.has_fbfetch || s_sysCaps.has_csaa)) // || csaa just in case fbfetch ever goes away...
		{
			s_sysCaps.soc_may_be_tegra = true;
			// look for T4 first, else assume T3/T2....
			if (!s_sysCaps.has_csaa
				&& s_sysCaps.has_shadow_cube
				&& (s_sysCaps.has_msaa || s_sysCaps.config_has_msaa)
				&& s_sysCaps.config_has_z24)
			{
				s_sysCaps.gpu_likely_family = TEGRA_GPU_T4T4i;
				s_sysCaps.soc_likely_family = TEGRA_SOC_T4;
				if (s_sysCaps.cpu_part_version==0xc09 || s_sysCaps.cpu_vfp_version == 3) 
					s_sysCaps.soc_likely_family = TEGRA_SOC_T4i;
			}
			else // wasn't T4 class detected, must be T2 or T3
			{
				s_sysCaps.gpu_likely_family = TEGRA_GPU_T2T3;
				s_sysCaps.soc_likely_family = TEGRA_SOC_T2;
				if (s_sysCaps.cpu_has_neon)
					s_sysCaps.soc_likely_family = TEGRA_SOC_T3;
			}
		}
		else
		{ // verify a bunch of stuff that a T5 chip would have/not-have.
			if (s_sysCaps.has_shadow_cube
				&& s_sysCaps.has_bindlesstex // so two key NV extensions
				&& (s_sysCaps.has_msaa || s_sysCaps.config_has_msaa) // must have msaa
				&& s_sysCaps.config_has_es3
				&& s_sysCaps.config_has_z24 // should have z24 and !nlz
				&& !s_sysCaps.has_nlz
				&& !s_sysCaps.has_fbfetch // and no fbfetch
				)
			{
				s_sysCaps.soc_may_be_tegra = true; 
				s_sysCaps.gpu_likely_family = TEGRA_GPU_FUTURE;
				s_sysCaps.soc_likely_family = TEGRA_SOC_FUTURE;
			}
			else
			{
				// we could count the number of NV_ extensions, and if say >10, we'll rate it a tegra.
				// if it's a tegra, and has ES3 or GL, then it's GPU2+/T5+
				// but for now... if we get this far and fail, we'll say we're NOT a tegra.
				// and we'll revise the code at some point in a year. :)
			}
		}
	}
	
	return &s_sysCaps;
}
*/

/*

#ifndef EGL_COVERAGE_BUFFERS_NV
#define EGL_COVERAGE_BUFFERS_NV           0x30E0
#define EGL_COVERAGE_SAMPLES_NV           0x30E1
#endif
#ifndef EGL_DEPTH_ENCODING_NV
#define EGL_DEPTH_ENCODING_NV             0x30E2
#define EGL_DEPTH_ENCODING_NONE_NV        0
#define EGL_DEPTH_ENCODING_NONLINEAR_NV   0x30E3
#endif

static bool queryEGLConfigFeatures()
{
	EGLDisplay disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	EGLint count = 0;
	if (!eglGetConfigs(disp, NULL, 0, &count))
	{
		LOGDV("queryEGLConfigFeatures cannot query count of all configs");
		return false;
	}

	LOGDV("Config count = %d", count);

	EGLConfig* configs = new EGLConfig[count];
	if (!eglGetConfigs(disp, configs, count, &count))
	{
		LOGDV("queryEGLConfigFeatures cannot query all configs");
		return false;
	}

	int i;
	for (i = 0; i < count; i++)
	{
		EGLint surfaceType = 0;
		EGLint blueBits = 0;
		EGLint greenBits = 0;
		EGLint redBits = 0;
		EGLint alphaBits = 0;
		EGLint depthBits = 0;
		EGLint stencilBits = 0;
		EGLint renderableFlags = 0;
		EGLint msaaSampleBufs = 0;
		EGLint msaaSamples = 0;
		EGLint depthEnc = 0;
		EGLint covBufs = 0;
		EGLint covSamples = 0;
		EGLBoolean ok = EGL_TRUE;

		ok &= eglGetConfigAttrib(disp, configs[i], EGL_RENDERABLE_TYPE, &renderableFlags);
		if (renderableFlags == EGL_OPENGL_ES_BIT)
		{
			// ignore ES1-only render configs, they are of no use.
			continue;
		}

		ok &= eglGetConfigAttrib(disp, configs[i], EGL_SURFACE_TYPE, &surfaceType);
		
		ok &= eglGetConfigAttrib(disp, configs[i], EGL_RED_SIZE, &redBits);
		ok &= eglGetConfigAttrib(disp, configs[i], EGL_GREEN_SIZE, &greenBits);
		ok &= eglGetConfigAttrib(disp, configs[i], EGL_BLUE_SIZE, &blueBits);
		ok &= eglGetConfigAttrib(disp, configs[i], EGL_ALPHA_SIZE, &alphaBits);
		
		ok &= eglGetConfigAttrib(disp, configs[i], EGL_DEPTH_SIZE, &depthBits);
		ok &= eglGetConfigAttrib(disp, configs[i], EGL_STENCIL_SIZE, &stencilBits);

		ok &= eglGetConfigAttrib(disp, configs[i], EGL_SAMPLE_BUFFERS, &msaaSampleBufs);
		ok &= eglGetConfigAttrib(disp, configs[i], EGL_SAMPLES, &msaaSamples);

		ok &= eglGetConfigAttrib(disp, configs[i], EGL_COVERAGE_BUFFERS_NV, &covBufs);
		ok &= eglGetConfigAttrib(disp, configs[i], EGL_COVERAGE_SAMPLES_NV, &covSamples);

		ok &= eglGetConfigAttrib(disp, configs[i], EGL_DEPTH_ENCODING_NV, &depthEnc);

		if (EGL_FALSE==ok)
		{
			//LOGDV("queryEGLConfigFeatures eglGetConfigAttrib failed!");
			//return false;
		}
		
//		LOGDV("Config[%d]: R%dG%dB%dA%d D%dS%d CSAA%d@%d MSAA%d@%d ZENC%d    Type=%04x Render=%04x",
//			i, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, 
//			covBufs, covSamples, msaaSampleBufs, msaaSamples, depthEnc, surfaceType, renderableFlags);
		
		// set flags to tell us things...
		if (depthEnc==EGL_DEPTH_ENCODING_NONLINEAR_NV)
			s_sysCaps.config_has_nlz = true;
		if (depthBits==24)
			s_sysCaps.config_has_z24 = true;

		
		if (covBufs>0 && covSamples>0)
			s_sysCaps.config_has_csaa = true;
		else
		if (msaaSampleBufs>0 && msaaSamples>0)
			s_sysCaps.config_has_msaa = true;
			
		if (renderableFlags & EGL_OPENGL_ES3_BIT)
			s_sysCaps.config_has_es3 = true;
		if (renderableFlags & EGL_OPENGL_BIT)
			s_sysCaps.config_has_opengl = true;
	}

	LOGV("##> Configs include:");
	if (s_sysCaps.config_has_nlz)
		LOGV("...> NLZ");
	if (s_sysCaps.config_has_z24)
		LOGV("...> Z24");
	if (s_sysCaps.config_has_csaa)
		LOGV("...> CSAA");
	if (s_sysCaps.config_has_msaa)
		LOGV("...> MSAA");
	if (s_sysCaps.config_has_es3)
		LOGV("...> ES3");
	if (s_sysCaps.config_has_opengl)
		LOGV("...> GL");
	
	delete[] configs;

	return true; 
}
*/


/////////////////////////////////////////////////////////////////////
// returns TEGRA_SOC_UNKNOWN if sysfs has 'tegra' but family number can't be ID'd.
// returns TEGRA_SOC_NONE if no sysfs
/////////////////////////////////////////////////////////////////////
Nv_TegraSocFamily nvGetTegraSocFamily() {
	char databuf[MAX_DATA_LEN]; // something more than reasonable.
	int dataLen;
	int cpuFamily = -1; // not a tegra
	
	dataLen = readDataFile("/sys/bus/soc/devices/soc0/family", databuf, MAX_DATA_LEN);
	if (dataLen <= 1)
		return TEGRA_SOC_ERROR;

	databuf[dataLen] = 0;
	if (charIsSeparator(databuf[dataLen-1]))
		databuf[--dataLen] = 0;
	LOGDV("Chip family string: %s", databuf);
	
	if (0!=strncmp(databuf, "Tegra", 5))
		return TEGRA_SOC_NOTNV; //read but not a tegra.
	// else
	cpuFamily = atoi(databuf+5); // jump past "tegra"	
	if (cpuFamily == 2)
		return TEGRA_SOC_T2;
	if (cpuFamily == 3)
		return TEGRA_SOC_T3;
	if (cpuFamily == 14)
		return TEGRA_SOC_T4i;
	if (cpuFamily == 11)
		return TEGRA_SOC_T4;
	// if we get here, we will assume no future chip less than family value 12.
	if (cpuFamily >= 12)
		return TEGRA_SOC_FUTURE;
	//else we don't know this chip value.
	return TEGRA_SOC_UNKNOWN;
}

/*
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
bool nvCheckBuildString(JNIEnv *env, const char *field, const char *value) {
	if (field==NULL || field[0]==0) return false;
	if (value==NULL || value[0]==0) return false;
	
    jclass build = env->FindClass("android/os/Build");
    jfieldID fieldID = env->GetStaticFieldID(build, field, "Ljava/lang/String;");
    jstring sfield = (jstring)(env->GetStaticObjectField(build, fieldID));
    
    bool isMatch = false;
    const char* str = env->GetStringUTFChars(sfield, NULL);
    if (str == NULL) return false;

    if (0==strncmp(value, str, strlen(value)))
		isMatch = true;    	
	env->ReleaseStringUTFChars(sfield, str);
    return isMatch;
}



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
bool isCpuX86(JNIEnv *env) {
    bool isX86 = false;
	if (nvCheckBuildString(env, "CPU_ABI", "x86"))
		isX86 = true;
	else
	if (nvCheckBuildString(env, "CPU_ABI2", "x86"))
		isX86 = true;

    LOGDV("Chip is %s X86", isX86 ? "" : "not");
    return isX86;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
bool isDeviceShield(JNIEnv *env) {
    bool isShield = false;
	if (nvCheckBuildString(env, "MODEL", "SHIELD"))
		isShield = true;
    
	LOGDV("Device is %s SHIELD", isShield ? "" : "not");
    return isShield;
}

/////////////////////////////////////////////////////////////////////
// now just an accessor
/////////////////////////////////////////////////////////////////////
bool nvIsOnShield() {
	return s_sysCaps.dev_is_shield;
}
*/

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#define SHADER_STRING_NUM(a) sizeof((a))/sizeof((a)[0])

const char *t2t3_extension = "GL_NV_shader_framebuffer_fetch";
const char *t2t3_shader[] =
{
	"#version 100\n",
	"#extension GL_NV_shader_framebuffer_fetch : require\n",
	"precision mediump float;\n",
	"void main()\n",
	"{\n",
	"	gl_FragColor = 0.5 * gl_LastFragColor;\n",
	"}\n",
};

const char *t4t4i_extension = "GL_NV_shadow_samplers_cube";
const char *t4t4i_shader[] =
{
	"#version 100\n",
	"#extension GL_NV_shadow_samplers_cube : require\n",
	"precision mediump float;\n",
	"uniform samplerCubeShadowNV u_tex0;\n",
	"varying vec4 texcoord_ref;\n",
	"void main()\n",
	"{\n",
	"	float lit = shadowCubeNV(u_tex0, texcoord_ref);\n",
	"	gl_FragColor = vec4(lit);\n",
	"}\n",
};

const char *t5plus_extension = "GL_NV_bindless_texture";
const char *t5plus_shader[] =
{
	"#version 100\n",
	"#extension GL_NV_bindless_texture : require\n",
	"#extension GL_NV_gpu_shader5 : require\n", 
	"in sampler2D bindless_texture;\n",
	"void main(void)\n",
	"{\n",
	"	gl_FragColor = texture2D(bindless_texture, vec2(0.5,0.5));\n",
	"}\n",	
};


static int check_gl_error()
{
	if (glGetError() != GL_NO_ERROR)
		return -1;

	return 0;
}


static int shader_src_compile(GLenum shader_type, const char **src_string_ptr, int src_string_num)
{
	int result;
	GLuint src_handle;
	GLint compile_status;

	result = -1;

	src_handle = glCreateShader(shader_type);
	if (check_gl_error())
	{
		LOGEV("glCreateShader fail");
		result = 0;
		goto shader_src_compile_exit;
	}

	glShaderSource(src_handle, src_string_num, src_string_ptr, NULL);
	if (check_gl_error())
	{
		LOGEV("glShaderSource fail");
		result = 0;
		goto shader_src_compile_exit;
	}

	/* compile */
	glCompileShader(src_handle);
	if (check_gl_error())
	{
		LOGEV("glCompileShader fail");
		result = 0;
		goto shader_src_compile_exit;
	}

	/* ok? */
	glGetShaderiv(src_handle, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE)
	{
		char msg_buf[1024];
		glGetShaderInfoLog(src_handle, 1024, 0, msg_buf);
		LOGEV("glGetShaderiv fail, compile error %d:\n%s", compile_status, msg_buf);
		result = 0;
		goto shader_src_compile_exit;
	}

	if (check_gl_error())
	{
		LOGEV("glGetShaderiv failed, gl error.");
		result = 0;
		goto shader_src_compile_exit;
	}

shader_src_compile_exit:

	if (src_handle)
		glDeleteShader(src_handle);

	return result;
}


Nv_TegraGpuFamily nvVerifyTegraGpuFamilyStrict()
{
	char databuf[MAX_DATA_LEN]; // something more than reasonable.
	int dataLen;
	const char *exts = (const char*) glGetString(GL_EXTENSIONS);

	Nv_TegraGpuFamily gpuClass = TEGRA_GPU_NOTNV; // not a tegra by default

	// do EVERY compile check, in increasing gpu order, so a failure
	// still may net us a valid result.
	if (strstr(exts, t2t3_extension) &&
		shader_src_compile(GL_FRAGMENT_SHADER, t2t3_shader, SHADER_STRING_NUM(t2t3_shader)))
	{
		gpuClass = TEGRA_GPU_T2T3;
	}
	
	if (strstr(exts, t4t4i_extension) &&
			shader_src_compile(GL_FRAGMENT_SHADER, t4t4i_shader, SHADER_STRING_NUM(t4t4i_shader)))
	{
		gpuClass = TEGRA_GPU_T4T4i;
	}

	if (strstr(exts, t5plus_extension) &&
		shader_src_compile(GL_FRAGMENT_SHADER, t5plus_shader, SHADER_STRING_NUM(t5plus_shader)))
	{
		gpuClass = TEGRA_GPU_FUTURE;
	}
		
	LOGDV("Tegra GPU family = %d", (int)gpuClass);
	return gpuClass;
}
