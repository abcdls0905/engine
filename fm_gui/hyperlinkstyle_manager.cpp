//--------------------------------------------------------------------
// �ļ���:		HyperLinkStyleManager.cpp
// ��  ��:		��������ʽ���ù���
// ˵  ��:		
// ��������:	2008��8��22��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "hyperlinkstyle_manager.h"
#include "gui_utils.h"
#include "../visual/i_painter.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"
#include "../public/core_log.h"
#include "../extern/rapidxml/rapidxml.hpp"
//#include "../sdk/tinyxml/tinyxml.h"
using namespace rapidxml;
class xml_parser_node;
class xml_parser : public rapidxml::xml_document<char>
{
	typedef TAutoMem<char, 256> XMLStringBuffer;
	typedef xml_document<char> BaseType;
public:
	xml_parser():m_auto_buf(NULL){}
	~xml_parser(){ if(m_auto_buf) delete m_auto_buf; }
	bool Parse(const char* szPathFile);
	xml_parser_node* first_node(const char* Name){ return (xml_parser_node*)BaseType::first_node(Name); }
private:
	XMLStringBuffer* m_auto_buf;
	static const char* const EMPTY_STRING;

};

class xml_parser_node:public rapidxml::xml_node<char>
{
	typedef xml_node<char> BaseType;
public:
	const char* Attribute(const char* name, const char* szDefaultValue = STRING_DEFALUT_VALUE);
	xml_parser_node* FirstChildElement(const char* Name){ return (xml_parser_node*)BaseType::first_node(Name); }
	xml_parser_node* NextSiblingElement(const char* Name = NULL);
protected:
	static const char* STRING_DEFALUT_VALUE;
};

/// entity: HyperLinkStyleManager
/// \brief ��������ʽ���ù���ʵ��
DECLARE_ENTITY(HyperLinkStyleManager, 0, IEntity);
#if 0

/// event: int on_click_hyperlink(object self, string data)
/// \brief �����ͼƬ
/// \param self ��ʵ��
/// \param data ������Ϣ
DECLARE_EVENT(on_click_hyperlink);

/// event: int on_mousein_hyperlink(object self, string data)
/// \brief �����ͼƬ
/// \param self ��ʵ��
/// \param data ������Ϣ
DECLARE_EVENT(on_mousein_hyperlink);

/// event: int on_mouseout_hyperlink(object self, string data)
/// \brief �����ͼƬ
/// \param self ��ʵ��
/// \param data ������Ϣ
DECLARE_EVENT(on_mouseout_hyperlink);

/// event: int on_doubleclick_hyperlink(object self, string data)
/// \brief �����ͼƬ
/// \param self ��ʵ��
/// \param data ������Ϣ
DECLARE_EVENT(on_doubleclick_hyperlink);
#endif

HyperLinkStyleManager::HyperLinkStyleManager()
{
}

bool HyperLinkStyleManager::Init(const IVarList& args)
{
	using namespace rapidxml;
	const char* pPainterName = args.StringVal(0);

	IPainter* pPainter = (IPainter*)GetCore()->GetInterface(pPainterName);
	if( !pPainter )
	{
		pPainter = (IPainter*)GetCore()->GetInterface("Painter");
	}
	if (NULL == pPainter)
	{
		return false;
	}

	// ��ȡ������Ϣ
	std::string fname = std::string(pPainter->GetDefaultPath()) 
		+ "skin\\HyperLinkStyle.xml";

    SAFE_RELEASE(pPainter);

	//TiXmlDocument doc(fname.c_str());
	xml_parser doc;

	//if (doc.LoadFile())
	if (doc.Parse(fname.c_str()))
	{
		if(!doc.first_node("Root"))
		{

			CORE_TRACE("HyperLinkStyleManager xml file don't find Root node!");
			return false;
		}
		xml_parser_node* pElement = doc.first_node("Root")->FirstChildElement(
			"HyperLinkStyle");

		while (pElement)
		{
			StyleItem styleitem;

			styleitem.szStyleName = pElement->Attribute("Name");

			xml_parser_node* pStyle = pElement->FirstChildElement("Style");

			while (pStyle)
			{
				const char* pValue = pStyle->Attribute("TypeID");

				long index = 0;

				if (pValue)
				{
					index = atol(pValue);

					if (index < 0 || index >= HYPERLINK_MAXSTATE)
					{
						continue;
					}
				}

				// ����
				styleitem.elements[index].szFont = 
					pStyle->Attribute("FontName");

				// ��ɫ
				pValue = pStyle->Attribute("Color");

				if (pValue)
				{
					styleitem.elements[index].color = 
						GuiUtil_ColorToInt(pValue);
				}

				// �����С
				pValue = pStyle->Attribute("Size");

				if (pValue)
				{
					styleitem.elements[index].size = atol(pValue);
				}

				pStyle = pStyle->NextSiblingElement("Style");
			}

			m_mStyleNameToIndex.Add(styleitem.szStyleName.c_str(), 
				m_vStyleItems.size());

			m_vStyleItems.push_back(styleitem);

			//m_mStyleNameToIndex[styleitem.szStyleName] = m_vStyleItems.size();

			pElement = pElement->NextSiblingElement("HyperLinkStyle");
		}
	}

	// ���óɵ���ʵ��
	//GetCore()->SetSingle(this);

	return true;
}

bool HyperLinkStyleManager::Shut()
{
	m_vStyleItems.clear();

	return true;
}

bool HyperLinkStyleManager::GetStyleByState(const char* pStyleName, 
	HyperLinkState state, StyleElement*& pElement)
{
	if (NULL == pStyleName)
	{
		return false;
	}

	if (state == HYPERLINK_MAXSTATE)
	{
		return false;
	}

	size_t index;

	if (!m_mStyleNameToIndex.GetData(pStyleName, index))
	{
		return false;
	}

	if (index >= m_vStyleItems.size())
	{
		return false;
	}

	// ���ֱ�Ӹ�ֵ������Σ��
	pElement = &m_vStyleItems[index].elements[state];

	return true;
	//std::map<std::string, long>::iterator it = m_mStyleNameToIndex.find(
	//	pStyleName);

	//if (it != m_mStyleNameToIndex.end())
	//{
	//	long index = it->second;

	//	if (index < 1 || index > (long)m_vStyleItems.size())
	//	{
	//		return false;
	//	}

		// ���ֱ�Ӹ�ֵ������Σ��
	//	pElement = &m_vStyleItems[index - 1].elements[state];

	//	return true;
	//}

	//return false;
}

////�ؼ��ص�
//int GuiUtil_RunCallback(IEntity* pControl, const char* event, 
//						const IVarList& args)
//{
//	if(pControl)
//	{
//		CVarList _args;
//		_args << pControl->GetID();
//		_args.Append(args, 0, args.GetCount());
//		return event_send(pControl->GetID(), event, _args);
//	}
//	else
//	{
//		return event_send(pControl->GetID(), event, args);
//	}
//}
//

bool HyperLinkStyleManager::OnClickHyperLink(std::wstring &szLinkData)
{
//     GuiUtil_RunCallback(this, "on_click_hyperlink", 
//         CVarList() << szLinkData.c_str());

    return true;
}

bool HyperLinkStyleManager::OnMouseInHyperLink(std::wstring &szLinkData)
{
//     GuiUtil_RunCallback(this, "on_mousein_hyperlink", 
//         CVarList() << szLinkData.c_str());

    return true;
}

bool HyperLinkStyleManager::OnMouseOutHyperLink(std::wstring &szLinkData)
{
//     GuiUtil_RunCallback(this, "on_mouseout_hyperlink", 
//         CVarList() << szLinkData.c_str());

    return true;
}

bool HyperLinkStyleManager::OnDoubleClickHyperLink(std::wstring &szLinkData)
{
//     GuiUtil_RunCallback(this, "on_doubleclick_hyperlink", 
//         CVarList() << szLinkData.c_str());

    return true;
}

bool HyperLinkStyleManager::OnRightClickHyperLink(std::wstring &szLinkData)
{
//     GuiUtil_RunCallback(this, "on_right_click_hyperlink", 
//         CVarList() << szLinkData.c_str());

    return true;
}