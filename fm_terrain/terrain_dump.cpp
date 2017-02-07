//--------------------------------------------------------------------
// 文件名:		terrain_dump.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_chunk.h"
#include "terrain_light.h"
#include "terrain_editor.h"
#include "terrain_brush.h"
#include "terrain_material.h"
#include "terrain_water.h"
#include "terrain_grass.h"
#include "terrain_culling.h"
#include "terrain_painter.h"
#include "terrain_visuals.h"
#include "terrain_region.h"
#include "blend_tex.h"
#include "blend_tex_set.h"
#include "chunk_style.h"
#include "sea_water.h"
#include "sea_base.h"
#include "grass_type.h"
#include "zone_manager.h"
#include "visual_manager.h"
#include "quad_collide.h"
#include "quad_visual.h"
#include "quad_tree.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/core_file.h"
#include "../public/core_mem.h"
#include "../public/auto_mem.h"
#include "../visual/i_dx_context.h"
#include "../visual/vis_utils.h"
//#include "../visual/tga_file.h"

// Terrain

#ifdef TERRAIN_EDIT
bool Terrain::SetSaveChunkWalkPos(float x, float z, bool value)
{
	CTerrainChunk* pTerrainChunk = this->GetInChunk(x, z);

	if (pTerrainChunk)
	{
		pTerrainChunk->SetSaveChunkWalkPos(value);
	}

	return true;
}

bool Terrain::GetSaveChunkWalkPos(float x, float z)
{
	CTerrainChunk* pTerrainChunk = this->GetInChunk(x, z);

	if (pTerrainChunk)
	{
		return pTerrainChunk->GetSaveChunkWalkPos();
	}

	return false;
}

bool Terrain::SaveChunkWalkPos(float x, float z, const char* file_name)
{
	CTerrainChunk* pTerrainChunk = this->GetInChunk(x, z);

	if (pTerrainChunk)
	{
		return pTerrainChunk->SaveChunkWalkPos(file_name);
	}

	return false;
}

// 高度值是否无效
inline bool height_is_null(float height)
{
	return (height > 1e7);
}

struct point_height_t
{
	unsigned int index;					// 点序号
	float height;						// 点高度
};

const int BUFFER_COUNT = 1024 * 1024;	// 一次写缓冲大小

struct data_t
{
	std::string strFileName;	// 可行走信息文件名
	size_t nRow;				// 可行走信息文件名中的行号
	size_t nCol;				// 可行走信息文件名中的列号
	CTerrainZone* zone;			// 记录新创建的区域，需要删除
	bool changed_level;			// 是否已经修改了Level
};

// 当前场景下可行走文件信息的相关数据
static std::vector<data_t> s_Data;
// 此处的行和列不能改size_t类型，因为需要差运算
static int s_start_zone_row = 0;
static int s_start_zone_col = 0;
static int s_end_zone_row = 0;
static int s_end_zone_col = 0;
static float s_start_x = 0.0F;
static float s_start_z = 0.0F;
static float s_end_x = 0.0F;
static float s_end_z = 0.0F;

// 分割文件路径和文件名
static void ext_split_file_path(const IVarList& args, IVarList& result)
{
	const char* path = args.StringVal(0);
	char buf[256];
	char* file_part;

	if (0 == Port_GetFullPathName(path, sizeof(buf), buf, &file_part))
	{
		return;
	}

	result_string file_name;

	if (file_part != NULL)
	{
		file_name = file_part;
		*file_part = 0;
	}

	result.AddString(buf);
	result.AddString(file_name.c_str());
}

// 分割文件名和扩展名
static void ext_split_file_name(const IVarList& args, IVarList& result)
{
	const char* str = args.StringVal(0);
	const size_t SIZE1 = strlen(str) + 1;
	TAutoMem<char, 256, TCoreAlloc> auto_buf(SIZE1);
	char* buf = auto_buf.GetBuffer();

	memcpy(buf, str, SIZE1);

	char* name = buf;
	char* ext = "";
	char* p = buf + SIZE1 - 1;

	while (--p >= buf)
	{
		if (*p == '.')
		{
			*p = 0;
			ext = p + 1;
			break;
		}
		else if (*p == '\\')
		{
			break;
		}
	}

	result.AddString(name);
	result.AddString(ext);
}

// 查找文件
static bool SearchFile(const char* path, const char* file_name, 
	std::vector<data_t>& vResultFiles)
{
	Assert(path != NULL);
	Assert(file_name != NULL);

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", path, file_name);
	
	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile(fname, &data);

	if (INVALID_HANDLE_VALUE == handle)
	{
		return false;
	}

	vResultFiles.clear();

	do 
	{
		if (0 == (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			vResultFiles.push_back(data_t());
			vResultFiles.back().strFileName = data.cFileName;
		}
	} while (FindNextFile(handle, &data));

	FindClose(handle);

	return true;
}

// 从可行走信息文件名中获得所在的行和列
static bool GetRowColFromWalkFileName(const char* file_name, size_t& row, 
	size_t& col)
{
	Assert(file_name != NULL);

	char buf[256];

	CopyString(buf, sizeof(buf), file_name);

	char* s1 = strchr(buf, '_');

	if (NULL == s1)
	{
		return false;
	}

	char* s2 = strchr(s1 + 1, '_');

	if (NULL == s2)
	{
		return false;
	}

	char* s3 = strchr(s2 + 1, '.');

	if (NULL == s3)
	{
		return false;
	}

	*s1 = 0;
	*s2 = 0;
	*s3 = 0;
	row = atoi(s1 + 1);
	col = atoi(s2 + 1);

	return true;
}

// 是否存在指定ZONE的可行走信息文件
static int ExistZone(std::vector<data_t>& vData, const size_t row, 
	const size_t col)
{
	size_t data_count = vData.size();

	for (size_t i = 0; i < data_count; ++i)
	{
		if (row == vData[i].nRow && col == vData[i].nCol)
		{
			return i;
		}
	}

	return -1;
}

// 保存缓存数据，当缓存满时，写文件，index将被改变
static void SetData(unsigned char* pData, unsigned char data, size_t& index, 
	FILE* pFile)
{
	if (index < BUFFER_COUNT)
	{
		pData[index++] = data;
	}
	else
	{
		core_file::fwrite(pData, sizeof(unsigned char), BUFFER_COUNT, pFile);
		index = 0;
		pData[index++] = data;
	}
}

// 导出可行走文件相关信息，用于3D寻路编辑器
static bool DumpAllWalkInfoToIni(const char* full_file_name)
{
	CVarList args;
	CVarList result;

	args.AddString(full_file_name);

	// 分割文件路径和文件名
	ext_split_file_path(args, result);

	result_string file_path = result.StringVal(0);
	result_string file_name_and_ext = result.StringVal(1);

	args.Clear();
	result.Clear();

	// 分割文件名和扩展名
	args.AddString(file_name_and_ext.c_str());
	ext_split_file_name(args, result);

	result_string file_name_no_ext = result.StringVal(0);
	result_string file_ext = result.StringVal(1);
	result_string ini_file_name = file_path + file_name_no_ext + ".ini";

	FILE* pFile = core_file::fopen(ini_file_name.c_str(), "wt");

	if (NULL == pFile)
	{
		return false;
	}

	char buffer[256] = { 0 };

	core_file::fwrite("[walk_data]\n", strlen("[walk_data]\n"), 1, pFile);

	SafeSprintf(buffer, sizeof(buffer), "TgaFile=%s.tga\n", 
		file_name_no_ext.c_str());
	core_file::fwrite(buffer, strlen(buffer), 1, pFile);

	SafeSprintf(buffer, sizeof(buffer), "PathFile=%s.path\n", 
		file_name_no_ext.c_str());
	core_file::fwrite(buffer, strlen(buffer), 1, pFile);

	SafeSprintf(buffer, sizeof(buffer), "left=%f\n", s_start_x);
	core_file::fwrite(buffer, strlen(buffer), 1, pFile);

	SafeSprintf(buffer, sizeof(buffer), "right=%f\n", s_end_x);
	core_file::fwrite(buffer, strlen(buffer), 1, pFile);

	SafeSprintf(buffer, sizeof(buffer), "top=%f\n", s_start_z);
	core_file::fwrite(buffer, strlen(buffer), 1, pFile);

	SafeSprintf(buffer, sizeof(buffer), "bottom=%f\n", s_end_z);
	core_file::fwrite(buffer, strlen(buffer), 1, pFile);

	core_file::fclose(pFile);

	return true;
}

bool Terrain::InnerBeginDumpWalkInfo()
{
	s_Data.clear();

	s_start_zone_row = 0;
	s_start_zone_col = 0;
	s_end_zone_row = 0;
	s_end_zone_col = 0;
	s_start_x = 0.0F;
	s_start_z = 0.0F;
	s_end_x = 0.0F;
	s_end_z = 0.0F;

	// 获得可行走信息文件（*.walk）路径
	char file_path[256];

	const char* walk_path = GetWalkablePath();

	if (StringEmpty(walk_path))
	{
		SafeSprintf(file_path, sizeof(file_path), GetFilePath());
	}
	else if ('\\' == walk_path[strlen(walk_path) - 1])
	{
		SafeSprintf(file_path, sizeof(file_path), "%s%s", GetFilePath(), 
			walk_path);
	}
	else
	{
		SafeSprintf(file_path, sizeof(file_path), "%s%s\\", GetFilePath(), 
			walk_path);
	}

	// 查找可行走信息文件路径下所有（*.walk）文件
	SearchFile(file_path, "*.walk", s_Data);

	size_t file_count = s_Data.size();

	if (0 == file_count)
	{
		return false;
	}

	// 获得可行走信息文件表示的行和列
	for (size_t i = 0; i < file_count; ++i)
	{
		GetRowColFromWalkFileName(s_Data[i].strFileName.c_str(), 
			s_Data[i].nRow, s_Data[i].nCol);
	}

	for (size_t i = 0; i < file_count; ++i)
	{
		s_Data[i].zone = NULL;
		s_Data[i].changed_level = false;
	}

	// 我们的地图x从东向西增大，z从北向南增大
	FmVec2 vStart(9999.0F, 9999.0F);
	FmVec2 vEnd(-9999.0F, -9999.0F);
	FmVec2 vTemp;

	for (size_t j = 0; j < file_count; ++j)
	{
		vTemp = FmVec2(float(s_Data[j].nRow), float(s_Data[j].nCol));

		D3DXVec2Minimize(&vStart, &vStart, &vTemp);
		D3DXVec2Maximize(&vEnd, &vEnd, &vTemp);
	}

	// 开始和结束zone的行和列
	s_start_zone_row = int(vStart.x);
	s_start_zone_col = int(vStart.y);
	s_end_zone_row = int(vEnd.x);
	s_end_zone_col = int(vEnd.y);

	// 一个zone的大小
	float zone_size = m_nZoneScale * m_fUnitSize;

	int zone_ori_col = GetZoneOriginCol();
	int zone_ori_row = GetZoneOriginRow();

	// 开始和结束点坐标
	s_start_x = zone_size * (s_start_zone_col - zone_ori_col - 0.5f); 
	s_start_z = zone_size * (s_start_zone_row - zone_ori_row - 0.5f);
	s_end_x = zone_size * (s_end_zone_col - zone_ori_col + 0.5f); 
	s_end_z = zone_size * (s_end_zone_row - zone_ori_row + 0.5f);

	return true;
}

bool Terrain::InnerDumpAllWalkPosToFile(const char* file_name)
{
	// 缓存数据和当前缓存位置的索引
	int index = 0;
	point_height_t* pData = (point_height_t*)CORE_ALLOC(
		sizeof(point_height_t) * BUFFER_COUNT);

	if (NULL == pData)
	{
		return false;
	}

	FILE* pFile = core_file::fopen(file_name, "wb");

	if (NULL == pFile)
	{
		return false;
	}

	// 获得碰撞分辨率
	size_t unit_size = GetCollidePerUnit();

	// 所要导出点的实际长和宽
	size_t width = size_t(s_end_x - s_start_x) * unit_size;
	size_t height = size_t(s_end_z - s_start_z) * unit_size;

	float step = 1.0F / unit_size;
	float fbias = 1.0F / unit_size / 2;	// 偏置，用于取中心位置

	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = width; j > 0; --j)
		{
			float x = s_start_x + j * step - fbias;
			float z = s_start_z + i * step + fbias;

			// 获得指定坐标所在的区域位置
			int zone_row;
			int zone_col;

			m_pZoneManager->GetZonePos(x, z, zone_row, zone_col);

			// 是否存在指定ZONE的可行走信息文件
			int idx = ExistZone(s_Data, zone_row, zone_col);

			if (-1 == idx)
			{
				continue;
			}

			CTerrainZone* zone = m_pZoneManager->GetInZone(x, z);

			// 加载还未加载的区域
			if (NULL == zone)
			{
				zone = m_pZoneManager->CreateZone(zone_row, zone_col);

				s_Data[idx].zone = zone;

				// 加载高度信息
				if (!zone->GetHeightLoaded())
				{
					if (NULL == zone->GetHeightLoader())
					{
						if (!zone->BeginCreateHeight())
						{
							continue;
						}
						else
						{
							zone->GetHeightLoader()->Load(false);
							zone->GetHeightLoader()->Create(false);
						}
					}
					else
					{
						while (!zone->GetHeightLoaded())
						{
							Sleep(0);
						}
					}
				}

				// 加载可行走信息
				if (!zone->GetWalkLoaded())
				{
					if (NULL == zone->GetWalkLoader())
					{
						if (!zone->BeginCreateWalk())
						{
							continue;
						}
						else
						{
							zone->GetWalkLoader()->Load(false);
							zone->GetWalkLoader()->Create(false);
						}
					}
					else
					{
						while (!zone->GetWalkLoaded())
						{
							Sleep(0);
						}
					}
				}
			}

			if (!s_Data[idx].changed_level)
			{
				const size_t zone_chunk_num = zone->GetChunkCount();
				CTerrainChunk* zone_chunks = zone->GetChunks();

				for (size_t i = 0; i < zone_chunk_num; i++)
				{
					CTerrainChunk* chunk = zone_chunks + i;

					// 分块必须为0级
					chunk->SetLevel(0);
				}

				// 检查LOD级别，保证相邻的两块LOD级别相差不超过1
				bool change = true;

				while (change)
				{
					// 循环检查直到不再需要改变
					change = false;

					for (size_t k = 0; k < zone_chunk_num; ++k)
					{
						CTerrainChunk* chunk = zone_chunks + k;

						if (chunk->ChangeLevel())
						{
							change = true;
						}
					}
				}

				// 决定分块使用什么风格
				for (size_t j = 0; j < zone_chunk_num; ++j)
				{
					CTerrainChunk* chunk = zone_chunks + j;

					chunk->CalcStyle();
				}

				for (size_t n = 0; n < zone_chunk_num; ++n)
				{
					CTerrainChunk* chunk = zone_chunks + n;

					chunk->CheckVertexBuffer();
				}

				s_Data[idx].changed_level = true;
			}

			while (!zone->GetWalkLoaded())
			{
				Sleep(0);
			}

			// 层范围
			int floor_num = GetFloorCount(x, z);

			for (int floor = 0; floor < floor_num; ++floor)
			{
				bool floor_exist = GetFloorExists(x, z, floor);

				if (floor_exist)
				{
					if (GetFloorCanMove(x, z, floor))
					{
						float height = GetFloorHeight(x, z, floor);

						if (height_is_null(height))
						{
							height = GetPosiY(x, z);
						}

						if (index < BUFFER_COUNT)
						{
							pData[index].index = i * width + (width - j);
							pData[index].height = height;
							index++;
						}
						else
						{
							core_file::fwrite(pData, sizeof(point_height_t), 
								BUFFER_COUNT, pFile);
							index = 0;

							pData[index].index = i * width + (width - j);
							pData[index].height = height;
							index++;
						}
					}
				}
			}
		}
	}

	if (index > 0)
	{
		core_file::fwrite(pData, sizeof(point_height_t), index, pFile);
	}

	core_file::fclose(pFile);

	CORE_FREE(pData, sizeof(point_height_t) * BUFFER_COUNT);

	size_t file_count = s_Data.size();

	for (size_t i = 0; i < file_count; ++i)
	{
		if (s_Data[i].zone)
		{
			// 卸载
			// 地形循环里会自动卸载刚才创建的新的zone，此处不做处理
		}
	}

	return true;
}

bool Terrain::DumpAllWalkPosToFile(const char* file_name)
{
	Assert(NULL != file_name);

	if (!InnerBeginDumpWalkInfo())
	{
		return false;
	}

	return InnerDumpAllWalkPosToFile(file_name);
}

bool Terrain::DumpAllWalkPosToImage(const char* file_name)
{
	Assert(NULL != file_name);

	if (!InnerBeginDumpWalkInfo())
	{
		return false;
	}

	if (!DumpAllWalkInfoToIni(file_name))
	{
		return false;
	}

	// 一个zone的大小
	float zone_size = m_nZoneScale * m_fUnitSize;
	float width = zone_size * (s_end_zone_col - s_start_zone_col + 1);
	float height = zone_size * (s_end_zone_row - s_start_zone_row + 1);
	float precision = (float)GetCollidePerUnit();
	float fbias = float(1 / precision / 2);

	// 缓存数据和当前缓存位置的索引
	size_t index = 0;
	unsigned char* pData = (unsigned char*)CORE_ALLOC(BUFFER_COUNT);

	if (NULL == pData)
	{
		return false;
	}

	FILE* pFile = core_file::fopen(file_name, "wb");

	if (NULL == pFile)
	{
		return false;
	}

	// TGA头数据
	tga_header_t head;

	memset(&head, 0, sizeof(head));
	head.type = 3;
	head.width = int(width * precision);
	head.height = int(height * precision);
	head.bpp = 8;
	head.attr8 = 8;

	core_file::fwrite(&head, sizeof(head), 1, pFile);

	const unsigned char WALK_DISABLE = 0x00;
	const unsigned char PLAYER_WALK = 0x10;
	const unsigned char NPC_PLAYER_WALK = 0x90;

	// 两个可行走点间的高度差限制
	float height_delta = 1.0F;

	for (int i = int(height * precision); i > 0; i--)
	{
		for (int j = int(width * precision); j > 0; j--)
		{
			float x = s_start_x + j / precision;
			float z = s_start_z + i / precision;

			// 获得指定坐标所在的区域位置
			int zone_row;
			int zone_col;

			m_pZoneManager->GetZonePos(x, z, zone_row, zone_col);

			// 是否存在指定ZONE的可行走信息文件
			int idx = ExistZone(s_Data, zone_row, zone_col);

			if (-1 == idx)
			{
				// 不存在即不可行走
				SetData(pData, WALK_DISABLE, index, pFile);
				continue;
			}

			CTerrainZone* zone = m_pZoneManager->GetInZone(x, z);

			// 加载还未加载的区域
			if (NULL == zone)
			{
				zone = m_pZoneManager->CreateZone(zone_row, zone_col);

				s_Data[idx].zone = zone;

				// 加载高度信息
				if (!zone->GetHeightLoaded())
				{
					if (NULL == zone->GetHeightLoader())
					{
						if (!zone->BeginCreateHeight())
						{
							continue;
						}
						else
						{
							zone->GetHeightLoader()->Load(false);
							zone->GetHeightLoader()->Create(false);
						}
					}
					else
					{
						while (!zone->GetHeightLoaded())
						{
							Sleep(0);
						}
					}
				}

				// 加载可行走信息
				if (!zone->GetWalkLoaded())
				{
					if (NULL == zone->GetWalkLoader())
					{
						if (!zone->BeginCreateWalk())
						{
							continue;
						}
						else
						{
							zone->GetWalkLoader()->Load(false);
							zone->GetWalkLoader()->Create(false);
						}
					}
					else
					{
						while (!zone->GetWalkLoaded())
						{
							Sleep(0);
						}
					}
				}
			}

			if (!s_Data[idx].changed_level)
			{
				const size_t zone_chunk_num = zone->GetChunkCount();
				CTerrainChunk* zone_chunks = zone->GetChunks();

				for (size_t i = 0; i < zone_chunk_num; i++)
				{
					CTerrainChunk* chunk = zone_chunks + i;

					// 分块必须为0级
					chunk->SetLevel(0);
				}

				// 检查LOD级别，保证相邻的两块LOD级别相差不超过1
				bool change = true;

				while (change)
				{
					// 循环检查直到不再需要改变
					change = false;

					for (size_t k = 0; k < zone_chunk_num; ++k)
					{
						CTerrainChunk* chunk = zone_chunks + k;

						if (chunk->ChangeLevel())
						{
							change = true;
						}
					}
				}

				// 决定分块使用什么风格
				for (size_t j = 0; j < zone_chunk_num; ++j)
				{
					CTerrainChunk* chunk = zone_chunks + j;

					chunk->CalcStyle();
				}

				for (size_t n = 0; n < zone_chunk_num; ++n)
				{
					CTerrainChunk* chunk = zone_chunks + n;

					chunk->CheckVertexBuffer();
				}

				s_Data[idx].changed_level = true;
			}

			while (!zone->GetWalkLoaded())
			{
				Sleep(0);
			}

			if (GetCollideEnable(x - fbias, z - fbias))
			{
				if (GetWalkEnable(x - fbias, z - fbias))
				{
					// 有高度差限制，检查周围点
					bool can_walk = true;
					float cur_posi_y = 0.0F;
					float nb_posi_y = 0.0F;
					float nb_bias = 1 / precision;

					FmVec2 nb_pos[8];

					nb_pos[0] = FmVec2(x + nb_bias, z);
					nb_pos[1] = FmVec2(x + nb_bias, z - nb_bias);
					nb_pos[2] = FmVec2(x, z - nb_bias);
					nb_pos[3] = FmVec2(x - nb_bias, z - nb_bias);
					nb_pos[4] = FmVec2(x - nb_bias, z);
					nb_pos[5] = FmVec2(x - nb_bias, z + nb_bias);
					nb_pos[6] = FmVec2(x, z + nb_bias);
					nb_pos[7] = FmVec2(x + nb_bias, z + nb_bias);

					cur_posi_y = GetWalkHeight(x - fbias, z - fbias);

					for (int i = 0; i < 8; ++i)
					{
						if (GetCollideEnable(x - fbias, z - fbias))
						{
							if (GetWalkEnable(nb_pos[i].x - fbias, 
								nb_pos[i].y - fbias))
							{
								nb_posi_y = GetWalkHeight(nb_pos[i].x - fbias, 
									nb_pos[i].y - fbias);

								float height_dis = nb_posi_y - cur_posi_y;

								if (height_dis < 0.0F)
								{
									height_dis = -height_dis;
								}

								if (height_dis >= height_delta)
								{
									can_walk = false;
									break;
								}
							}
						}
					}

					if (can_walk)
					{
						SetData(pData, NPC_PLAYER_WALK, index, pFile);
					}
					else
					{
						SetData(pData, WALK_DISABLE, index, pFile);
					}
				}
				else
				{
					SetData(pData, WALK_DISABLE, index, pFile);
				}
			}
			else
			{
				SetData(pData, WALK_DISABLE, index, pFile);
			}
		}
	}

	if (index > 0)
	{
		core_file::fwrite(pData, sizeof(unsigned char), index, pFile);
	}

	core_file::fclose(pFile);

	CORE_FREE(pData, BUFFER_COUNT);

	size_t file_count = s_Data.size();

	for (size_t i = 0; i < file_count; ++i)
	{
		if (s_Data[i].zone)
		{
			// 卸载
			// 地形循环里会自动卸载刚才创建的新的zone，此处不做处理
		}
	}

	return true;
}
#endif // TERRAIN_EDIT
*/