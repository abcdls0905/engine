//--------------------------------------------------------------------
// 文件名:		design_res.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月21日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "design_res.h"

// CDesignRes

CDesignRes::CDesignRes()
{
}

CDesignRes::~CDesignRes()
{
}

bool CDesignRes::FindCollect(const char* file) const
{
	Assert(file != NULL);

	return m_Collects.Exists(file);
}

bool CDesignRes::AddCollect(const char* file)
{
	Assert(file != NULL);
	
	return m_Collects.Add(file, 0);
}

bool CDesignRes::ClearCollect()
{
	m_Collects.Clear();

	return true;
}

size_t CDesignRes::GetResFileCount(size_t type) const
{
	Assert(type < RESTYPE_MAX);

	return m_Resources[type].GetCount();
}

void CDesignRes::GetResFileList(size_t type, IVarList& result)
{
	Assert(type < RESTYPE_MAX);
	
	result.Clear();

	name_container_t::iterator it = m_Resources[type].Begin();

	for (; it != m_Resources[type].End(); ++it)
	{
		result.AddString(it.GetKey());
	}
}

bool CDesignRes::FindResFile(size_t type, const char* file) const
{
	Assert(type < RESTYPE_MAX);
	
	return m_Resources[type].Exists(file);
}

bool CDesignRes::AddResFile(size_t type, const char* file)
{
	Assert(type < RESTYPE_MAX);
	
	return m_Resources[type].Add(file, 0);
}

bool CDesignRes::RemoveResFile(size_t type, const char* file)
{
	Assert(type < RESTYPE_MAX);
	
	return m_Resources[type].Remove(file);
}

bool CDesignRes::ClearResFile(size_t type)
{
	Assert(type < RESTYPE_MAX);

	m_Resources[type].Clear();

	return true;
}

size_t CDesignRes::GetResType(const char* res_name) const
{
	//if (stricmp(res_name, "color") == 0)
	//{
	//	return RESTYPE_COLOR;
	//}
	//else if (stricmp(res_name, "image") == 0)
	//{
	//	return RESTYPE_IMAGE;
	//}
	//else 
	if (stricmp(res_name, "font") == 0)
	{
		return RESTYPE_FONT;
	}
	else if (stricmp(res_name, "cursor") == 0)
	{
		return RESTYPE_CURSOR;
	}
    else if (stricmp(res_name, "sound") == 0)
    {
        return RESTYPE_SOUND;
    }

	return RESTYPE_MAX;
}

const char* CDesignRes::GetResName(size_t type) const
{
	switch (type)
	{
	//case RESTYPE_COLOR:
	//	return "color";
	//case RESTYPE_IMAGE:
	//	return "image";
	case RESTYPE_FONT:
		return "font";
	case RESTYPE_CURSOR:
		return "cursor";
    case RESTYPE_SOUND:
        return "sound";
	default:
		break;
	}
	
	return "";
}

