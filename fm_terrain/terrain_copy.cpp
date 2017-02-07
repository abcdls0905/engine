//--------------------------------------------------------------------
// 文件名:		terrain_copy.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年1月10日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#include "terrain_copy.h"
#include "terrain.h"
#include "terrain_editor.h"
#include "terrain_chunk.h"
#include "terrain_zone.h"
#include "../visual/vis_utils.h"
#include "../public/var.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../public/core_mem.h"
#include "../public/auto_mem.h"

/// \file terrain_copy.cpp
/// \brief 地形拷贝

/// entity: TerrainCopy
/// \brief 地形拷贝实体
DECLARE_ENTITY("", TerrainCopy, IEntity);

/// method: bool SetTerrain(PERSISTID id)
/// \brief 地形
/// \param id 地形对象ID
/// \return true表示成功，false表示失败
DECLARE_METHOD_1(bool, TerrainCopy, SetTerrain, PERSISTID);

/// method: bool SetRect(float x1, float z1, float x2, float z2)
/// \brief 设置编辑范围
/// \param x1,z1,x2,z2 起始点和终点坐标
/// \return true表示成功，false表示失败
DECLARE_METHOD_4(bool, TerrainCopy, SetRect, float, float, float, float);

/// method: bool SetCenter(float center_x, float center_z)
/// \brief 设置整体的编辑中心（未分组的为Rect中心，分组的为整体范围中心）
/// \param center_x,center_z 中心点坐标
/// \return true表示成功，false表示失败
DECLARE_METHOD_2(bool, TerrainCopy, SetCenter, float, float);

/// method: bool SetCurrentFile(const char* file_name)
/// \brief 设置当前文件名
/// \param file_name 当前文件名
/// \return true表示成功，false表示失败
DECLARE_METHOD_1(bool, TerrainCopy, SetCurrentFile, const char*);

/// method: bool SetAngle(float value)
/// \brief 设置旋转角度
/// \param value 旋转角度
/// \return true表示成功，false表示失败
DECLARE_METHOD_1(bool, TerrainCopy, SetAngle, float);

/// property: bool CopyBlend
/// \brief 是否保存混合贴图数据
DECLARE_PROPERTY(bool, TerrainCopy, CopyBlend, GetCopyBlend, SetCopyBlend);
/// property: bool ClearBlend
/// \brief 是否清除指定范围内原混合贴图数据
DECLARE_PROPERTY(bool, TerrainCopy, ClearBlend, GetClearBlend, SetClearBlend);

/// property: bool CopyGrass
/// \brief 是否保存花草数据
DECLARE_PROPERTY(bool, TerrainCopy, CopyGrass, GetCopyGrass, SetCopyGrass);
/// property: bool ClearGrass
/// \brief 是否清除指定范围内原花草数据
DECLARE_PROPERTY(bool, TerrainCopy, ClearGrass, GetClearGrass, SetClearGrass);

/// method: bool BeginSave(const char* file_name)
/// \brief 保存前的处理
/// \param file_name 当前文件名
/// \return true表示成功，false表示失败
DECLARE_METHOD_1(bool, TerrainCopy, BeginSave, const char*);
/// method: bool Save()
/// \brief 保存所有需要拷贝的数据
/// \return -1表示保存失败，0表示没有需要保存的数据，1表示保存成功
DECLARE_METHOD_0(int, TerrainCopy, Save);

/// method: bool GetBlendDataCount()
/// \brief 当前混合贴图数据数量
DECLARE_METHOD_0(int, TerrainCopy, GetBlendDataCount);

/// method: bool GetGrassDataCount()
/// \brief 当前花草数据数量
DECLARE_METHOD_0(int, TerrainCopy, GetGrassDataCount);

/// method: bool ModifyTerrain()
/// \brief 使用存储数据修改指定范围的地形
/// \return true表示成功，false表示失败
DECLARE_METHOD_0(bool, TerrainCopy, ModifyTerrain);

// TerrainCopy

TerrainCopy::TerrainCopy()
{
	m_pTerrain = NULL;
	m_pEditor = NULL;
	m_strFileName = "";
	m_vStartPos.x = -1.0F;
	m_vStartPos.z = -1.0F;
	m_vEndPos.x = 1.0F;
	m_vEndPos.z = 1.0F;
	m_vCenterPos.x = 0.0F;
	m_vCenterPos.z = 0.0F;
	m_fAngle = 0.0F;

	m_bCopyBlend = false;
	m_bClearBlend = false;

	m_bCopyGrass = true;
	m_bClearGrass = true;
}

TerrainCopy::~TerrainCopy()
{
	ClearAllBlendData();
	ClearAllGrassData();
}

bool TerrainCopy::Init(const IVarList& args)
{
	return true;
}

bool TerrainCopy::Shut()
{
	return true;
}

// 输入一个相对于原点(0, 0)的坐标
// 返回原点到该点的连线与X轴正向的夹角[0, 2PI)
static float cal_angle(float x, float y)
{
	if (FloatEqual(0.0F, x))
	{
		if (y < 0.0F)
		{
			// 处理点在Y轴负半轴
			return FLOAT_PI / 2;
		}
		else if (y > 0.0F)
		{
			// 处理点在Y轴正半轴
			return 3 * FLOAT_PI / 2;
		}
		else
		{
			// 处理原点
			return 0.0F;
		}
	}

	// 计算在(-PI/2, PI/2)区间的值
	float ret = atan(y / x);

	if (x < 0.0F)
	{
		// 处理第二和第三象限
		ret = ret + FLOAT_PI;
	}
	else if (x > 0.0F && y < 0.0F)
	{
		// 处理第四象限
		ret = ret + 2 * FLOAT_PI;
	}

	return ret;
}

// 获得(x, z)绕指定点(center_x, center_z)旋转(rotate(角度))后的坐标
static bool get_new_pos(Terrain* pTerrain, float x, float z, float center_x, 
	float center_z, float rotate, IVarList& result)
{
	float r_x = x;
	float r_y = 0.0F;
	float r_z = z;
	
	if ((!FloatEqual(0.0F, rotate)) && (!FloatEqual(360.0F, rotate)))
	{
		float radian = D3DXToRadian(-rotate);

		float relative_x = x - center_x;
		float relative_z = z - center_z;

		r_x = center_x + relative_x * cos(radian) - relative_z * sin(radian);
		r_z = center_z + relative_z * cos(radian) + relative_x * sin(radian);
	}

	if (pTerrain)
	{
		r_y = pTerrain->GetPosiY(r_x, r_z);
	}
	
	result.AddFloat(r_x);
	result.AddFloat(r_y);
	result.AddFloat(r_z);

	return true;
}

static int run_callback(IEntity* pEntity, const char* event, 
	const IVarList& args)
{
	CVarList res;

	if (!pEntity->GetCore()->ExecCallback(pEntity, event, args, &res))
	{
		return 0;
	}

	return res.IntVal(0);
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

static IEntity* get_entity(TerrainCopy* pThis, PERSISTID id)
{
	return pThis->GetCore()->GetEntity(id);
}

bool TerrainCopy::SetTerrain(PERSISTID id)
{ 
	m_pTerrain = (Terrain*)get_entity(this, id);
	m_pEditor = (TerrainEditor*)get_entity(this, m_pTerrain->GetEditorID());

	return true;
}

bool TerrainCopy::SetRect(float start_x, float start_z, float end_x, 
	float end_z)
{
	m_vStartPos.x = floor(start_x);
	m_vStartPos.z = floor(start_z);

	m_vEndPos.x = floor(end_x);
	m_vEndPos.z = floor(end_z);

	//m_vCenterPos.x = (m_vStartPos.x + m_vEndPos.x) / 2.0F;
	//m_vCenterPos.z = (m_vStartPos.z + m_vEndPos.z) / 2.0F;

	return true;
}

bool TerrainCopy::SetCenter(float center_x, float center_z)
{
	m_vCenterPos.x = center_x;
	m_vCenterPos.z = center_z;

	return true;
}

bool TerrainCopy::SetCurrentFile(const char* file_name)
{
	Assert(NULL != file_name);

	if (0 == file_name[0])
	{
		return false;
	}

	ClearAllBlendData();
	ClearAllGrassData();

	CVarList args;
	CVarList result;

	args.AddString(file_name);

	// 分割文件名和扩展名
	ext_split_file_name(args, result);

	const char* name = result.StringVal(0);
	const char* dot = result.StringVal(1);

	m_strFileName = name;

	//std::map<std::string, std::vector<blend_t*>>::iterator iter = 
	//	m_BlendAll.find(name);

	//if (iter != m_BlendAll.end())
	if (m_BlendAll.GetData(name, m_CurBlend)
		&& m_GrassAll.GetData(name, m_CurGrass))
	{
		//m_CurBlend = &(iter->second);

		//std::map<std::string, std::vector<grass_t*>>::iterator iter_grass = 
		//	m_GrassAll.find(name);

		//if (iter_grass != m_GrassAll.end())
		//{
		//	m_CurGrass = &(iter_grass->second);
		//}

		return true;
	}
	else
	{
		if (InsertNewNode(name))
		{
			LoadBlendData((m_strFileName + ".blend").c_str());
			LoadGrassData((m_strFileName + ".grass").c_str());
			return true;
		}
	}

	return false;
}

bool TerrainCopy::SetAngle(float value)
{
	m_fAngle = value;

	return true;
}

void TerrainCopy::SetCopyBlend(bool value)
{
	m_bCopyBlend = value;
}

bool TerrainCopy::GetCopyBlend() const
{
	return m_bCopyBlend;
}

void TerrainCopy::SetClearBlend(bool value)
{
	m_bClearBlend = value;
}

bool TerrainCopy::GetClearBlend() const
{
	return m_bClearBlend;
}

void TerrainCopy::SetCopyGrass(bool value)
{
	m_bCopyGrass = value;
}

bool TerrainCopy::GetCopyGrass() const
{
	return m_bCopyGrass;
}

void TerrainCopy::SetClearGrass(bool value)
{
	m_bClearGrass = value;
}

bool TerrainCopy::GetClearGrass() const
{
	return m_bClearGrass;
}

bool TerrainCopy::InsertNewNode(const char* name)
{
	if (!InsertNewBlendNode(name))
	{
		return false;
	}

	if (!InsertNewGrassNode(name))
	{
		return false;
	}

	return true;
}

bool TerrainCopy::InsertNewBlendNode(const char* name)
{
	//std::map<std::string, std::vector<blend_t*>>::iterator iter = 
	//	m_BlendAll.find(name);

	//if (iter != m_BlendAll.end())
	if (m_BlendAll.Exists(name))
	{
		// 存在则添加失败
		return false;
	}

	//std::vector<blend_t*> vData;
	//std::pair<std::map<std::string, std::vector<blend_t*>>::iterator, bool> 
	//	pid = m_BlendAll.insert(make_pair(name, vData));
	BLEND_CONTAINER* pBlend = CORE_NEW(BLEND_CONTAINER);

	m_BlendAll.Add(name, pBlend);
	//if (!pid.second)
	//{
	//	// 添加失败
	//	return false;
	//}

	//iter = m_BlendAll.find(name);

	//if (iter == m_BlendAll.end())
	//{
	//	// 已经添加成功，应该不会获取失败
	//	return false;
	//}

	//m_CurBlend = &(iter->second);
	m_CurBlend = pBlend;

	return true;
}

bool TerrainCopy::InsertNewGrassNode(const char* name)
{
	//std::map<std::string, std::vector<grass_t*>>::iterator iter = 
	//	m_GrassAll.find(name);

	//if (iter != m_GrassAll.end())
	if (m_GrassAll.Exists(name))
	{
		// 存在则添加失败
		return false;
	}

	//std::vector<grass_t*> vData;
	//std::pair<std::map<std::string, std::vector<grass_t*>>::iterator, bool> 
	//	pid = m_GrassAll.insert(make_pair(name, vData));

	//if (!pid.second)
	//{
	//	// 添加失败
	//	return false;
	//}
	GRASS_CONTAINER* pGrass = CORE_NEW(GRASS_CONTAINER);

	m_GrassAll.Add(name, pGrass);

	//iter = m_GrassAll.find(name);

	//if (iter == m_GrassAll.end())
	//{
	//	// 已经添加成功，应该不会获取失败
	//	return false;
	//}

	//m_CurGrass = &(iter->second);
	m_CurGrass = pGrass;

	return true;
}

bool TerrainCopy::BeginSave(const char* file_name)
{
	Assert(NULL != file_name);

	if (0 == file_name[0])
	{
		return false;
	}

	CVarList args;
	CVarList result;

	args.AddString(file_name);

	// 分割文件名和扩展名
	ext_split_file_name(args, result);

	const char* name = result.StringVal(0);
	const char* dot = result.StringVal(1);

	m_strFileName = name;

	//std::map<std::string, std::vector<blend_t*>>::iterator iter = 
	//	m_BlendAll.find(name);

	//if (iter == m_BlendAll.end())
	if (!m_BlendAll.Exists(name))
	{
		// 没有保存过数据，则仅需要添加节点
		return InsertNewNode(name);
	}
	else
	{
		// 清空原数据
		return ClearOldData(name);
	}
}

bool TerrainCopy::Save()
{
	if (m_bCopyBlend)
	{
		CollectBlendData();
		SaveBlendData((m_strFileName + ".blend").c_str());
	}

	if (m_bCopyGrass)
	{
		CollectGrassData();
		SaveGrassData((m_strFileName + ".grass").c_str());
	}

	return true;
}

bool TerrainCopy::ModifyTerrain()
{
	if (m_bCopyBlend)
	{
		UseBlendDataModifyTerrain();
	}

	if (m_bCopyGrass)
	{
		UseGrassDataModifyTerrain();
	}

	return true;
}

bool TerrainCopy::CollectBlendData()
{
	float unit_size = m_pTerrain->GetUnitSize();
	float brush_unit_size = unit_size / m_pTerrain->GetAlphaPerUnit();
	float cols = m_vEndPos.x - m_vStartPos.x;
	float rows = m_vEndPos.z - m_vStartPos.z;

	cols = (cols / unit_size) * (1 / brush_unit_size);
	rows = (rows / unit_size) * (1 / brush_unit_size);

	for (int r = 0; r < int(rows); ++r)
	{
		for (int c = 0; c < int(cols); ++c)
		{
			float offset_x = c * brush_unit_size;
			float offset_z = r * brush_unit_size;
			float x = m_vStartPos.x + offset_x;
			float z = m_vStartPos.z + offset_z;
			int blend_layer_count = m_pEditor->GetTexBlendLayerCount(x, z);

			for (int i = 0; i < blend_layer_count; ++i)
			{
				// 获得指定位置的指定层的混合贴图索引
				int index = m_pEditor->GetTexBlendLayerTexIndex(x, z, i);
				// 获得指定位置的指定层的混合贴图样式
				int format = m_pEditor->GetTexBlendLayerFormat(x, z, i);
				// 获得混合贴图在当前位置的值
				float value = m_pEditor->GetTexBlend(x, z, index, format);

				if (value > 0.0)
				{
					// 记录相对位置和混合贴图索引
					const char* blend_name = 
						m_pTerrain->GetBlendTexName(index);
					blend_t* data = CORE_NEW(blend_t);

					data->strName = blend_name;
					data->x = offset_x;
					data->z = offset_z;
					data->value = value;
					data->format = format;

					m_CurBlend->push_back(data);
				}
			}
		}
	}

	return true;
}

int TerrainCopy::GetBlendDataCount() const
{
	return (*m_CurBlend).size();
}

const char* TerrainCopy::GetBlendDataName(int index)
{
	if (size_t(index) >= (*m_CurBlend).size())
	{
		return "";
	}

	blend_t* data = (*m_CurBlend)[index];

	return data->strName.c_str();
}

const float TerrainCopy::GetBlendDataX(int index)
{
	if (size_t(index) >= (*m_CurBlend).size())
	{
		return 0.0F;
	}

	blend_t* data = (*m_CurBlend)[index];

	return data->x;
}

const float TerrainCopy::GetBlendDataZ(int index)
{
	if (size_t(index) >= (*m_CurBlend).size())
	{
		return 0.0F;
	}

	blend_t* data = (*m_CurBlend)[index];

	return data->z;
}

const float TerrainCopy::GetBlendDataValue(int index)
{
	if (size_t(index) >= (*m_CurBlend).size())
	{
		return 0.0F;
	}

	blend_t* data = (*m_CurBlend)[index];

	return data->value;
}

const int TerrainCopy::GetBlendDataFormat(int index)
{
	if (size_t(index) >= (*m_CurBlend).size())
	{
		return 0;
	}

	blend_t* data = (*m_CurBlend)[index];

	return data->format;
}

// 混合贴图信息文件头
struct blend_file_header_t
{
	unsigned int nBlendCount;	// 数量
};

int TerrainCopy::LoadBlendData(const char* file_name)
{
	FILE* fp = core_file::fopen(file_name, "rb");

	if (NULL == fp)
	{
		return -1;
	}

	blend_file_header_t header;

	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		core_file::fclose(fp);
		return -1;
	}

	// 读取
	for (size_t i = 0; i < header.nBlendCount; ++i)
	{
		// 名称
		unsigned int name_len;

		if (core_file::fread(&name_len, sizeof(unsigned int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		if (name_len > 256)
		{
			core_file::fclose(fp);
			return -1;
		}

		TAutoMem<char, 256, TCoreAlloc> auto_buf(name_len + 1);
		char* name = auto_buf.GetBuffer();

		if (core_file::fread(name, sizeof(char), name_len, fp) != name_len)
		{
			core_file::fclose(fp);
			return -1;
		}

		name[name_len] = 0;

		// x
		float x = 0.0F;

		if (core_file::fread(&x, sizeof(float), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// z
		float z = 0.0F;

		if (core_file::fread(&z, sizeof(float), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// value
		float value = 0.0F;

		if (core_file::fread(&value, sizeof(float), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// format
		int format = 0;

		if (core_file::fread(&format, sizeof(int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		blend_t* data = CORE_NEW(blend_t);

		data->strName = name;
		data->x = x;
		data->z = z;
		data->value = value;
		data->format = format;

		(*m_CurBlend).push_back(data);
	}

	core_file::fclose(fp);

	return 1;
}

int TerrainCopy::SaveBlendData(const char* file_name)
{
	size_t nBlendCount = GetBlendDataCount();

	if (0 == nBlendCount)
	{
		return 0;
	}

	FILE* fp = core_file::fopen(file_name, "wb");

	if (NULL == fp)
	{
		return -1;
	}

	blend_file_header_t header;

	header.nBlendCount = nBlendCount;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		core_file::fclose(fp);
		return -1;
	}

	// 写入
	for (size_t i = 0; i < nBlendCount; ++i)
	{
		// 名称
		const char* name = GetBlendDataName(i);

		unsigned int name_len = strlen(name) + 1;

		if (core_file::fwrite(&name_len, sizeof(unsigned int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		if (core_file::fwrite(name, sizeof(char), name_len, fp) != name_len)
		{
			core_file::fclose(fp);
			return -1;
		}

		// x
		float x = GetBlendDataX(i);

		if (core_file::fwrite(&x, sizeof(float), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// z
		float z = GetBlendDataZ(i);

		if (core_file::fwrite(&z, sizeof(float), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// value
		float value = GetBlendDataValue(i);

		if (core_file::fwrite(&value, sizeof(float), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// format
		int format = GetBlendDataFormat(i);

		if (core_file::fwrite(&format, sizeof(int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}
	}

	core_file::fclose(fp);

	return 1;
}

bool TerrainCopy::UseBlendDataModifyTerrain()
{
	int blend_data_count = GetBlendDataCount();

	if (blend_data_count == 0)
	{
		return false;
	}

	float rotate = m_fAngle;

	float unit_size = m_pTerrain->GetUnitSize();
	float brush_unit_size = unit_size / m_pTerrain->GetAlphaPerUnit();

	float cols = m_vEndPos.x - m_vStartPos.x;
	float rows = m_vEndPos.z - m_vStartPos.z;

	cols = (cols / unit_size) * (1 / brush_unit_size);
	rows = (rows / unit_size) * (1 / brush_unit_size);

	bool changed = false;

	// 清除当前范围的混合地表贴图
	if (m_bClearBlend)
	{
		for (int r = 0; r < int(rows); ++r)
		{
			for (int c = 0; c < int(cols); ++c)
			{
				float offset_x = c * brush_unit_size;
				float offset_z = r * brush_unit_size;

				float x = m_vStartPos.x + offset_x;
				float z = m_vStartPos.z + offset_z;

				CVarList result;

				get_new_pos(NULL, x, z, m_vCenterPos.x, m_vCenterPos.z, 
					rotate, result);
				
				float new_x = result.FloatVal(0);
				float new_z = result.FloatVal(2);

				m_pEditor->EraseAllBlend(new_x, new_z);

				changed = true;
			}
		}
	}
	
	IEntity* pUndoManager = GetCore()->LookupEntity("UndoManager");

	CVarList res;

	GetCore()->InvokeMethod(pUndoManager, "BeginCommand", 
		CVarList() << "blend_tex", res);

	char key[128] = { 0 };

	for (int i = 0; i < blend_data_count; ++i)
	{
		const char* blend_name = GetBlendDataName(i);
		const float relative_x = GetBlendDataX(i);
		const float relative_z = GetBlendDataZ(i);
		const float value = GetBlendDataValue(i);
		const int format = GetBlendDataFormat(i);
		
		if (blend_name[0] != 0 && value > 0.0 
			&& m_pTerrain->FindBlendTex(blend_name))
		{
			int tex_index = m_pTerrain->GetBlendTexIndex(blend_name);
			
			if (tex_index >= 0)
			{
				float x = m_vStartPos.x + relative_x;
				float z = m_vStartPos.z + relative_z;

				CVarList result;

				get_new_pos(NULL, x, z, m_vCenterPos.x, m_vCenterPos.z, 
					rotate, result);

				float new_x = result.FloatVal(0);
				float new_z = result.FloatVal(2);

				int layers = m_pEditor->GetTexBlendLayerCount(x, z);

				if (layers >= 4)
				{
					if (!m_pEditor->FindTexBlendLayer(new_x, new_z, tex_index, 
						format))
					{
						// 不能超过6层混合贴图
						continue;
					}
				}

				float old_value = m_pEditor->GetTexBlend(new_x, new_z, 
					tex_index, format);

				if (old_value == value)
				{
					continue;
				}

				if (value > 0.95)
				{
					// 清除以前的混合贴图，防止过多重叠
					m_pEditor->EraseAllBlend(new_x, new_z);
				}

				if (!m_pEditor->SetTexBlend(new_x, new_z, tex_index, format, 
					value))
				{
					continue;
				}

				changed = true;

				if (blend_data_count < 65535)
				{
					SafeSprintf(key, sizeof(key), "%.3f,%.3f", new_x, new_z);

					run_callback(this, "on_blend_action", CVarList() << key 
						<< new_x << new_z << tex_index << format << old_value 
						<< value);
				}
			}
		}
	}

	if (changed)
	{
		m_pEditor->RangeBlendChanged(m_vStartPos.x, m_vStartPos.z, 
			m_vEndPos.x, m_vEndPos.z);
	}

	GetCore()->InvokeMethod(pUndoManager, "EndCommand", CVarList(), res);
	
	return true;
}

bool TerrainCopy::CollectGrassData()
{
	float unit_size = m_pTerrain->GetUnitSize();
	float brush_unit_size = unit_size;

	float cols = m_vEndPos.x - m_vStartPos.x;
	float rows = m_vEndPos.z - m_vStartPos.z;

	cols = (cols / unit_size) * (1 / brush_unit_size);
	rows = (rows / unit_size) * (1 / brush_unit_size);

	size_t all_grass_count = m_pTerrain->GetGrassCount();

	for (size_t i = 0; i < all_grass_count; ++i)
	{
		const char* grass_name = m_pTerrain->GetGrassName(i);
		int grass_index = m_pTerrain->GetGrassIndex(grass_name);

		for (int r = 0; r < int(rows); ++r)
		{
			for (int c = 0; c < int(cols); ++c)
			{
				float offset_x = c * brush_unit_size;
				float offset_z = r * brush_unit_size;

				float x = m_vStartPos.x + offset_x;
				float z = m_vStartPos.z + offset_z;

				CVarList args, result;

				args.AddInt(grass_index);
				args.AddFloat(x);
				args.AddFloat(z);

				m_pEditor->GetGrass(args, result);

				if (result.GetCount() > 0)
				{
					grass_t* data = CORE_NEW(grass_t);

					data->strName = grass_name;
					data->x = offset_x;
					data->z = offset_z;
					data->nScale = result.IntVal(0);
					data->nAmount = result.IntVal(1);
					data->nSeed = result.IntVal(2);

					(*m_CurGrass).push_back(data);
				}
			}
		}
	}

	return true;
}

int TerrainCopy::GetGrassDataCount() const
{
	return (*m_CurGrass).size();
}

const char* TerrainCopy::GetGrassDataName(int index)
{
	if (size_t(index) >= (*m_CurGrass).size())
	{
		return "";
	}

	grass_t* data = (*m_CurGrass)[index];

	return data->strName.c_str();
}

const float TerrainCopy::GetGrassDataX(int index)
{
	if (size_t(index) >= (*m_CurGrass).size())
	{
		return 0.0F;
	}

	grass_t* data = (*m_CurGrass)[index];

	return data->x;
}

const float TerrainCopy::GetGrassDataZ(int index)
{
	if (size_t(index) >= (*m_CurGrass).size())
	{
		return 0.0F;
	}

	grass_t* data = (*m_CurGrass)[index];

	return data->z;
}

const int TerrainCopy::GetGrassDataScale(int index)
{
	if (size_t(index) >= (*m_CurGrass).size())
	{
		return 0;
	}

	grass_t* data = (*m_CurGrass)[index];

	return data->nScale;
}

const int TerrainCopy::GetGrassDataAmount(int index)
{
	if (size_t(index) >= (*m_CurGrass).size())
	{
		return 0;
	}

	grass_t* data = (*m_CurGrass)[index];

	return data->nAmount;
}

const int TerrainCopy::GetGrassDataSeed(int index)
{
	if (size_t(index) >= (*m_CurGrass).size())
	{
		return 0;
	}

	grass_t* data = (*m_CurGrass)[index];

	return data->nSeed;
}

// 信息文件头
struct grass_file_header_t
{
	unsigned int nGrassCount;	// 数量
};

int TerrainCopy::LoadGrassData(const char* file_name)
{
	FILE* fp = core_file::fopen(file_name, "rb");

	if (NULL == fp)
	{
		return -1;
	}

	grass_file_header_t header;

	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		core_file::fclose(fp);
		return -1;
	}

	// 读取
	for (size_t i = 0; i < header.nGrassCount; ++i)
	{
		// 名称
		unsigned int name_len;

		if (core_file::fread(&name_len, sizeof(unsigned int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		if (name_len > 256)
		{
			core_file::fclose(fp);
			return -1;
		}

		TAutoMem<char, 256, TCoreAlloc> auto_buf(name_len + 1);
		char* name = auto_buf.GetBuffer();

		if (core_file::fread(name, sizeof(char), name_len, fp) != name_len)
		{
			core_file::fclose(fp);
			return -1;
		}

		name[name_len] = 0;

		// x
		float x = 0.0F;

		if (core_file::fread(&x, sizeof(float), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// z
		float z = 0.0F;

		if (core_file::fread(&z, sizeof(float), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// scale
		int scale = 0;

		if (core_file::fread(&scale, sizeof(int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// amount
		int amount = 0;

		if (core_file::fread(&amount, sizeof(int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// seed
		int seed = 0;

		if (core_file::fread(&seed, sizeof(int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		grass_t* data = CORE_NEW(grass_t);

		data->strName = name;
		data->x = x;
		data->z = z;
		data->nScale = scale;
		data->nAmount = amount;
		data->nSeed = seed;

		(*m_CurGrass).push_back(data);
	}

	core_file::fclose(fp);

	return 1;
}

int TerrainCopy::SaveGrassData(const char* file_name)
{
	size_t nGrassCount = GetGrassDataCount();

	if (0 == nGrassCount)
	{
		return 0;
	}

	FILE* fp = core_file::fopen(file_name, "wb");

	if (NULL == fp)
	{
		return -1;
	}

	grass_file_header_t header;

	header.nGrassCount = nGrassCount;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		core_file::fclose(fp);
		return -1;
	}

	// 写入
	for (size_t i = 0; i < nGrassCount; ++i)
	{
		// 名称
		const char* name = GetGrassDataName(i);

		unsigned int name_len = strlen(name) + 1;

		if (core_file::fwrite(&name_len, sizeof(unsigned int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		if (core_file::fwrite(name, sizeof(char), name_len, fp) != name_len)
		{
			core_file::fclose(fp);
			return -1;
		}

		// x
		float x = GetGrassDataX(i);

		if (core_file::fwrite(&x, sizeof(float), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// z
		float z = GetGrassDataZ(i);

		if (core_file::fwrite(&z, sizeof(float), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// scale
		int scale = GetGrassDataScale(i);

		if (core_file::fwrite(&scale, sizeof(int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// amount
		int amount = GetGrassDataAmount(i);

		if (core_file::fwrite(&amount, sizeof(int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}

		// seed
		int seed = GetGrassDataSeed(i);

		if (core_file::fwrite(&seed, sizeof(int), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return -1;
		}
	}

	core_file::fclose(fp);

	return 1;
}

bool TerrainCopy::UseGrassDataModifyTerrain()
{
	int data_count = GetGrassDataCount();

	if (data_count == 0)
	{
		return false;
	}

	float rotate = m_fAngle;

	float unit_size = m_pTerrain->GetUnitSize();
	float brush_unit_size = unit_size;

	float cols = m_vEndPos.x - m_vStartPos.x;
	float rows = m_vEndPos.z - m_vStartPos.z;

	cols = (cols / unit_size) * (1 / brush_unit_size);
	rows = (rows / unit_size) * (1 / brush_unit_size);

	// 清除当前范围的
	if (m_bClearGrass)
	{
		if (FloatEqual(0.0F, m_fAngle) || FloatEqual(360.0F, m_fAngle))
		{
			for (int i = 0; i < data_count; ++i)
			{
				const char* grass_name = GetGrassDataName(i);
				int grass_index = m_pTerrain->GetGrassIndex(grass_name);

				m_pEditor->RemoveGrassRect(grass_index, m_vStartPos.x, 
					m_vStartPos.z, m_vEndPos.x, m_vEndPos.z);
			}
		}
		else
		{
			for (int r = 0; r < int(rows); ++r)
			{
				for (int c = 0; c < int(cols); ++c)
				{
					float offset_x = c * brush_unit_size;
					float offset_z = r * brush_unit_size;

					float x = m_vStartPos.x + offset_x;
					float z = m_vStartPos.z + offset_z;

					CVarList result;

					get_new_pos(NULL, x, z, m_vCenterPos.x, m_vCenterPos.z, 
						rotate, result);

					float new_x = result.FloatVal(0);
					float new_z = result.FloatVal(2);

					m_pEditor->RemoveGrassRect(-1, new_x - 1.0F, new_z - 1.0F, 
						new_x + 1.0F, new_z + 1.0F);
				}
			}
		}
	}

	IEntity* pUndoManager = GetCore()->LookupEntity("UndoManager");

	CVarList res;

	GetCore()->InvokeMethod(pUndoManager, "BeginCommand", 
		CVarList() << "grass", res);

	char key[128] = { 0 };

	for (int i = 0; i < data_count; ++i)
	{
		const char* grass_name = GetGrassDataName(i);
		const float relative_x = GetGrassDataX(i);
		const float relative_z = GetGrassDataZ(i);
		const int scale = GetGrassDataScale(i);
		const int amount = GetGrassDataAmount(i);
		const int seed = GetGrassDataSeed(i);

		int grass_index = m_pTerrain->GetGrassIndex(grass_name);

		if (grass_index >= 0)
		{
			float x = m_vStartPos.x + relative_x;
			float z = m_vStartPos.z + relative_z;

			CVarList args, result;

			get_new_pos(NULL, x, z, m_vCenterPos.x, m_vCenterPos.z, rotate, 
				result);

			float new_x = result.FloatVal(0);
			float new_z = result.FloatVal(2);

			result.Clear();

			args.AddInt(grass_index);
			args.AddFloat(new_x);
			args.AddFloat(new_z);

			m_pEditor->GetGrass(args, result);

			int old_scale = 0;
			int old_amount = 0;
			int old_seed = 0;

			if (result.GetCount() > 0)
			{
				old_scale = result.IntVal(0);
				old_amount = result.IntVal(1);
				old_seed = result.IntVal(2);

				m_pEditor->RemoveGrass(grass_index, new_x, new_z);
			}

			if (m_pEditor->AddGrass(grass_index, new_x, new_z, scale, amount, 
				seed))
			{
				if (data_count < 65535)
				{
					SafeSprintf(key, sizeof(key), "%.3f,%.3f", new_x, new_z);

					run_callback(this, "on_grass_action", 
						CVarList() << key << grass_index << new_x << new_z 
						<< scale << old_scale << amount << old_amount << seed
						<< old_seed);
				}
			}
		}
	}

	GetCore()->InvokeMethod(pUndoManager, "EndCommand", CVarList(), res);

	// 刷新显示
	m_pTerrain->RefreshGrass();

	return true;
}

bool TerrainCopy::ClearOldData(const char* name)
{
	if (!ClearBlendData(name))
	{
		return false;
	}

	if (!ClearGrassData(name))
	{
		return false;
	}

	return InsertNewNode(m_strFileName.c_str());;
}

bool TerrainCopy::ClearBlendData(const char* name)
{
	//std::map<std::string, std::vector<blend_t*>>::iterator iter = 
	//	m_BlendAll.find(name);
	BLEND_CONTAINER* pBlend;

	//if (iter != m_BlendAll.end())
	if (m_BlendAll.GetData(name, pBlend))
	{
		//std::vector<blend_t*>& vData = iter->second;

		for (size_t i = 0; i < pBlend->size(); ++i)
		{
			CORE_DELETE((*pBlend)[i]);
		}

		CORE_DELETE(pBlend);
		//vData.clear();
		m_BlendAll.Remove(name);
		return true;
	}

	return false;
}

bool TerrainCopy::ClearAllBlendData()
{
	//std::map<std::string, std::vector<blend_t*>>::iterator iter = 
	//	m_BlendAll.begin();
	TStringPod<char, BLEND_CONTAINER*, TStringTraits<char>,
		TCoreAlloc>::iterator iter = m_BlendAll.Begin();

	while (iter != m_BlendAll.End())
	{
		BLEND_CONTAINER* pBlend = iter.GetData();

		for (size_t i = 0; i < pBlend->size(); ++i)
		{
			CORE_DELETE((*pBlend)[i]);
		}

		//vData.clear();
		CORE_DELETE(pBlend);
		++iter;
	}

	m_BlendAll.Clear();

	return true;
}

bool TerrainCopy::ClearGrassData(const char* name)
{
	//std::map<std::string, std::vector<grass_t*>>::iterator iter = 
	//	m_GrassAll.find(name);
	GRASS_CONTAINER* pGrass;

	//if (iter != m_GrassAll.end())
	if (m_GrassAll.GetData(name, pGrass))
	{
		//GRASS_CONTAINER* pGrass = iter.GetData();

		for (size_t i = 0; i < pGrass->size(); ++i)
		{
			CORE_DELETE((*pGrass)[i]);
		}

		CORE_DELETE(pGrass);
		//vData.clear();
		m_GrassAll.Remove(name);
		return true;
	}

	return false;
}

bool TerrainCopy::ClearAllGrassData()
{
	//std::map<std::string, std::vector<grass_t*>>::iterator iter = 
	//	m_GrassAll.begin();
	TStringPod<char, GRASS_CONTAINER*, TStringTraits<char>,
		TCoreAlloc>::iterator iter = m_GrassAll.Begin();

	while (iter != m_GrassAll.End())
	{
		GRASS_CONTAINER* pGrass = iter.GetData();

		for (size_t i = 0; i < pGrass->size(); ++i)
		{
			CORE_DELETE((*pGrass)[i]);
		}

		CORE_DELETE(pGrass);
		//vData.clear();
		++iter;
	}

	m_GrassAll.Clear();

	return true;
}
*/