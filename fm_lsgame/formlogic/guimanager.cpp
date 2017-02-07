
#include "guimanager.h"
#include "../public/persistid.h"
#include "../public/i_entity.h"
#include "../public/core_log.h"

#include "../fm_gui/gui.h"
#include "../fm_gui/form.h"
#include "../command.h"

#include "../util/logic_util.h"

//
DECLARE_ENTITY(GuiManager, 0, IEntity);

//
GuiManager::GuiManager()
{
	m_mapStageToFormList.clear();
	m_mapNameToFormInfo.clear();
}

//
GuiManager::~GuiManager()
{
	m_mapStageToFormList.clear();
	m_mapNameToFormInfo.clear();
}

bool GuiManager::Init(const IVarList& args)
{
	return true;
}

bool GuiManager::Shut()
{
	return true;
}

//
bool GuiManager::InitConfig()
{
	m_mapStageToFormList.clear();
	m_mapNameToFormInfo.clear();

	//
	std::string strResPath = GetCore()->GetResourcePath();
	strResPath += "skin/uiload.xml";

	TiXmlDocument  xmlDocument;
	if (!load_xml_doc(xmlDocument, strResPath.c_str()))
	{
		return false;
	}

	TiXmlElement* pRootElement = xmlDocument.RootElement();
	if (pRootElement == NULL)
	{
		return false;
	}

	TiXmlElement* eleSection = pRootElement->FirstChildElement("stage");
	while (eleSection)
	{
		std::string strName = eleSection->Attribute("Name");

		//
		InitStageUI(strName.c_str(), eleSection);


		eleSection = eleSection->NextSiblingElement("stage");
	}
	return true;
}

//加载某阶段的ui(根据配置来决定是否预加载)
bool GuiManager::LoadStageUI(const std::string& szStageName)
{
	std::map<std::string, std::vector<UIConfig> >::iterator itFind = m_mapStageToFormList.find(szStageName.c_str());
	if (itFind == m_mapStageToFormList.end())
	{
		return false;
	}

	//
	std::vector<UIConfig> vecFormList = itFind->second;
	for (size_t i=0; i<vecFormList.size(); i++)
	{
		//界面预先加载
		UIConfig config = vecFormList[i];
		if (config.bPrLoad)
		{
		}
	}

	return true;
}

//卸载某阶段UI
bool GuiManager::UnLoadStageUI(const std::string& szStageName)
{
	std::map<std::string, std::vector<UIConfig> >::iterator itFind = m_mapStageToFormList.find(szStageName.c_str());
	if (itFind == m_mapStageToFormList.end())
	{
		return false;
	}

	//
	std::vector<UIConfig> vecFormList = itFind->second;
	for (size_t i=0; i<vecFormList.size(); i++)
	{
		//界面卸载
		UIConfig config = vecFormList[i];
	}
	return true;
}


//获取界面逻辑类对应界面配置
bool GuiManager::GetFormConfig(std::string szLogicName, UIConfig& uiConfig)
{
	std::map<std::string, UIConfig>::iterator itFind = m_mapNameToFormInfo.find(szLogicName.c_str());
	if (itFind == m_mapNameToFormInfo.end())
	{
		return false;
	}

	//
	uiConfig = itFind->second;
	
	return true;
}

//
bool GuiManager::InitStageUI(const char* szStageName, TiXmlElement* element)
{
	std::map<std::string, std::vector<UIConfig> >::iterator it = m_mapStageToFormList.find(szStageName);
	if (it != m_mapStageToFormList.end())
	{
		return false;
	}

	if (element == NULL)
	{
		return false;
	}

	std::vector<UIConfig> vecFormList;

	TiXmlElement* eleItem = element->FirstChildElement("Property");
	while (eleItem)
	{
		std::string strLogicName = eleItem->Attribute("LogicName");
		std::string strFileName = eleItem->Attribute("FilePath");
		bool bStatic = strcmp(eleItem->Attribute("Static"), "true") == 0?true:false;
		bool bPrLoad = strcmp(eleItem->Attribute("PrLoad"), "true") == 0?true:false;
		bool bAsyncLoad = strcmp(eleItem->Attribute("AsyncLoad"), "true") == 0?true:false;
		bool bCenter = strcmp(eleItem->Attribute("Center"), "true") == 0?true:false;

		if(strFileName != "" && strLogicName != "")
		{
			UIConfig config;
			config.strLogicName = strLogicName;
			config.strFormPath = strFileName;
			config.strStage = szStageName;

			config.bStatic = bStatic;
			config.bPrLoad = bPrLoad;
			config.bAsyncLoad = bAsyncLoad;
			config.bCenter = bCenter;

			vecFormList.push_back(config);

			//
			m_mapNameToFormInfo[strLogicName] = config;
		}

		eleItem = eleItem->NextSiblingElement("Property");
	}

	//
	m_mapStageToFormList[szStageName] = vecFormList;

	return true;
}