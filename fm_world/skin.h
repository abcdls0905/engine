//--------------------------------------------------------------------
// 文件名:		skin.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年2月10日
// 创建人:		惠军超
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SKIN_H
#define _SKIN_H

#include "../public/core_type.h"
#include "../visual/i_model_system.h"
#include "../visual/i_vis_base.h"
// 调试物理引擎需要的头文件
//#include "../physics/PhysicsMacros.h"
#include "../visual/i_render.h"
#include "../visual/i_skin.h"

// 带骨骼动画的模型

class Skin: public ISkin
{
	IRender* m_pRender;
public:
	Skin();
	virtual ~Skin();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual bool Load();

	virtual void Update(float seconds);
	virtual void Realize();
	virtual void RealizeNormal();
	virtual void RealizeReflect();
	virtual void RealizeShadowMap();

	// 加载数据是否成功
	virtual bool GetLoaded();
	// 是否加载完成
	virtual bool GetLoadFinish(); 

	virtual void SetWorldMatrix(const FmMat4& mat);
	virtual void GetWorldMatrix(FmMat4& mat);
	
	virtual bool SetPosition(float x, float y, float z);
	virtual bool SetAngle(float x, float y, float z);
	virtual bool SetScale(float x, float y, float z);

	virtual FmVec3 GetPosition() const;
	virtual FmVec3 GetAngle() const;
	virtual FmVec3 GetScale() const;

	virtual FmVec3 GetCenter();
	virtual FmVec3 GetBoxSize();
	virtual float GetRadius();

	// 碰撞测试详细信息
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result);
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// 球体范围碰撞
    virtual bool TraceSphere(const FmVec3& center, float radius);
    // 三角形碰撞
    virtual bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2);

	// 拣选
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// 是否可见
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// 是否在水下
	virtual void SetUnderWater(bool value);
	virtual bool GetUnderWater() const;

	// 颜色
	virtual void SetColor(unsigned int value);
	virtual unsigned int GetColor() const;

	void SetColorString(const char* value);
	result_string GetColorString() const;

	// 获得模型播放器
	IModelPlayer* GetModelPlayer() const { return m_pModelPlayer; }

	// 动画播放器
	void SetActionPlayer(IActionPlayer* value);
	IActionPlayer* GetActionPlayer() const;

	// 是否检测碰撞
	virtual void SetTraceEnable(bool value);
	virtual bool GetTraceEnable() const;

	// 是否可以剪裁
	virtual void SetCullEnable(bool value);
	virtual bool GetCullEnable() const;

	// 获取当前模型是否存在主模（裸模）
	bool ExistMainModel();
	// 指定当前模型仅对主模（裸模）进行碰撞检测
	void SetTraceMainModel(bool value);
	// 获取当前模型是否仅对主模（裸模）进行碰撞检测
	bool GetTraceMainModel();

	// 名称（测试用）
	const char* GetName() const { return m_strModelFile.c_str(); }

	// 是否异步加载
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// 是否显示包围盒
	void SetShowBoundBox(bool value) { m_bShowBoundBox = value; }
	bool GetShowBoundBox() const { return m_bShowBoundBox; }

	// 设置材质文件
	void SetMaterialFile(const char* value);
	const char* GetMaterialFile() const;

	// 加载材质文件
	bool LoadMaterial();

	// 模型文件
	void SetModelFile(const char* pszModelFile);
	const char* GetModelFile();

	// 贴图路径
	void SetTexPaths(const char* value);
	const char* GetTexPaths() const;

	// 设置骨骼动画模型
	bool AddModel(IActionPlayer* pActionPlayer, const char* pszModelFile);

	// 获得根节点名列表
	void GetRootNodeList(const IVarList& args, IVarList& result);
	// 获得辅助节点名列表
	void GetHelperNodeList(const IVarList& args, IVarList& result);
	// 获得辅助点的当前位置
	void GetHelperPosition(const IVarList& args, IVarList& result);
	// 获得辅助点的当前方向
	void GetHelperAngle(const IVarList& args, IVarList& result);
	// 根据名字获得节点索引
	int GetNodeFromName(const char* pszName);
	// 获得指定节点的矩阵
	void GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM);
	bool GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM);

	// 获得指定摄像机类型节点的信息
	void GetCameraInfo(const IVarList& args, IVarList& result);

	// 节点信息
	size_t GetObjectCount() const;
	size_t GetObjectVertexCount(size_t index) const;
	size_t GetObjectTriangleCount(size_t index) const;
	int GetAllVertexCount() const;
	int GetAllTriangleCount();

	// 获得模型节点个数
	int GetNodeCount();
	// 获得模型的所有节点名列表
	void GetNodeNameList(const IVarList& args, IVarList& result);
	// 获得模型节点数据是否可编辑
	bool GetModelNodeAllowModify(const char* key);
	// 设置模型节点数据
	bool SetNodeValue(const char* node_name, const char* key, const char* val);
	// 读取模型节点数据
	result_string GetNodeValue(const char* node_name, const char* key);

	// 获取材质数
	int GetMaterialCount() const;
	// 获取材质的贴图数
	int GetMaterialTexCount(int index) const;
	// 获取指定材质的的纹理贴图文件
	const char* GetMaterialTexName(int mat_index, int tex_index) const;

	// 获得模型的所有材质名列表
	void GetMaterialNameList(const IVarList& args, IVarList& result);
	// 设置模型材质数据
	bool SetMaterialValue(const char* mat_name, const char* key, 
		const char* val);
	// 读取模型材质数据
	result_string GetMaterialValue(const char* mat_name, const char* key);
	// 重新加载模型材质的所有贴图
	bool ReloadMaterialTextures();

	// 查找是否存在自定义材质数据
	bool FindCustomMaterial(const char* mat_name);
	// 设置自定义材质数据
	bool SetCustomMaterialValue(const char* mat_name, const char* key, 
		const char* val);
	// 读取自定义材质数据
	result_string GetCustomMaterialValue(const char* mat_name, const char* key);
	// 重新加载自定义材质的指定贴图
	bool ReloadCustomMaterialTexture(const char* mat_name, const char* key);
	// 重新加载自定义材质的所有贴图
	bool ReloadCustomMaterialTextures();

	// 重新设置蒙皮模型的骨骼map
	bool ResetSkinMap();

private:
	// 更新包围盒
	void UpdateBoundingBox();

private:
	IModelSystem* m_pModelSystem;
	bool m_bVisible;
	bool m_bUnderWater;
	bool m_bTraceEnable;
	bool m_bCullEnable;
	bool m_bAsyncLoad;
	bool m_bShowBoundBox;
	unsigned int m_nColor;
	core_string m_strModelFile;
	core_string m_strMaterialFile;
	core_string m_strTexPaths;
	IModelPlayer* m_pModelPlayer;
	IActionPlayer* m_pActionPlayer;
	FmMat4 m_mtxWorld;
	FmVec3 m_vBoundMin;
	FmVec3 m_vBoundMax;
	float m_fRadius;
	bool m_bUpdatedBoundingBox;
};

#endif // _SKIN_H
