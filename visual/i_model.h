#ifndef _I_MODEL_H
#define _I_MODEL_H

#include "i_vis_base.h"

class IModelPlayer;

class IModel : public IVisBase
{
public:
	virtual bool Load() = 0;
	
	virtual void Update(float seconds) = 0;
	virtual void Realize() = 0;
	virtual void RealizeReflect() = 0;
	virtual void RealizeShadowMap() = 0;

	// 加载数据是否成功
	virtual bool GetLoaded() = 0;
	// 是否加载完成
	virtual bool GetLoadFinish() = 0; 

	// 加载完成后
	virtual bool AfterLoaded() = 0;

	// 位置
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// 方向
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// 缩放系数
	virtual bool SetScale(float x, float y, float z) = 0; 
	virtual FmVec3 GetScale() const = 0;

	// 全局中心点
	virtual FmVec3 GetCenter() = 0; 
	// 包围盒尺寸
	virtual FmVec3 GetBoxSize() = 0;
	// 模型半径
	virtual float GetRadius() = 0; 

	// 世界转换矩阵
	virtual void SetWorldMatrix(const FmMat4& mat) = 0; 
	virtual void GetWorldMatrix(FmMat4& mat) = 0;

	// 碰撞测试详细信息
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result) = 0;
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst) = 0;
	// 球体范围碰撞
	virtual bool TraceSphere(const FmVec3& center, float radius) = 0; 
	// 拣选
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context) = 0;

	// 是否可见 
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

	// 是否在水下
	virtual void SetUnderWater(bool value) = 0;
	virtual bool GetUnderWater() const = 0;

	// 颜色
	virtual void SetColor(unsigned int value) = 0;
	virtual unsigned int GetColor() const = 0;

	// 获得模型播放器
	virtual IModelPlayer* GetModelPlayer() = 0;

	// 名称（测试用）
	virtual const char* GetName() const = 0;

	// 是否已经加载光照
	virtual bool GetLightLoaded() const = 0;
	// 是否可用光贴图
	virtual bool GetHasLightMap() const = 0;
	// 是否可用法线贴图
	virtual bool GetHasNormalMap() const = 0;
	// 是否可用定点色
	virtual bool GetHasVertexColor() const = 0;

	// 是否异步加载
	virtual void SetAsyncLoad(bool value) = 0;
	virtual bool GetAsyncLoad() const = 0;

	// 是否显示包围盒
	virtual void SetShowBoundBox(bool value) = 0;
	virtual bool GetShowBoundBox() const = 0;

	// 是否使用顶点光照
	virtual void SetUseVertexColor(bool value) = 0;
	virtual bool GetUseVertexColor() const = 0;

	// 是否使用光贴图
	virtual void SetUseLightMap(bool value) = 0;
	virtual bool GetUseLightMap() const = 0;

	// 是否在摄像机焦点位置半透明显示
	//void SetFocusTransparent(bool value) = 0;
	//bool GetFocusTransparent() const = 0;

	// 是否检测碰撞
	virtual void SetTraceEnable(bool value) = 0;
	virtual bool GetTraceEnable() const = 0;

	// 是否可以剪裁
	virtual void SetCullEnable(bool value) = 0;
	virtual bool GetCullEnable() const = 0;

	// 是否需要水面反射
	virtual void SetWaterReflect(bool value) = 0;
	virtual bool GetWaterReflect() const = 0;

	// 写入速度纹理标志
	virtual void SetWriteVelocity(int value) = 0;
	virtual int GetWriteVelocity() const = 0;

	// 记录临时的透明度变量（只供地形系统使用）
	virtual void SetAlphaValue(float value) = 0;
	virtual float GetAlphaValue() const = 0;

	// 不需要投射影子
	virtual void SetCastShadow(bool value) = 0; 
	virtual bool GetCastShadow() const  = 0;

	// 不需要接受投射影子
	virtual void SetReceiveShadow(bool value) = 0;
	virtual bool GetReceiveShadow() const = 0;

	// 模型颜色
	virtual void SetColorString(const char* value) = 0;
	virtual result_string GetColorString() const = 0;

	// 附加信息
	virtual void SetExtraInfo(int value) = 0;
	virtual int GetExtraInfo() const = 0;

	// 模型文件
	virtual void SetModelFile(const char* value) = 0; 
	virtual const char* GetModelFile() const = 0;

	// 光照文件
	virtual void SetLightFile(const char* value) = 0;
	virtual const char* GetLightFile() const = 0;

	// 材质文件
	virtual void SetMaterialFile(const char* value) = 0;
	virtual const char* GetMaterialFile() const = 0;

	// 贴图路径
	virtual void SetTexPaths(const char* value) = 0;
	virtual const char* GetTexPaths() const = 0;

	// 增加焦点半透明材质
	//bool AddFocusTransparent(int mat_index) = 0;
	// 清除所有焦点半透明材质
	//bool ClearFocusTransparent() = 0;

	// 加载光照文件
	virtual bool LoadLight() = 0; 
	// 加载材质文件
	virtual bool LoadMaterial() = 0;
	
	// 碰撞测试获得高度
	virtual float TraceHeight(float x, float z, float high_y, float low_y) = 0; 

	// 获得辅助点组名列表
	virtual void GetHelperGroupList(const IVarList& args, IVarList& result) = 0;
	// 获得辅助点名字列表
	virtual void GetHelperNameList(const IVarList& args, IVarList& result) = 0;
	// 获得辅助点的当前位置
	virtual void GetHelperPosition(const IVarList& args, IVarList& result) = 0;
	// 获得辅助点的当前方向
	virtual void GetHelperAngle(const IVarList& args, IVarList& result) = 0;
	
	// 获得指定点（摄像机）的信息
	virtual void GetCameraInfo(const IVarList& args, IVarList& result) = 0;

	// 是否显示辅助点
	virtual void ShowHelper(bool bShow) = 0;
	virtual bool IsShowHelper() const = 0;

	// 获得节点矩阵
	virtual void GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM) = 0;
	virtual bool GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM) = 0;
	// 根据名字查找节点
	virtual int GetNodeFromName(const char* pszName) = 0;
	// 获得指定节点的名字
	virtual const char* GetHelperName(int helpID) = 0;

	// 当前选择的节点
	virtual void SetSelectHelper(int nHelper) = 0;
	virtual int GetSelectHelper() = 0;
	
	// 播放
	virtual bool Play() = 0;
	// 停止
	virtual bool Stop() = 0;
	// 暂停
	virtual bool Pause() = 0;
	// 恢复播放
	virtual bool Resume() = 0;
	
	// 是否循环
	virtual void SetLoop(bool bLoop) = 0;
	virtual bool GetLoop() = 0;
	// 获得帧数
	virtual int GetFrameCount() = 0;
	// 获得当前播放帧
	virtual int GetCurrentFrame() = 0;
	// 设置当前播放帧
	virtual bool SetCurrentFrame(int nFrame) = 0;
	// 获得当前播放时间
	virtual double GetCurrentTimeFloat() = 0;
	// 设置当前播放时间
	virtual bool SetCurrentTimeFloat(double fTime) = 0;
	// 获得缺省播放速度
	virtual float GetDefaultSpeed() = 0;
	// 设置播放速度
	virtual bool SetSpeed(float speed) = 0;

	// 对辅助点进行碰撞检测
	virtual int	TraceHelper(const FmVec3& start, const FmVec3& direction) = 0;
	// 使用屏幕坐标对辅助点进行碰撞检测
	virtual int TraceHelperInScreen(int screen_x, int screen_y) = 0;
	// 获得辅助点数量
	virtual int GetHelperCount() = 0;
	// 获得辅助点名称
	virtual const char* GetHelperIDFromIndex(int nIndex) = 0;

	// 节点信息
	virtual size_t GetObjectCount() const = 0;
	virtual size_t GetObjectVertexCount(size_t index) const = 0;
	virtual size_t GetObjectTriangleCount(size_t index) const = 0;
	virtual int GetAllVertexCount() const = 0;
	virtual int GetAllTriangleCount() = 0;

	// 获得模型节点个数
	virtual int GetNodeCount() = 0;
	// 获得模型的所有节点名列表
	virtual void GetNodeNameList(const IVarList& args, IVarList& result) = 0;
	// 获得模型节点数据是否可编辑
	virtual bool GetModelNodeAllowModify(const char* key) = 0;
	// 设置模型节点数据
	virtual bool SetNodeValue(const char* node_name, const char* key, const char* val) = 0;
	// 读取模型节点数据
	virtual result_string GetNodeValue(const char* node_name, const char* key) = 0;

	// 获取材质数
	virtual int GetMaterialCount() const = 0;
	// 获取指定材质的纹理贴图数
	virtual int GetMaterialTexCount(int index) const = 0;
	// 获取指定材质的的纹理贴图文件
	virtual const char* GetMaterialTexName(int mat_index, int tex_index) const = 0;
	// 获得指定材质的Ambient
	virtual result_string GetMaterialAmbient(size_t mat_index) const = 0;
	// 获得指定材质的Diffuse
	virtual result_string GetMaterialDiffuse(size_t mat_index) const = 0;

	// 获得指定顶点缓冲的指定顶点的颜色数据
	virtual result_string GetVertexDiffuse(size_t vb_index, size_t vertex_index) const = 0;

	// 获得模型的所有材质名列表
	virtual void GetMaterialNameList(const IVarList& args, IVarList& result) = 0;
	// 设置模型材质数据
	virtual bool SetMaterialValue(const char* mat_name, const char* key, 
		const char* val) = 0;
	// 读取模型材质数据
	virtual result_string GetMaterialValue(const char* mat_name, const char* key) = 0;
	// 重新加载模型材质的所有贴图
	virtual bool ReloadMaterialTextures() = 0;

	// 查找是否存在自定义材质数据
	virtual bool FindCustomMaterial(const char* mat_name) = 0;
	// 设置自定义材质数据
	virtual bool SetCustomMaterialValue(const char* mat_name, const char* key, 
		const char* val) = 0;
	// 读取自定义材质数据
	virtual result_string GetCustomMaterialValue(const char* mat_name, const char* key) = 0;
	// 重新加载自定义材质的指定贴图
	virtual bool ReloadCustomMaterialTexture(const char* mat_name, const char* key) = 0;
	// 重新加载自定义材质的所有贴图
	virtual bool ReloadCustomMaterialTextures() = 0;

	// 模型当前LOD的级别
	virtual void SetCurLODLevel(unsigned int lod_level) = 0;
	virtual unsigned int GetCurLODLevel() = 0;
	// 模型总共的LOD级数
	virtual unsigned int GetLODLevels() = 0;

	// Warning:This API just for LOD tools use!
	virtual bool LOD(unsigned int lod_levels, float dec_ratio) = 0;

	virtual void OnLinkEvent(bool bLink) = 0;

	// 获得内存占用
	virtual size_t GetMemoryUsage() = 0;

};
#endif