
#pragma once

#include "../public/i_entity.h"
#include "../extern/tinyxml/tinyxml.h"
#include <string>
#include <vector>
#include <map>

struct UIConfig 
{
	std::string strStage; //UI�����׶�
	std::string strLogicName; //UI�߼�����
	std::string strFormPath;  //UI��Դ�����ļ�·��
	bool bStatic; //�����ļ��Ƿ�פ�ڴ棨Ƶ����UIʹ�ã�
	bool bPrLoad; //�Ƿ�Ԥ�ȼ���
	bool bAsyncLoad; //ui�첽����
	bool bCenter; //�Ƿ����

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

	//����ĳ�׶ε�ui(���������������Ƿ�Ԥ����)
	bool LoadStageUI(const std::string& szStageName);

	//ж��ĳ�׶�UI
	bool UnLoadStageUI(const std::string& szStageName);


	//��ȡ�����߼����Ӧ��������
	bool GetFormConfig(std::string szLogicName, UIConfig& uiConfig);

private:
	//
	bool InitStageUI(const char* szStageName, TiXmlElement* element);

private:
	//�׶ζ�Ӧ�����б�
	std::map<std::string, std::vector<UIConfig> > m_mapStageToFormList;

	//�߼�ʵ������Ӧ������Ϣ
	std::map<std::string, UIConfig>  m_mapNameToFormInfo;
};