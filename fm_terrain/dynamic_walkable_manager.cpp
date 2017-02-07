//--------------------------------------------------------------------
// 文件名:		DynamicWalkableManager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月1日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "dynamic_walkable_manager.h"

#include "dynamic_walkable.h"
#include "terrain.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include "../public/var.h"
#include "../public/var_list.h"
#include "../public/portable.h"

// CDynamicWalkableManager
#define MAX_PATH 260

extern void extern_to_2n(size_t& row, size_t& col);

// 分割文件路径和文件名
static void ext_split_file_path(const IVarList& args, IVarList& result)
{
	const char* path = args.StringVal(0);
	char buf[MAX_PATH];
	char* file_part;

	if (0 == Port_GetFullPathName(path, buf, sizeof(buf), &file_part))
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

CDynamicWalkableManager::CDynamicWalkableManager()
{
	m_pTerrain = NULL;
}

CDynamicWalkableManager::~CDynamicWalkableManager()
{
	for(size_t i = 0; i < m_VisualWalkableArr.size(); i++ )
	{
		if(m_VisualWalkableArr[i])
		{
			m_VisualWalkableArr[i]->~CDynamicWalkable();
			CORE_FREE(m_VisualWalkableArr[i], sizeof(CDynamicWalkable));
		}
	}
	m_VisualWalkableArr.clear();
}

void CDynamicWalkableManager::SetTerrain(Terrain* pTerrain)
{
	Assert(NULL != pTerrain);

	m_pTerrain = pTerrain;
}

result_string CDynamicWalkableManager::GetVisualName(IVisBase* pVisBase) const
{
	result_string res;
	res.clear();

	IVar* pVar = pVisBase->GetCustoms()->GetValue("DynamicWalkName");

	if (NULL == pVar)
	{
		return res;
	}

	const char* full_file_name = pVar->StringVal();

	if (0 == strcmp(full_file_name, ""))
	{
		return res;
	}

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

	res = result.StringVal(0);

	return res;
}

result_string CDynamicWalkableManager::GetVisualExtName(IVisBase* pVisBase) const
{
	Assert(NULL != pVisBase);
	result_string res;
	res.clear();

	FmVec3 angle = pVisBase->GetAngle();
	pVisBase->SetAngle(angle.x, 0.0f, angle.z);
	size_t rows = (size_t)(pVisBase->GetBoxSizeZ() / m_pTerrain->GetUnitSize() * m_pTerrain->GetCollidePerUnit() + 0.5f);
	size_t cols = (size_t)(pVisBase->GetBoxSizeX() / m_pTerrain->GetUnitSize() * m_pTerrain->GetCollidePerUnit() + 0.5f);
	pVisBase->SetAngle(angle.x, angle.y, angle.z);

	IVar* pVar = pVisBase->GetCustoms()->GetValue("DynamicWalkName");

	if (NULL == pVar)
	{
		return res;
	}

	const char* full_file_name = pVar->StringVal();

	if (0 == strcmp(full_file_name, ""))
	{
		return res;
	}

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

	char wfname[256];
	SafeSprintf(wfname, 256, "%s_%d_%d", result.StringVal(0), rows, cols);
	res = wfname;

	return res;
}

bool CDynamicWalkableManager::AddDynamicWalkable(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);

	const char* mode_file_name = GetVisualName(pVisBase).c_str();

	if (NULL == mode_file_name || 0 == strcmp(mode_file_name, ""))
	{
		CORE_TRACE("AddDynamicWalkable false! mode_file_name:");
		if(mode_file_name)
		{
			CORE_TRACE(mode_file_name);
		}
		else
		{
			CORE_TRACE("");
		}
		return false;
	}

	const char* file_name = GetVisualExtName(pVisBase).c_str();

	if (NULL == file_name || 0 == strcmp(file_name, ""))
	{
        CORE_TRACE("AddDynamicWalkable false! file_name:");
 		if(file_name)
		{
			CORE_TRACE(file_name);
		}
		else
		{
			CORE_TRACE("");
		}
		return false;
	}

	for (size_t i = 0; i < m_VisualWalkableArr.size(); ++i)
	{
		if (0 == strcmp(file_name, m_VisualWalkableArr[i]->GetFileName()))
		{
			return m_VisualWalkableArr[i]->AddDynamicWalkable(pVisBase);
		}
	}

	FmVec3 angle = pVisBase->GetAngle();
	pVisBase->SetAngle(angle.x, 0.0f, angle.z);
	size_t rows = (size_t)(pVisBase->GetBoxSizeZ() / m_pTerrain->GetUnitSize() * m_pTerrain->GetCollidePerUnit() + 0.5f);
	size_t cols = (size_t)(pVisBase->GetBoxSizeX() / m_pTerrain->GetUnitSize() * m_pTerrain->GetCollidePerUnit() + 0.5f);
	pVisBase->SetAngle(angle.x, angle.y, angle.z);

	extern_to_2n(rows, cols);

	CDynamicWalkable* dynamic_walkable = 
		(CDynamicWalkable*)CORE_ALLOC(sizeof(CDynamicWalkable));
	new (dynamic_walkable) CDynamicWalkable(m_pTerrain, pVisBase, mode_file_name, file_name, rows, cols);

	m_VisualWalkableArr.push_back(dynamic_walkable);

	size_t index = m_VisualWalkableArr.size() - 1;
	return m_VisualWalkableArr[index]->AddDynamicWalkable(pVisBase);
}

bool CDynamicWalkableManager::AddDynamicWalkable2(IVisBase* pVisBase)
{
	Assert(NULL != pVisBase);

	const char* mode_file_name = GetVisualName(pVisBase).c_str();

	if (NULL == mode_file_name || 0 == strcmp(mode_file_name, ""))
	{
		CORE_TRACE("AddDynamicWalkable2 false! mode_file_name:");
		if(mode_file_name)
		{
			CORE_TRACE(mode_file_name);
		}
		else
		{
			CORE_TRACE("");
		}
		return false;
	}

	const char* file_name = GetVisualExtName(pVisBase).c_str();

	if (NULL == file_name || 0 == strcmp(file_name, ""))
	{
        CORE_TRACE("AddDynamicWalkable2 false! file_name:");
 		if(file_name)
		{
			CORE_TRACE(file_name);
		}
		else
		{
			CORE_TRACE("");
		}
		return false;
	}

	for (size_t i = 0; i < m_VisualWalkableArr.size(); ++i)
	{
		if (0 == strcmp(file_name, m_VisualWalkableArr[i]->GetFileName()))
		{
			return true;
		}
	}

	FmVec3 angle = pVisBase->GetAngle();
	pVisBase->SetAngle(angle.x, 0.0f, angle.z);
	size_t rows = (size_t)(pVisBase->GetBoxSizeZ() / m_pTerrain->GetUnitSize() * m_pTerrain->GetCollidePerUnit() + 0.5f);
	size_t cols = (size_t)(pVisBase->GetBoxSizeX() / m_pTerrain->GetUnitSize() * m_pTerrain->GetCollidePerUnit() + 0.5f);
	pVisBase->SetAngle(angle.x, angle.y, angle.z);

	extern_to_2n(rows, cols);

	CDynamicWalkable* dynamic_walkable = 
		(CDynamicWalkable*)CORE_ALLOC(sizeof(CDynamicWalkable));
	new (dynamic_walkable) CDynamicWalkable(m_pTerrain, pVisBase, mode_file_name, file_name, rows, cols);

	m_VisualWalkableArr.push_back(dynamic_walkable);

	size_t index = m_VisualWalkableArr.size() - 1;
	return true;
}

bool CDynamicWalkableManager::DeleteDynamicWalkable(IVisBase* pVisBase)
{
	const char* file_name = GetVisualExtName(pVisBase).c_str();

	if (NULL == file_name || 0 == strcmp(file_name, ""))
	{
		return false;
	}

	for (size_t i = 0; i < m_VisualWalkableArr.size(); ++i)
	{
		CDynamicWalkable* dynamic_walkable = m_VisualWalkableArr[i];
		if (0 == strcmp(file_name, dynamic_walkable->GetFileName()))
		{
			if (dynamic_walkable->DeleteDynamicWalkable(pVisBase))
			{
				if (!dynamic_walkable->IsLoaded())
				{
					dynamic_walkable->~CDynamicWalkable();
					CORE_FREE(dynamic_walkable, sizeof(CDynamicWalkable));
					dynamic_walkable = NULL;

					m_VisualWalkableArr.remove(i);
				}

				return true;
			}

			return false;
		}
	}

	return false;
}

bool CDynamicWalkableManager::LoadDynamicWalkable(IVisBase* pVisBase)
{
	const char* file_name = GetVisualExtName(pVisBase).c_str();

	if (NULL == file_name || 0 == strcmp(file_name, ""))
	{
		return false;
	}

	for (size_t i = 0; i < m_VisualWalkableArr.size(); ++i)
	{
		CDynamicWalkable* dynamic_walkable = m_VisualWalkableArr[i];
		if (0 == strcmp(file_name, dynamic_walkable->GetFileName()))
		{
			return dynamic_walkable->Load();
		}
	}

	return false;
}

bool CDynamicWalkableManager::UnloadDynamicWalkable(IVisBase* pVisBase)
{
	const char* file_name = GetVisualExtName(pVisBase).c_str();

	if (NULL == file_name || 0 == strcmp(file_name, ""))
	{
		return false;
	}

	for (size_t i = 0; i < m_VisualWalkableArr.size(); ++i)
	{
		CDynamicWalkable* dynamic_walkable = m_VisualWalkableArr[i];
		if (0 == strcmp(file_name, dynamic_walkable->GetFileName()))
		{
			return dynamic_walkable->Unload();
		}
	}

	return false;
}

CDynamicWalkable* CDynamicWalkableManager::GetDynamicWalkable(IVisBase* pVisBase) const
{
	const char* file_name = GetVisualExtName(pVisBase).c_str();

	if (NULL == file_name || 0 == strcmp(file_name, ""))
	{
		return NULL;
	}

	for (size_t i = 0; i < m_VisualWalkableArr.size(); ++i)
	{
		if (0 == strcmp(file_name, m_VisualWalkableArr[i]->GetFileName()))
		{
			return m_VisualWalkableArr[i];
		}
	}

	return NULL;
}
