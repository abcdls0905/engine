#ifndef _VISUAL_I_SKIN_H
#define _VISUAL_I_SKIN_H

#include "../public/core_type.h"
#include "../visual/i_model_system.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"

class ISkin : public IVisBase
{
public:
	virtual IModelPlayer* GetModelPlayer() const  = 0;
	
	// ���ģ�͵����в������б�
	virtual void GetMaterialNameList(const IVarList& args, IVarList& result) = 0;
	
	// ����ģ�Ͳ�������
	virtual bool SetMaterialValue(const char* mat_name, const char* key, const char* val) = 0;

	virtual bool ReloadMaterialTextures() = 0;
	
	virtual result_string GetMaterialValue(const char* mat_name, const char* key) = 0;

	virtual IActionPlayer* GetActionPlayer() const = 0;

	virtual bool SetCustomMaterialValue(const char* mat_name, const char* key, 
		const char* val) = 0;

	// �����Ƿ�����Զ����������
	virtual bool FindCustomMaterial(const char* mat_name) = 0;

	// ���¼����Զ�����ʵ�ָ����ͼ
	virtual bool ReloadCustomMaterialTexture(const char* mat_name, const char* key) = 0;

	// ���¼����Զ�����ʵ�������ͼ
	virtual bool ReloadCustomMaterialTextures() = 0;

		// ��ȡ�Զ����������
	virtual result_string GetCustomMaterialValue(const char* mat_name, const char* key) = 0;
};

#endif