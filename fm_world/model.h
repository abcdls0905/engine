//--------------------------------------------------------------------
// 文件名:		model.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _MODEL_H
#define _MODEL_H

#include "../public/core_type.h"

#include "../visual/i_vis_base.h"
#include "../visual/i_model_player.h"
#include "../visual/i_render.h"
#include "../visual/i_model.h"

// 调试物理引擎需要的头文件
//#include "../physics/PhysicsMacros.h"

// 3D模型对象

class IModelSystem;

class Model: public IModel
{
	bool m_updateCenter;
public:
	Model();
	virtual ~Model();

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

	// 加载完成后
	bool AfterLoaded();

	// 位置
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// 方向
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// 缩放系数
	virtual bool SetScale(float x, float y, float z); 
	virtual FmVec3 GetScale() const;

	// 全局中心点
	virtual FmVec3 GetCenter(); 
	// 包围盒尺寸
	virtual FmVec3 GetBoxSize();
	// 模型半径
	virtual float GetRadius(); 

	// 世界转换矩阵
	virtual void SetWorldMatrix(const FmMat4& mat); 
	virtual void GetWorldMatrix(FmMat4& mat);

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

	// 获得模型播放器
	virtual IModelPlayer* GetModelPlayer() { return m_pModelPlayer; }

	// 名称（测试用）
	const char* GetName() const { return m_strModelFile.c_str(); }

	// 是否已经加载光照
	bool GetLightLoaded() const;
	// 是否可用光贴图
	bool GetHasLightMap() const;
	// 是否可用法线贴图
	bool GetHasNormalMap() const;
	// 是否可用定点色
	bool GetHasVertexColor() const;

	// 是否异步加载
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// 是否显示包围盒
	void SetShowBoundBox(bool value);
	bool GetShowBoundBox() const;

	// 是否使用顶点光照
	void SetUseVertexColor(bool value);
	bool GetUseVertexColor() const;

	// 是否使用光贴图
	void SetUseLightMap(bool value);
	bool GetUseLightMap() const;

	// 是否在摄像机焦点位置半透明显示
	//void SetFocusTransparent(bool value);
	//bool GetFocusTransparent() const;

	// 是否检测碰撞
	virtual void SetTraceEnable(bool value);
	virtual bool GetTraceEnable() const;

	// 是否可以剪裁
	virtual void SetCullEnable(bool value);
	virtual bool GetCullEnable() const;

	// 是否需要水面反射
	virtual void SetWaterReflect(bool value) { m_bWaterReflect = value; }
	virtual bool GetWaterReflect() const { return m_bWaterReflect; }

	// 写入速度纹理标志
	virtual void SetWriteVelocity(int value);
	virtual int GetWriteVelocity() const;

	// 记录临时的透明度变量（只供地形系统使用）
	virtual void SetAlphaValue(float value) { m_fAlphaValue = value; }
	virtual float GetAlphaValue() const { return m_fAlphaValue; }

	// 不需要投射影子
	virtual void SetCastShadow(bool value);
	virtual bool GetCastShadow() const { return m_bCastShadow; }

	// 不需要接受投射影子
	virtual void SetReceiveShadow(bool value);
	virtual bool GetReceiveShadow() const { return m_bReceiveShadow; }

	// 模型颜色
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// 附加信息
	void SetExtraInfo(int value);
	int GetExtraInfo() const;

	// 模型文件
	void SetModelFile(const char* value); 
	const char* GetModelFile() const;

	// 光照文件
	void SetLightFile(const char* value);
	const char* GetLightFile() const;

	// 材质文件
	void SetMaterialFile(const char* value);
	const char* GetMaterialFile() const;

	// 贴图路径
	void SetTexPaths(const char* value);
	const char* GetTexPaths() const;

	// 增加焦点半透明材质
	//bool AddFocusTransparent(int mat_index);
	// 清除所有焦点半透明材质
	//bool ClearFocusTransparent();

	// 加载光照文件
	bool LoadLight(); 
	// 加载材质文件
	bool LoadMaterial();
	
	// 碰撞测试获得高度
	float TraceHeight(float x, float z, float high_y, float low_y); 

	// 获得辅助点组名列表
	void GetHelperGroupList(const IVarList& args, IVarList& result);
	// 获得辅助点名字列表
	void GetHelperNameList(const IVarList& args, IVarList& result);
	// 获得辅助点的当前位置
	void GetHelperPosition(const IVarList& args, IVarList& result);
	// 获得辅助点的当前方向
	void GetHelperAngle(const IVarList& args, IVarList& result);
	
	// 获得指定点（摄像机）的信息
	void GetCameraInfo(const IVarList& args, IVarList& result);

	// 是否显示辅助点
	void ShowHelper(bool bShow);
	bool IsShowHelper() const;

	// 获得节点矩阵
	void GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM);
	bool GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM);
	// 根据名字查找节点
	int GetNodeFromName(const char* pszName);
	// 获得指定节点的名字
	const char* GetHelperName(int helpID);

	// 当前选择的节点
	void SetSelectHelper(int nHelper);
	int GetSelectHelper();
	
	// 播放
	bool Play();
	// 停止
	bool Stop();
	// 暂停
	bool Pause();
	// 恢复播放
	bool Resume();
	
	// 是否循环
	void SetLoop(bool bLoop);
	bool GetLoop();
	// 获得帧数
	int GetFrameCount();
	// 获得当前播放帧
	int GetCurrentFrame();
	// 设置当前播放帧
	bool SetCurrentFrame(int nFrame);
	// 获得当前播放时间
	double GetCurrentTimeFloat();
	// 设置当前播放时间
	bool SetCurrentTimeFloat(double fTime);
	// 获得缺省播放速度
	float GetDefaultSpeed();
	// 设置播放速度
	bool SetSpeed(float speed);

	// 对辅助点进行碰撞检测
	int	TraceHelper(const FmVec3& start, const FmVec3& direction);
	// 使用屏幕坐标对辅助点进行碰撞检测
	int TraceHelperInScreen(int screen_x, int screen_y);
	// 获得辅助点数量
	int GetHelperCount();
	// 获得辅助点名称
	const char* GetHelperIDFromIndex(int nIndex);

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
	// 获取指定材质的纹理贴图数
	int GetMaterialTexCount(int index) const;
	// 获取指定材质的的纹理贴图文件
	const char* GetMaterialTexName(int mat_index, int tex_index) const;
	// 获得指定材质的Ambient
	result_string GetMaterialAmbient(size_t mat_index) const;
	// 获得指定材质的Diffuse
	result_string GetMaterialDiffuse(size_t mat_index) const;

	// 获得指定顶点缓冲的指定顶点的颜色数据
	result_string GetVertexDiffuse(size_t vb_index, size_t vertex_index) const;

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

	// 模型当前LOD的级别
	void SetCurLODLevel(unsigned int lod_level);
	unsigned int GetCurLODLevel();
	// 模型总共的LOD级数
	unsigned int GetLODLevels();

	// Warning:This API just for LOD tools use!
	bool LOD(unsigned int lod_levels, float dec_ratio);

	void OnLinkEvent(bool bLink);

	// 获得内存占用
	virtual size_t GetMemoryUsage();

private:
	// 创建模型
	bool Create();

private:
	IModelSystem* m_pModelSystem;
	bool m_bAsyncLoad;
	bool m_bShowBoundBox;
	bool m_bUseVertexColor;
	bool m_bUseLightMap;
	bool m_bVisible;
	bool m_bUnderWater;
	bool m_bTraceEnable;
	bool m_bCullEnable;
	bool m_bWaterReflect;
	bool m_bCastShadow;
	bool m_bReceiveShadow;
	int m_nWriteVelocity;
	unsigned int m_nColor;
	float m_fAlphaValue;
	int m_nExtraInfo;
	core_string m_strModelFile;
	core_string m_strLightFile;
	core_string m_strMaterialFile;
	core_string m_strTexPaths;
	IModelPlayer* m_pModelPlayer;
	FmMat4 m_mtxWorld;

	IRender* m_pRender;

	float   m_offset_seconds;
};

#endif // _MODEL_H
