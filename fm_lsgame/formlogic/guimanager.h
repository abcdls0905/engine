
#pragma once

#include "../public/i_entity.h"
#include "../extern/tinyxml/tinyxml.h"
#include <string>
#include <vector>
#include <map>

struct UIConfig 
{
	std::string strStage; //UI所属阶段
	std::string strLogicName; //UI逻辑类名
	std::string strFormPath;  //UI资源配置文件路径
	bool bStatic; //配置文件是否常驻内存（频繁打开UI使用）
	bool bPrLoad; //是否预先加载
	bool bAsyncLoad; //ui异步加载
	bool bCenter; //是否居中

	UIConfig()
	{
		bStatic = false;
		bPrLoad = false;
		bAsyncLoad = false;
		bCenter = true;

		strStage = "";
		strLogicName = "";
		strFormPath = "";
	}
};

//
class GuiManager : public IEntity
{
public:
	GuiManager();
	virtual ~GuiManager();

	virtual bool	Init(const IVarList& args);
	virtual bool	Shut();

public:
	//
	bool InitConfig();

	//加载某阶段的ui(根据配置来决定是否预加载)
	bool LoadStageUI(const std::string& szStageName);

	//卸载某阶段UI
	bool UnLoadStageUI(const std::string& szStageName);


	//获取界面逻辑类对应界面配置
	bool GetFormConfig(std::string szLogicName, UIConfig& uiConfig);

private:
	//
	bool InitStageUI(const char* szStageName, TiXmlElement* element);

private:
	//阶段对应界面列表
	std::map<std::string, std::vector<UIConfig> > m_mapStageToFormList;

	//逻辑实体名对应界面信息
	std::map<std::string, UIConfig>  m_mapNameToFormInfo;
};