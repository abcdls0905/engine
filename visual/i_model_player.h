//--------------------------------------------------------------------
// 文件名:		i_model_player.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年9月1日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_MODEL_PLAYER_H
#define _VISUAL_I_MODEL_PLAYER_H

#include "i_res_base.h"
#include "fm_define.h"
#include "i_model_system.h"


/*
#define DX_VERTEX_BLOCK_NUM 16
#define DX_VERTEX_BLOCK_SIZE 4096
#define DX_VERTEX_BLOCK_SHIFT 12

// 顶点缓冲数据
struct vertex_data_t
{
	size_t nStride; // 单个顶点的数据长度
	size_t nCount; // 顶点总数据
	unsigned char* pBlocks[DX_VERTEX_BLOCK_NUM]; // 分块存放的顶点数据
};

// 创建顶点数据
inline void create_vertex_data(vertex_data_t* pData, size_t stride, 
	size_t count)
{
	Assert(count <= (DX_VERTEX_BLOCK_NUM * DX_VERTEX_BLOCK_SIZE));

	size_t block = 0;
	size_t num = count;

	while (num > 0)
	{
		size_t block_size;

		if (num > DX_VERTEX_BLOCK_SIZE)
		{
			block_size = DX_VERTEX_BLOCK_SIZE;
		}
		else
		{
			block_size = num;
		}

		pData->pBlocks[block] = (unsigned char*)CORE_ALLOC(
			stride * block_size);
		++block;
		num -= block_size;
	}

	pData->nStride = stride;
	pData->nCount = count;
}

// 释放顶点数据
inline void release_vertex_data(vertex_data_t* pData)
{
	Assert(pData->nStride > 0);

	size_t block = 0;
	size_t num = pData->nCount;

	while (num > 0)
	{
		size_t block_size;

		if (num > DX_VERTEX_BLOCK_SIZE)
		{
			block_size = DX_VERTEX_BLOCK_SIZE;
		}
		else
		{
			block_size = num;
		}

		CORE_FREE(pData->pBlocks[block], pData->nStride * block_size);
		pData->pBlocks[block] = NULL;
		++block;
		num -= block_size;
	}

	pData->nCount = 0;
}

// 获得指定索引数据块的顶点数
inline size_t get_vertex_block_size(vertex_data_t* pData, size_t block)
{
	Assert(block < DX_VERTEX_BLOCK_NUM);

	size_t start = block << DX_VERTEX_BLOCK_SHIFT;

	if (start < pData->nCount)
	{
		size_t size = pData->nCount - start;

		if (size > DX_VERTEX_BLOCK_SIZE)
		{
			return DX_VERTEX_BLOCK_SIZE;
		}
		else
		{
			return size;
		}
	}
	else
	{
		return 0;
	}
}

// 获得指定索引的顶点数据
inline unsigned char* get_vertex_by_index(vertex_data_t* pData, size_t index)
{
	Assert(index < pData->nCount);

	size_t block = index >> DX_VERTEX_BLOCK_SHIFT;
	Assert(block < DX_VERTEX_BLOCK_NUM);
	size_t order = index & (DX_VERTEX_BLOCK_SIZE - 1);

	return pData->pBlocks[block] + order * pData->nStride;
}
*/
// 顶点缓冲数据
struct vertex_data_t
{
#ifdef CPU_COMPUTE_BONE
	vertex_data_t()
		:pDynamicVertices(0)
	{}
#endif
	unsigned int nStride; // 单个顶点的数据长度
	size_t nCount; // 顶点总数据
	unsigned char* pVertices;
#ifdef CPU_COMPUTE_BONE
	unsigned char* pDynamicVertices;
#endif
};

// 创建顶点数据
inline void create_vertex_data(vertex_data_t* pData, unsigned int stride, 
	size_t count)
{
	pData->pVertices = (unsigned char*)CORE_ALLOC(stride * count);
	pData->nStride = stride;
	pData->nCount = count;
}

// 释放顶点数据
inline void release_vertex_data(vertex_data_t* pData)
{
	Assert(pData->pVertices != NULL);
	
	CORE_FREE(pData->pVertices, pData->nStride * pData->nCount);
	pData->pVertices = NULL;
	pData->nStride = 0;
	pData->nCount = 0;
}

// 获得指定索引的顶点数据
inline unsigned char* get_vertex_by_index(const vertex_data_t* pData, 
	size_t index)
{
	Assert(index < pData->nCount);

	return pData->pVertices + pData->nStride * index;
}

// 模型播放器接口

class IRenderContext;
class IActionPlayer;
struct skt_pose_t;

class IModelPlayer: public IResBase
{
public:
	// 是否就绪
	virtual bool IsReady() = 0;
	// 是否加载完毕（成功或失败）
	virtual bool IsLoadComplete() = 0;
	//是否创建成功
	virtual bool IsCreate() = 0;

	// 设置场景上下文
	virtual void SetContext(IRenderContext* value) = 0;

	// 渲染
	virtual bool Draw(const FmPlane* planes, size_t plane_num) = 0;

	virtual bool DrawReflect( const FmPlane* planes, size_t plane_num) = 0;
	// 渲染阴影贴图
	virtual bool DrawShadowMap(const FmPlane* planes, size_t plane_num) = 0;
	
	// 碰撞检测
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result) = 0;
	// 碰撞检测
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst) = 0;
	// 球体范围碰撞
	virtual bool TraceSphere(const FmVec3& center, float radius) = 0;
    // 三角形碰撞
    virtual bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2) = 0;
	// 拣选
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context, const FmMat4& mat) = 0;
	
	// 获得是否可以渲染光照
	virtual bool GetLightingEnable() = 0;
	// 是否可用光贴图
	virtual bool GetLightMapEnable() = 0;
	// 是否可用法线贴图
	virtual bool GetNormalMapEnable() = 0;
	// 是否可用顶点色
	virtual bool GetVertexColorEnable() = 0;
	// 获得包围盒尺寸
	virtual FmVec3 GetBoxSize() = 0;
	
	// 获得标记点数量
	virtual size_t GetLabelCount() = 0;
	// 获得标记点组名
	virtual const char* GetLabelGroup(size_t index) = 0;
	// 获得标记点名称
	virtual const char* GetLabelName(size_t index) = 0;
	// 查找标记点组和名称
	virtual int FindLabelIndex(const char* group, const char* name) = 0;
	// 查找标记点名称
	virtual int FindLabelNameIndex(const char* name) = 0;
	// 获得标记点矩阵
	virtual bool GetLabelMatrix(size_t index, FmMat4& mat) = 0;
	// 获得标记点绑定的骨骼
	virtual int GetLabelBoneId(size_t index) = 0;
	
	// 获得模型节点数量
	virtual size_t GetObjectCount() = 0;
	// 获得指定节点的顶点数量
	virtual size_t GetObjectVertexCount(size_t index) = 0;
	// 获得指定节点的顶点起始位置
	virtual size_t GetObjectStartVertex(size_t index) = 0;
	// 获得指定节点的面数量
	virtual size_t GetObjectTriangleCount(size_t index) = 0;
	// 获得指定节点的顶点缓冲索引
	virtual size_t GetObjectVerterBufferIndex(size_t index) = 0;
	// 获得指定节点的材质索引
	virtual size_t GetObjectMaterial(size_t index) = 0;
	// 获得指定节点的索引缓冲数据
	virtual unsigned short* GetObjectIndexData(size_t index) = 0;
	// 获得指定节点的当前世界矩阵
	virtual bool GetObjectMatrix(size_t index, FmMat4& mat) = 0;

	// 获得材质数量
	virtual size_t GetMaterialCount() = 0;
	// 获得指定材质的贴图层数
	virtual size_t GetMaterialTextureCount(size_t mat_index) = 0;
	// 获得指定材质是否可用光贴图
	virtual bool GetMaterialLightmapEnable(size_t mat_index) = 0;
	// 获得指定材质的贴图名
	virtual const char* GetMaterialTextureName(size_t mat_index, 
		size_t tex_index) = 0;
	// 获得指定材质的光贴图名
	virtual const char* GetMaterialLightmapName(size_t mat_index) = 0;
	// 获得指定材质的Ambient
	virtual unsigned int GetMaterialAmbient(size_t mat_index) = 0;
	// 获得指定材质的Diffuse
	virtual unsigned int GetMaterialDiffuse(size_t mat_index) = 0;
	// 设置材质使用合并后的光贴图
	virtual bool SetMaterialAtlasLightMap(size_t mat_index, 
		const char* atlas_light_map, float scale_u, float scale_v,
		float offset_u, float offset_v) = 0;
	// 设置指定材质光照贴图颜色范围的缩放
	virtual bool SetMaterialLigthMapColorRangeScale(size_t mat_index, float x, float y, float z) = 0;
	// 获取指定材质光照贴图颜色范围的缩放
	virtual const FmVec3& GetMaterialLigthMapColorRangeScale(size_t mat_index) = 0;
	
	// 获得顶点缓冲数量
	virtual size_t GetVertexBufferCount() = 0;
	// 获得指定顶点缓冲的大小
	virtual size_t GetVertexBufferSize(size_t index) = 0;
	// 获得指定顶点缓冲的顶点数据大小
	virtual size_t GetVertexDataStride(size_t index) = 0;
	// 获得指定顶点缓冲的顶点大小
	virtual size_t GetVertexStride(size_t index) = 0;
	// 获得指定顶点缓冲的顶点数量
	virtual size_t GetVertexCount(size_t index) = 0;
	// 获得指定顶点缓冲的位置数据偏移
	virtual int GetVertexPositionOffset(size_t index) = 0;
	// 获得指定顶点缓冲的指定顶点的顶点数据(如果有骨骼，参于骨骼计算)
	virtual bool GetVertexPosition(size_t index, size_t vertex_index, FmVec3& pos, bool checkbone = true) = 0;
	// 获得指定顶点缓冲的法线数据偏移
	virtual int GetVertexNormalOffset(size_t index) = 0;
	// 获得指定顶点缓冲的颜色数据偏移
	virtual int GetVertexDiffuseOffset(size_t index) = 0;
	// 获得指定顶点缓冲的指定顶点的颜色数据
	virtual unsigned int GetVertexDiffuse(size_t vb_index, 
		size_t vertex_index) = 0;
	// 获得指定顶点缓冲的贴图的UV数据偏移
	virtual int GetVertexUVOffset(size_t index) = 0;
	// 获得指定顶点缓冲的光贴图UV数据偏移
	virtual int GetVertexLightmapUVOffset(size_t index) = 0;
	// 获得指定顶点缓冲的顶点数据
	virtual vertex_data_t* GetVertexData(size_t index) = 0;
	// 更新指定顶点缓冲的顶点数据
	virtual bool UpdateVertexData(size_t index, vertex_data_t* data) = 0;
	
	// 动画播放器
	virtual void SetActionPlayer(IActionPlayer* value) = 0;
	virtual IActionPlayer* GetActionPlayer() = 0;

	// 世界矩阵
	virtual void SetWorldMatrix(const FmMat4& mat) = 0;
	virtual FmMat4 GetWorldMatrix() = 0;

	// 是否显示包围盒
	virtual void SetShowBoundBox(bool value) = 0;
	virtual bool GetShowBoundBox() = 0;

	// 是否使用顶点色
	virtual void SetUseVertexColor(bool value) = 0;
	virtual bool GetUseVertexColor() = 0;

	// 是否使用光贴图
	virtual void SetUseLightMap(bool value) = 0;
	virtual bool GetUseLightMap() = 0;

	// 写入速度纹理标志
	virtual void SetWriteVelocity(int value) = 0;
	virtual int GetWriteVelocity() = 0;

	// 需要投射影子
	virtual void SetCastShadow(bool value) = 0;
	virtual bool GetCastShadow() const = 0;

	// 需要接受投射影子
	virtual void SetReceiveShadow(bool value) = 0;
	virtual bool GetReceiveShadow() const  = 0;

	// 光照信息是否成功加载
	virtual bool GetLightLoaded() = 0;

	// 更新顶点色
	virtual bool UpdateVertexColor(unsigned int* color_data, 
		size_t color_size) = 0;
	// 更新光贴图
	virtual bool UpdateLightMap(size_t material, const char* light_file, 
		unsigned int* tex_data, size_t tex_width, size_t tex_height) = 0;
	// 加载光照文件
	virtual bool LoadLight(const char* light_file, bool async_load) = 0;
	// 加载材质文件
	virtual bool LoadMaterial(const char* material_file, bool async_load) = 0;

	// 设置指定模型的附加颜色
	virtual	void SetColor(unsigned int cr) = 0;
	// 获取指定模型的附加颜色
	virtual	unsigned int GetColor() = 0;
	// 设置指定模型是否在水下
	virtual	void SetUnderWater(bool value) = 0;
	// 获取指定模型是否在水下
	virtual	bool GetUnderWater() = 0;
	// 获取指定模型的帧数，这个数字就是美术在Max制作的模型动画帧数
	virtual unsigned int GetFrameCount() = 0;
	// 获取指定模型的原始FPS，这个FPS就是美术在Max制作的模型动画的FPS
	virtual float GetInternalFPS() = 0;
	// 设置模型的当前速度
	virtual void SetSpeed(float fSpeed) = 0;
	// 获取模型的当前速度
	virtual float GetSpeed() = 0;
	// 设置是否进行模型动画的自动循环播放
	virtual void SetLoop(bool bLoop) = 0;
	// 判断是否进行模型动画的自动循环播放
	virtual bool GetLoop() = 0;
	// 启动模型动画播放
	virtual void Play() = 0;
	// 停止模型动画播放
	virtual void Stop() = 0;
	// 暂停模型动画播放
	virtual void Pause() = 0;
	// 恢复模型动画播放
	virtual void Resume() = 0;
	// 设置模型动画的当前播放时间
	virtual void SetCurrentTimeFloat(double time) = 0;
	// 获取模型动画的当前播放时间
	virtual double GetCurrentTimeFloat() = 0;
	// 设置模型动画的当前帧
	virtual void SetCurrentFrame(unsigned int nFrame) = 0;
	// 获取模型动画的当前帧
	virtual unsigned int GetCurrentFrame() = 0;
	// 对模型进行辅助点碰撞，这个函数可以有效的选择模型上的辅助点
	virtual void* RayTraceHelper(const FmVec3* start, 
		const FmVec3* direction) = 0;

	// 指定是否隐藏当前指定的模型
	virtual void SetHide(bool bHide) = 0;
	// 获取当前指定的模型是否被隐藏
	virtual bool GetHide() = 0;

	// 是否可活动物体
	virtual void SetLive(bool value) = 0;
	virtual bool GetLive() = 0;

	// 指定是否当前模型为选择态，在选择态的模型将显示其包围盒
	virtual void SetSelection(bool bSelected) = 0;
	// 获取是否当前模型处于选择态
	virtual bool GetSelection() = 0;

	// 指定当前模型使用网格模式进行渲染
	//virtual void SetWireframe(bool bWireframe) = 0;
	// 获取当前模型是否使用网格模式进行渲染
	//virtual bool GetWireframe() = 0;
	// 显示当前模型中包含的辅助点
	virtual void SetShowHelper(bool bShow) = 0;
	// 判断当前模型中包含的辅助点是否被显示
	virtual bool GetShowHelper() = 0;
	// 获取当前模型中的根节点个数
	virtual unsigned int GetRootNodeCount() = 0;
	// 根据当前模型中的根节点索引号来获取结点上下文
	virtual void* GetRootNodeFromID(unsigned int index) = 0;
	// 获取当前模型中的结点的个数
	virtual unsigned int GetNodeCount() = 0;
	// 根据当前模型中的结点索引号来获取结点上下文
	virtual void* GetNodeFromID(int iNodeID) = 0;
	// 根据当前模型中的结点名称来获取结点上下文
	virtual void* GetNodeFromName(const char* pszName) = 0;
	// 根据当前模型中的结点上下文获取结点的索引号
	virtual int GetNodeID(void* node) = 0;
	// 根据当前模型中的结点上下文获取结点的名称
	virtual const char* GetNodeName(void* node) = 0;
	// 根据当前模型中的结点上下文来获取结点的类型
	virtual FXNODE_TYPE GetNodeType(void* node) = 0;
	// 根据当前模型中的结点索引号来获取摄像机信息
	virtual float GetCameraNodeFov(void* node) = 0;
	virtual float GetCameraNodeFarClip(void* node) = 0;
	virtual float GetCameraNodeNearClip(void* node) = 0;
	// 是否附属节点
	virtual bool IsAttachedNode(void* node) = 0;
	// 获得附属节点的父节点名
	virtual const char* GetAttachedParentName(void* node) = 0;
	// 获取当前模型中的结点的当前变换矩阵，这个变换矩阵是相对于当前模型坐标系的
	virtual void GetCurrentNodeTM(void* node, FmMat4* mtxTM) = 0;
	// 获取当前模型的变换矩阵
	virtual void GetCurrentTM(FmMat4* mtxTM) = 0;

	// 获取当前模型是否存在主模（裸模）
	virtual	bool ExistMainModel() = 0;
	// 指定当前模型仅对主模（裸模）进行碰撞检测
	virtual	void SetTraceMainModel(bool value) = 0;
	// 获取当前模型是否仅对主模（裸模）进行碰撞检测
	virtual	bool GetTraceMainModel() = 0;

	// 指定是否显示当前指定的模型结点
	virtual void SetShowNode(void* node, bool bShow) = 0;
	// 获取当前指定的结点是否被显示
	virtual bool GetShowNode(void* node) = 0;
	// 获取当前模型的材质个数
	//virtual unsigned int GetMaterialCount();
	// 根据当前模型的材质索引获取材质上下文
	virtual	void* GetMaterialFromID(int iMatIndex) = 0;
	// 根据当前模型的材质名称获取材质上下文
	virtual	void* GetMaterialFromName(const char* pszMatName) = 0;
	// 根据材质上下文来获取材质索引
	//virtual	int GetMaterialID(void* mat) = 0;
	// 获取材质上下文获取材质名称
	virtual	const char* GetMaterialName(void* mat) = 0;
	// 获取指定的材质的文理个数
	virtual	unsigned int GetMaterialTextureCount(void* mat) = 0;
	// 获取指定的材质的纹理贴图
	virtual const char* GetMaterialTextureName(void* mat, unsigned int index) = 0;
	// 获取指定材质的顶点缓冲
	virtual	void* GetMaterialVertexBuffer(void* mat) = 0;
	// 获取指定材质的索引缓冲
	virtual	void* GetMaterialIndexBuffer(void* mat) = 0;
	// 获取指定材质的顶点大小
	virtual	unsigned int GetMaterialVertexSize(void* mat) = 0;
	// 获取指定材质的顶点个数
	virtual	unsigned int GetMaterialVertexCount(void* mat) = 0;
	// 获取指定材质的索引大小，一般为sizeof(unsigned short)
	virtual	unsigned int GetMaterialIndexSize(void* mat) = 0;
	// 获取指定材质的索引个数
	virtual unsigned int GetMaterialIndexCount(void* mat) = 0;
	// 获取指定材质的顶点位置信息在一个顶点中偏移了多少个字节
	virtual int GetPositionOffset(void* mat) = 0;
	// 获取指定材质的顶点法线信息在一个顶点中偏移了多少个字节
	virtual int GetNormalOffset(void* mat) = 0;
	// 获取指定材质的顶点颜色信息在一个顶点中偏移了多少个字节
	virtual int GetDiffuseOffset(void* mat) = 0;
	// 获取指定材质的顶点Lightmap纹理坐标信息在一个顶点中偏移了多少个字节
	virtual int GetLightmapUVOffset(void* mat) = 0;
	// 获取指定的Diffuse纹理文件名
	virtual	const char* GetTexDiffuseFileName(void* mat) = 0;
	// 获取指定的Bump纹理文件名
	virtual	const char* GetTexBumpFileName(void* mat) = 0;
	// 获取指定的Specularmap纹理文件名
	virtual	const char* GetTexSpecularmapFileName(void* mat) = 0;
	// 获取指定的Specularlevelmap纹理文件名
	virtual	const char* GetTexSpecularlevelmapFileName(void* mat) = 0;
	// 获取指定的Glossinessmap纹理文件名
	virtual	const char* GetTexGlossinessmapFileName(void* mat) = 0;
	// 获取指定的Reflectionmap纹理文件名
	virtual	const char* GetTexReflectionmapFileName(void* mat) = 0;
	// 获取指定的Lightmap纹理文件名
	virtual	const char* GetTexLightmapFileName(void* mat) = 0;
	// 获得指定材质的Ambient
	virtual	unsigned int GetMaterialAmbient(void* mat) = 0;
	// 获得指定材质的Diffuse
	virtual	unsigned int GetMaterialDiffuse(void* mat) = 0;

	// 查找自定义的材质
	virtual bool FindCustomMaterial(const char* mat_name) = 0;
	// 设置自定义材质数据
	virtual bool SetCustomMaterialValue(const char* mat_name,
		const char* key, const char* val) = 0;
	// 读取自定义材质数据
	virtual bool GetCustomMaterialValue(const char* mat_name,
		const char* key, char* val, int val_size) = 0;
	// 重新加载自定义材质的指定贴图
	virtual bool ReloadCustomMaterialTexture(const char* mat_name, 
		const char* key, const char* tex_paths, bool async) = 0;
	// 重新加载自定义材质的所有贴图
	virtual bool ReloadCustomMaterialTextures(const char* tex_paths, 
		bool async) = 0;

	// 设置模型材质数据
	virtual bool SetModelMaterialValue(const char* mat_name, 
		const char* key, const char* val) = 0;
	// 读取模型材质数据
	virtual bool GetModelMaterialValue(const char* mat_name,
		const char* key, char* val, int val_size) = 0;
	// 重新加载模型的所有贴图
	virtual bool ReloadMaterialTextures(const char* tex_paths, 
		bool async) = 0;

	// 获得模型节点数据是否可编辑
	virtual bool GetModelNodeAllowModify(const char* key) = 0;
	// 设置模型节点数据
	virtual bool SetModelNodeValue(const char* node_name, 
		const char* key, const char* val) = 0;
	// 读取模型节点数据
	virtual bool GetModelNodeValue(const char* node_name,
		const char* key, char* val, int val_size) = 0;

	// 获取指定模型的资源文件名字
	virtual const char* GetModelFileName() = 0;
	// 获取指定模型的存储器使用情况描述
	//virtual void GetMemoryStatus(LPFXMEMORY_STATUS status) = 0;
	// 将当前模型与指定动作集进行蒙皮映射
	virtual bool InitSkeletonMappingTable() = 0;
	// 判断当前模型是否与指定动作集进行了蒙皮映射
	virtual bool IsInitSkeletonMappingTable() = 0;
	// 释放当前模型的蒙皮影射
	virtual void ReleaseSkeletonMappingTable() = 0;
	// 设置当前模型的指定辅助点为选择态
	virtual void SetSelectHelper(int nHelper) = 0;
	// 获取当前模型中处于选择态的辅助点
	virtual int GetSelectHelper() = 0;
	// 获取当前模型中包含的辅助点个数
	virtual unsigned int GetHelperCount() = 0;
	// 根据当前模型的辅助点索引获取辅助点的名字
	virtual const char* GetHelperIDFromIndex(unsigned int nIndex) = 0;
	// 指定是否开启当前指定模型的FallOff(轮廓光)效果
	virtual void SetEnableFallOff(bool value) = 0;
	// 判断当前指定的模型是否开启了FallOff(轮廓光)效果
	virtual bool GetEnableFallOff() = 0;
	// 指定是否开启当前指定模型的点光源光照效果
	virtual void SetEnablePointLighting(bool value) = 0;
	// 判断当前指定的模型是否开启了点光源光照效果
	virtual bool GetEnablePointLighting() = 0;
	// 模型当前LOD的级别
	virtual void SetCurLODLevel(unsigned int lod_level) = 0;
	virtual unsigned int GetCurLODLevel() = 0;
	// 模型总共的LOD级数
	virtual unsigned int GetLODLevels() = 0;
	//取得包维盒
	virtual void GetNodeLocationBoundingBox(FmVec3& bbDstMin, 
		FmVec3& bbDstMax, FmVec3& bbDstCenter) = 0;
	// 执行播放
	virtual void Update(float seconds) = 0;

	// 性能优化
	const FmVec3& GetCenter() const { return m_vCenter; }
	float GetRadius() const { return m_fRadius; }

	//设置用户传入Sahder的常量
	virtual void SetUserVsConstValue(const char* name, const FmVec4& value) = 0;

	virtual void EnableKeepOut(bool keepout) = 0;
	
	virtual bool IsEnableKeepOut() = 0;

	virtual void SetKeepOutTexture(const char* filename) = 0;
protected:
	// 为了优化
	FmVec3 m_vCenter;
	float m_fRadius;
};

#endif // _VISUAL_I_MODEL_PLAYER_H
