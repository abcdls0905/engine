//--------------------------------------------------------------------
// 文件名:		walk_generator_trace.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2011年12月9日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "walk_generator.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "../visual/vis_utils.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../public/i_file_sys.h"

static inline bool DirExists(const char* name)
{
	return g_pCore->GetFileSys()->DirExists(name);
}

static inline bool DirCreate(const char* name)
{
	if (DirExists(name))
	{
		return false;
	}

	if(g_pCore->GetFileSys()->DirCreate(name))
	{
		return true;
	}

	bool succeed = true;
	size_t size = strlen(name) + 1;
	TAutoMem<char, 256, TCoreAlloc> auto_buf(size);
	char* buffer = auto_buf.GetBuffer();

	CopyString(buffer, size, name);

	for (size_t i = 0; i < size; ++i)
	{
		if ((buffer[i] == '\\') || (buffer[i] == '/'))
		{
			buffer[i] = 0;

			if (!DirExists(buffer))
			{
				if( g_pCore->GetFileSys()->DirCreate(buffer) )
				{
					succeed = false;
				}
			}

			buffer[i] = '\\';
		}
	}

	return succeed;
}

static inline bool DirDelete(const char* name)
{
	return g_pCore->GetFileSys()->DirDelete(name);
}

result_string get_scene_name(const char* terrain_file_path)
{
	size_t len = strlen(terrain_file_path);
	char scene_name[256];

	SafeSprintf(scene_name, sizeof(scene_name), "%s", terrain_file_path);

	if (scene_name[len - 1] == '\\')
	{
		scene_name[len - 1] = 0;
	}

	char* s = strrchr(scene_name, '\\');

	if (s)
	{
		return ++s;
	}

	return "";
}

bool CWalkGenerator::Trace(float x, float y, float z, int marker, 
	const char* info)
{
	if (!m_pTerrain->GetGenWalkTrace())
	{
		return false;
	}

	if (m_strWalkGeneratorTraceFile.empty())
	{
		char path[256];
		char file_name[256];

		SafeSprintf(path, sizeof(path), "%s%s", 
			m_pTerrain->GetCore()->GetWorkPath(), "WalkGenerator\\");

		if (!DirExists(path))
		{
			DirCreate(path);
		}

		result_string scene_name = get_scene_name(m_pTerrain->GetFilePath());

		if (!scene_name.empty())
		{
			SafeSprintf(path, sizeof(path), "%s%s\\", path, scene_name.c_str());

			if (!DirExists(path))
			{
				DirCreate(path);
			}
		}

		SafeSprintf(file_name, sizeof(file_name), "%s%s%s", path, 
			m_pZone->GetName(), "_walk_info.txt");
		g_pCore->GetFileSys()->FileDelete(file_name);

		m_strWalkGeneratorTraceFile = file_name;

		if (NULL == m_pWalkGeneratorTraceFile)
		{
			m_pWalkGeneratorTraceFile = core_file::fopen(
				m_strWalkGeneratorTraceFile.c_str(), "at");
		}
	}

	if (NULL == m_pWalkGeneratorTraceFile)
	{
		return false;
	}

	char buf[256] = { 0 };
	//static char marker_info[256] = { 0 };

	//if (marker >= 0)
	//{
	//	if (FLOOR_MARKER_EXISTS & marker)
	//	{
	//		SafeSprintf(marker_info, sizeof(marker_info), "%s%s", 
	//			marker_info, "(floor exists)");
	//	}

	//	if (FLOOR_MARKER_MOVE & marker)
	//	{
	//		SafeSprintf(marker_info, sizeof(marker_info), "%s%s", 
	//			marker_info, "(can move)");
	//	}

	//	if (FLOOR_MARKER_STAND & marker)
	//	{
	//		SafeSprintf(marker_info, sizeof(marker_info), "%s%s", 
	//			marker_info, "(can stand)");
	//	}

	//	if (FLOOR_MARKER_WALL & marker)
	//	{
	//		SafeSprintf(marker_info, sizeof(marker_info), "%s%s", 
	//			marker_info, "(is wall)");
	//	}

	//	int all_marker = FLOOR_MARKER_WALL | FLOOR_MARKER_MOVE 
	//		| FLOOR_MARKER_STAND | FLOOR_MARKER_WALL;

	//	if (0 == (all_marker & marker))
	//	{
	//		SafeSprintf(marker_info, sizeof(marker_info), "%s", "(no floor)");
	//	}

	//	SafeSprintf(buf, sizeof(buf), "%.2f %.2f y(%.2f) marker(%d), "
	//		"marker info(%s), info(%s)\n", x, z, y, marker, marker_info, info);
	//}
	//else
	//{
	//	SafeSprintf(buf, sizeof(buf), "%.2f %.2f y(%.2f) info(%s)\n", x, z, 
	//		y, info);
	//}

	SafeSprintf(buf, sizeof(buf), "%.2f %.2f y(%.2f) (%s)\n", x, z, y, info);

	core_file::fwrite(buf, strlen(buf), 1, m_pWalkGeneratorTraceFile);

	return true;
}