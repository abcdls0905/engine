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
	
	// 获得模型的所有材质名列表
	virtual void GetMaterialNameList(const IVarList& args, IVarList& result) = 0;
	
	// 设置模型材质数据
	virtual bool SetMaterialValue(const char* mat_name, const char* key, const char* val) = 0;

	virtual bool ReloadMaterialTextures() = 0;
	
	virtual result_string GetMaterialValue(const char* mat_name, const char* key) = 0;

	virtual IActionPlayer* GetActionPlayer() const = 0;

	virtual bool SetCustomMaterialValue(const char* mat_name, const char* key, 
		const char* val) = 0;

	// 查找是否存在自定义材质数据
	virtual bool FindCustomMaterial(const char* mat_name) = 0;

	// 重新加载自定义材质的指定贴图
	virtual bool ReloadCustomMaterialTexture(const char* mat_name, const char* key) = 0;

	// 重新加载自定义材质的所有贴图
	virtual bool ReloadCustomMaterialTextures() = 0;

		// 读取自定义材质数据
	virtual result_string GetCustomMaterialValue(const char* mat_name, const char* key) = 0;
};

#endif