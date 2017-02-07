

#pragma once
#include "form_base.h"
#include <string>
#include <map>

#include "../helper/exact_model_config.h"

struct ModelGroup
{
	PERSISTID role_model;
	std::string role_show_action;

	PERSISTID stage_model;
};

//
class FormCreateRole : public FormBase
{
public:
	typedef std::map<std::string, std::wstring> PropValueType;
	typedef std::map<int, PropValueType> PropValueMapType;

	typedef std::map<int, ModelGroup> ModelGroupType;
public:
	FormCreateRole();
	~FormCreateRole();

	static FormCreateRole* Instance();

	bool	OnSlidLeft(float fSpeed, float fDist);
	bool	OnSlidEnd(const IVarList& args);

	void	StopSlid();

	std::wstring	OnCreateSucceedMessage();
private:
	virtual bool	Init(const IVarList& args);
	virtual bool	Shut();

	virtual void	OnCreate();
	virtual void	OnShow(bool isShowed);
	virtual void	OnClosed();
	virtual void	Execute(float seconds);

    void OnSockClosed(const char* event, const IVarList& arg);

	void InitFormPosition();

	std::wstring _getRolePara(const std::string& propName, int index);
	void _parseRolePara(const std::wstring& para, int index);
	
	bool _is_angle_near(float first, float second);

	void _createServerRoleModel(int serverIndex);
	void _createLocalRoleModel(int roleIndex);
	void _removeRoleModel(int roleIndex);

	// ������ɫ���µ�̨��ģ��
	void _createStageModel(int roleIndex);

    // ������ɫ��Ϣui
    void _createRoleInfoUi(int roleIndex);
    void _removeRoleInfoUi() ;

	// ������ģ
	void _createExactModel(int roleIndex);
	// ��ʾ��ģ��չʾ����
	void _playShowAction(int roleIndex);

	void _setRolePosition();
	int  _getServerIndex(int roleIndex);
	int _getServerNewLocation(); 

	void _selectRoleChanged(int roleIndex);

	void OnBtnEnterGame(const char* szEvent, const IVarList& arg);
	void OnBtnCreateRole(const char* szEvent, const IVarList& arg);
	void OnBtnRandName(const char* szEvent, const IVarList& arg);
	void OnBtnBack(const char* szEvent, const IVarList& arg);

	PropValueMapType m_ServerRoleList;
	ModelGroupType m_ModelGroups;

	int m_SelectRoleIndex;

	// ��ǰ�����н�ɫΧ�����ĵ���ת�Ƕ�
	float m_CurYawAngle;

	bool m_bSlipping;
	bool m_bSlippingLeft;

	ExactModelConfig m_ExactModelConfig;
};
