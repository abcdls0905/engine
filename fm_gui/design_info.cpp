//--------------------------------------------------------------------
// 文件名:		design_info.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "design_info.h"
#include "../public/inlines.h"

// CDesignInfo

CDesignInfo::CDesignInfo()
{
}

CDesignInfo::~CDesignInfo()
{
	for (size_t k = 0; k < m_PropInfo.size(); ++k)
	{
		CORE_DELETE(m_PropInfo[k]);
	}

	for (size_t j = 0; j < m_SuborInfo.size(); ++j)
	{
		CORE_DELETE(m_SuborInfo[j]);
	}

	for (size_t i = 0; i < m_EventInfo.size(); ++i)
	{
		CORE_DELETE(m_EventInfo[i]);
	}
}

bool CDesignInfo::FindPropInfoIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);
	const size_t SIZE1 = m_PropInfo.size();

	for (size_t i = 0; i < SIZE1; i++)
	{
		if ((m_PropInfo[i]->nHash == hash) 
			&& (m_PropInfo[i]->strName == name))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool CDesignInfo::FindPropInfo(const char* name) const
{
	size_t index;

	return FindPropInfoIndex(name, index);
}

bool CDesignInfo::AddPropInfo(const char* name, int type, bool save, 
	bool subor_save, const IVar* def_value)
{
	size_t index;
	
	if (FindPropInfoIndex(name, index))
	{
		return false;
	}
	
	prop_info_t* data = CORE_NEW(prop_info_t);
	
	data->strName = name;
	data->nHash = GetHashValueCase(name);
	data->nType = type;
	data->bSave = save;
	data->bSuborSave = subor_save;

	if (def_value != NULL)
	{
		data->DefValue.Assign(*def_value);
	}
	else
	{
		switch (type)
		{
		case TYPE_BOOL:
			data->DefValue = CVar(VTYPE_BOOL, false);
			break;
		case TYPE_INT:
			data->DefValue = CVar(VTYPE_INT, 0);
			break;
		case TYPE_INT64:
			data->DefValue = CVar(VTYPE_INT64, int64_t(0));
			break;
		case TYPE_FLOAT:
			data->DefValue = CVar(VTYPE_FLOAT, 0.0F);
			break;
		case TYPE_DOUBLE:
			data->DefValue = CVar(VTYPE_DOUBLE, 0.0);
			break;
		case TYPE_STRING:
			data->DefValue = CVar(TYPE_STRING, "");
			break;
		case TYPE_WIDESTR:
			data->DefValue = CVar(TYPE_WIDESTR, L"");
			break;
		case TYPE_FONT:
			data->DefValue = CVar(TYPE_STRING, "");
			break;
		case TYPE_COLOR:
			data->DefValue = CVar(TYPE_STRING, "");
			break;
		case TYPE_IMAGE:
			data->DefValue = CVar(TYPE_STRING, "");
			break;
		case TYPE_CURSOR:
			data->DefValue = CVar(TYPE_STRING, "");
			break;
        case TYPE_SOUND:
            data->DefValue = CVar(TYPE_STRING, "");
            break;
		case TYPE_FILE:
			data->DefValue = CVar(TYPE_STRING, "");
			break;
		default:
			Assert(0);
			break;
		}
	}

	m_PropInfo.push_back(data);

	return true;
}

bool CDesignInfo::RemovePropInfo(const char* name)
{
	size_t index;

	if (!FindPropInfoIndex(name, index))
	{
		return false;
	}

	CORE_DELETE(m_PropInfo[index]);
	m_PropInfo.remove(index);

	return true;
}

size_t CDesignInfo::GetPropCount() const
{
	return m_PropInfo.size();
}

size_t CDesignInfo::GetPropList(IVarList& result) const
{
	result.Clear();

	for (size_t i = 0; i < m_PropInfo.size(); ++i)
	{
		result.AddString(m_PropInfo[i]->strName.c_str());
	}
	
	return result.GetCount();
}

const char* CDesignInfo::GetPropName(size_t index) const
{
	if (index >= m_PropInfo.size())
	{
		return "";
	}

	return m_PropInfo[index]->strName.c_str();
}

int CDesignInfo::GetPropType(const char* name) const
{
	size_t index;
	
	if (!FindPropInfoIndex(name, index))
	{
		return 0;
	}

	return m_PropInfo[index]->nType;
}

bool CDesignInfo::GetPropSave(const char* name) const
{
	size_t index;
	
	if (!FindPropInfoIndex(name, index))
	{
		return false;
	}
	
	return m_PropInfo[index]->bSave;
}

bool CDesignInfo::GetPropSuborSave(const char* name) const
{
	size_t index;
	
	if (!FindPropInfoIndex(name, index))
	{
		return false;
	}
	
	return m_PropInfo[index]->bSuborSave;
}

bool CDesignInfo::GetPropDefValue(const char* name, IVar& def_value) const
{
	size_t index;
	
	if (!FindPropInfoIndex(name, index))
	{
		return false;
	}
	
	def_value.Assign(m_PropInfo[index]->DefValue);

	return true;
}

const char* CDesignInfo::GetPropTypeString(int type) const
{
	switch (type)
	{
	case TYPE_BOOL:
		return "bool";
	case TYPE_INT:
		return "int";
	case TYPE_INT64:
		return "int64";
	case TYPE_FLOAT:
		return "float";
	case TYPE_DOUBLE:
		return "double";
	case TYPE_STRING:
		return "string";
	case TYPE_WIDESTR:
		return "widestr";
	case TYPE_FONT:
		return "font";
	case TYPE_COLOR:
		return "color";
	case TYPE_IMAGE:
		return "image";
	case TYPE_CURSOR:
		return "cursor";
    case TYPE_SOUND:
        return "sound";
	case TYPE_FILE:
		return "file";
	default:
		break;
	}

	return "unknown";
}

bool CDesignInfo::FindSuborInfoIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);
	
	unsigned int hash = GetHashValueCase(name);
	const size_t SIZE1 = m_SuborInfo.size();
	
	for (size_t i = 0; i < SIZE1; ++i)
	{
		if ((m_SuborInfo[i]->nHash == hash)
			&& (m_SuborInfo[i]->strName == name))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool CDesignInfo::FindSuborInfo(const char* name) const
{
	size_t index;

	return FindSuborInfoIndex(name, index);
}

bool CDesignInfo::AddSuborInfo(const char* name, const char* type)
{
	Assert(type != NULL);

	if (FindSuborInfo(name))
	{
		return false;
	}

	subor_info_t* data = CORE_NEW(subor_info_t);

	data->strName = name;
	data->nHash = GetHashValueCase(name);
	data->strType = type;

	m_SuborInfo.push_back(data);

	return true;
}

bool CDesignInfo::RemoveSuborInfo(const char* name)
{
	size_t index;

	if (!FindSuborInfoIndex(name, index))
	{
		return false;
	}

	CORE_DELETE(m_SuborInfo[index]);
	m_SuborInfo.remove(index);

	return true;
}

size_t CDesignInfo::GetSuborCount() const
{
	return m_SuborInfo.size();
}

size_t CDesignInfo::GetSuborList(IVarList& result) const
{
	result.Clear();
	
	for (size_t i = 0; i < m_SuborInfo.size(); ++i)
	{
		result.AddString(m_SuborInfo[i]->strName.c_str());
	}
	
	return result.GetCount();
}

const char* CDesignInfo::GetSuborType(const char* name) const
{
	size_t index;
	
	if (!FindSuborInfoIndex(name, index))
	{
		return "";
	}
	
	return m_SuborInfo[index]->strType.c_str();
}

bool CDesignInfo::FindEventInfoIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);
	
	unsigned int hash = GetHashValueCase(name);
	const size_t SIZE1 = m_EventInfo.size();
	
	for (size_t i = 0; i < SIZE1; ++i)
	{
		if ((m_EventInfo[i]->nHash == hash)
			&& (m_EventInfo[i]->strName == name))
		{
			index = i;
			return true;
		}
	}
	
	return false;
}

bool CDesignInfo::FindEventInfo(const char* name) const
{
	size_t index;

	return FindEventInfoIndex(name, index);
}

bool CDesignInfo::AddEventInfo(const char* name)
{
	if (FindEventInfo(name))
	{
		return false;
	}
	
	event_info_t* data = CORE_NEW(event_info_t);
	
	data->strName = name;
	data->nHash = GetHashValueCase(name);

	m_EventInfo.push_back(data);
	
	return true;
}

bool CDesignInfo::RemoveEventInfo(const char* name)
{
	size_t index;
	
	if (!FindEventInfoIndex(name, index))
	{
		return false;
	}
	
	CORE_DELETE(m_EventInfo[index]);
	m_EventInfo.remove(index);
	
	return true;
}

size_t CDesignInfo::GetEventCount() const
{
	return m_EventInfo.size();
}

size_t CDesignInfo::GetEventList(IVarList& result) const
{
	result.Clear();
	
	for (size_t i = 0; i < m_EventInfo.size(); ++i)
	{
		result.AddString(m_EventInfo[i]->strName.c_str());
	}
	
	return result.GetCount();
}