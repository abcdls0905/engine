//--------------------------------------------------------------------
// �ļ���:		i_model_player.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��9��1��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// ���㻺������
struct vertex_data_t
{
	size_t nStride; // ������������ݳ���
	size_t nCount; // ����������
	unsigned char* pBlocks[DX_VERTEX_BLOCK_NUM]; // �ֿ��ŵĶ�������
};

// ������������
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

// �ͷŶ�������
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

// ���ָ���������ݿ�Ķ�����
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

// ���ָ�������Ķ�������
inline unsigned char* get_vertex_by_index(vertex_data_t* pData, size_t index)
{
	Assert(index < pData->nCount);

	size_t block = index >> DX_VERTEX_BLOCK_SHIFT;
	Assert(block < DX_VERTEX_BLOCK_NUM);
	size_t order = index & (DX_VERTEX_BLOCK_SIZE - 1);

	return pData->pBlocks[block] + order * pData->nStride;
}
*/
// ���㻺������
struct vertex_data_t
{
#ifdef CPU_COMPUTE_BONE
	vertex_data_t()
		:pDynamicVertices(0)
	{}
#endif
	unsigned int nStride; // ������������ݳ���
	size_t nCount; // ����������
	unsigned char* pVertices;
#ifdef CPU_COMPUTE_BONE
	unsigned char* pDynamicVertices;
#endif
};

// ������������
inline void create_vertex_data(vertex_data_t* pData, unsigned int stride, 
	size_t count)
{
	pData->pVertices = (unsigned char*)CORE_ALLOC(stride * count);
	pData->nStride = stride;
	pData->nCount = count;
}

// �ͷŶ�������
inline void release_vertex_data(vertex_data_t* pData)
{
	Assert(pData->pVertices != NULL);
	
	CORE_FREE(pData->pVertices, pData->nStride * pData->nCount);
	pData->pVertices = NULL;
	pData->nStride = 0;
	pData->nCount = 0;
}

// ���ָ�������Ķ�������
inline unsigned char* get_vertex_by_index(const vertex_data_t* pData, 
	size_t index)
{
	Assert(index < pData->nCount);

	return pData->pVertices + pData->nStride * index;
}

// ģ�Ͳ������ӿ�

class IRenderContext;
class IActionPlayer;
struct skt_pose_t;

class IModelPlayer: public IResBase
{
public:
	// �Ƿ����
	virtual bool IsReady() = 0;
	// �Ƿ������ϣ��ɹ���ʧ�ܣ�
	virtual bool IsLoadComplete() = 0;
	//�Ƿ񴴽��ɹ�
	virtual bool IsCreate() = 0;

	// ���ó���������
	virtual void SetContext(IRenderContext* value) = 0;

	// ��Ⱦ
	virtual bool Draw(const FmPlane* planes, size_t plane_num) = 0;

	virtual bool DrawReflect( const FmPlane* planes, size_t plane_num) = 0;
	// ��Ⱦ��Ӱ��ͼ
	virtual bool DrawShadowMap(const FmPlane* planes, size_t plane_num) = 0;
	
	// ��ײ���
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result) = 0;
	// ��ײ���
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst) = 0;
	// ���巶Χ��ײ
	virtual bool TraceSphere(const FmVec3& center, float radius) = 0;
    // ��������ײ
    virtual bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2) = 0;
	// ��ѡ
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context, const FmMat4& mat) = 0;
	
	// ����Ƿ������Ⱦ����
	virtual bool GetLightingEnable() = 0;
	// �Ƿ���ù���ͼ
	virtual bool GetLightMapEnable() = 0;
	// �Ƿ���÷�����ͼ
	virtual bool GetNormalMapEnable() = 0;
	// �Ƿ���ö���ɫ
	virtual bool GetVertexColorEnable() = 0;
	// ��ð�Χ�гߴ�
	virtual FmVec3 GetBoxSize() = 0;
	
	// ��ñ�ǵ�����
	virtual size_t GetLabelCount() = 0;
	// ��ñ�ǵ�����
	virtual const char* GetLabelGroup(size_t index) = 0;
	// ��ñ�ǵ�����
	virtual const char* GetLabelName(size_t index) = 0;
	// ���ұ�ǵ��������
	virtual int FindLabelIndex(const char* group, const char* name) = 0;
	// ���ұ�ǵ�����
	virtual int FindLabelNameIndex(const char* name) = 0;
	// ��ñ�ǵ����
	virtual bool GetLabelMatrix(size_t index, FmMat4& mat) = 0;
	// ��ñ�ǵ�󶨵Ĺ���
	virtual int GetLabelBoneId(size_t index) = 0;
	
	// ���ģ�ͽڵ�����
	virtual size_t GetObjectCount() = 0;
	// ���ָ���ڵ�Ķ�������
	virtual size_t GetObjectVertexCount(size_t index) = 0;
	// ���ָ���ڵ�Ķ�����ʼλ��
	virtual size_t GetObjectStartVertex(size_t index) = 0;
	// ���ָ���ڵ��������
	virtual size_t GetObjectTriangleCount(size_t index) = 0;
	// ���ָ���ڵ�Ķ��㻺������
	virtual size_t GetObjectVerterBufferIndex(size_t index) = 0;
	// ���ָ���ڵ�Ĳ�������
	virtual size_t GetObjectMaterial(size_t index) = 0;
	// ���ָ���ڵ��������������
	virtual unsigned short* GetObjectIndexData(size_t index) = 0;
	// ���ָ���ڵ�ĵ�ǰ�������
	virtual bool GetObjectMatrix(size_t index, FmMat4& mat) = 0;

	// ��ò�������
	virtual size_t GetMaterialCount() = 0;
	// ���ָ�����ʵ���ͼ����
	virtual size_t GetMaterialTextureCount(size_t mat_index) = 0;
	// ���ָ�������Ƿ���ù���ͼ
	virtual bool GetMaterialLightmapEnable(size_t mat_index) = 0;
	// ���ָ�����ʵ���ͼ��
	virtual const char* GetMaterialTextureName(size_t mat_index, 
		size_t tex_index) = 0;
	// ���ָ�����ʵĹ���ͼ��
	virtual const char* GetMaterialLightmapName(size_t mat_index) = 0;
	// ���ָ�����ʵ�Ambient
	virtual unsigned int GetMaterialAmbient(size_t mat_index) = 0;
	// ���ָ�����ʵ�Diffuse
	virtual unsigned int GetMaterialDiffuse(size_t mat_index) = 0;
	// ���ò���ʹ�úϲ���Ĺ���ͼ
	virtual bool SetMaterialAtlasLightMap(size_t mat_index, 
		const char* atlas_light_map, float scale_u, float scale_v,
		float offset_u, float offset_v) = 0;
	// ����ָ�����ʹ�����ͼ��ɫ��Χ������
	virtual bool SetMaterialLigthMapColorRangeScale(size_t mat_index, float x, float y, float z) = 0;
	// ��ȡָ�����ʹ�����ͼ��ɫ��Χ������
	virtual const FmVec3& GetMaterialLigthMapColorRangeScale(size_t mat_index) = 0;
	
	// ��ö��㻺������
	virtual size_t GetVertexBufferCount() = 0;
	// ���ָ�����㻺��Ĵ�С
	virtual size_t GetVertexBufferSize(size_t index) = 0;
	// ���ָ�����㻺��Ķ������ݴ�С
	virtual size_t GetVertexDataStride(size_t index) = 0;
	// ���ָ�����㻺��Ķ����С
	virtual size_t GetVertexStride(size_t index) = 0;
	// ���ָ�����㻺��Ķ�������
	virtual size_t GetVertexCount(size_t index) = 0;
	// ���ָ�����㻺���λ������ƫ��
	virtual int GetVertexPositionOffset(size_t index) = 0;
	// ���ָ�����㻺���ָ������Ķ�������(����й��������ڹ�������)
	virtual bool GetVertexPosition(size_t index, size_t vertex_index, FmVec3& pos, bool checkbone = true) = 0;
	// ���ָ�����㻺��ķ�������ƫ��
	virtual int GetVertexNormalOffset(size_t index) = 0;
	// ���ָ�����㻺�����ɫ����ƫ��
	virtual int GetVertexDiffuseOffset(size_t index) = 0;
	// ���ָ�����㻺���ָ���������ɫ����
	virtual unsigned int GetVertexDiffuse(size_t vb_index, 
		size_t vertex_index) = 0;
	// ���ָ�����㻺�����ͼ��UV����ƫ��
	virtual int GetVertexUVOffset(size_t index) = 0;
	// ���ָ�����㻺��Ĺ���ͼUV����ƫ��
	virtual int GetVertexLightmapUVOffset(size_t index) = 0;
	// ���ָ�����㻺��Ķ�������
	virtual vertex_data_t* GetVertexData(size_t index) = 0;
	// ����ָ�����㻺��Ķ�������
	virtual bool UpdateVertexData(size_t index, vertex_data_t* data) = 0;
	
	// ����������
	virtual void SetActionPlayer(IActionPlayer* value) = 0;
	virtual IActionPlayer* GetActionPlayer() = 0;

	// �������
	virtual void SetWorldMatrix(const FmMat4& mat) = 0;
	virtual FmMat4 GetWorldMatrix() = 0;

	// �Ƿ���ʾ��Χ��
	virtual void SetShowBoundBox(bool value) = 0;
	virtual bool GetShowBoundBox() = 0;

	// �Ƿ�ʹ�ö���ɫ
	virtual void SetUseVertexColor(bool value) = 0;
	virtual bool GetUseVertexColor() = 0;

	// �Ƿ�ʹ�ù���ͼ
	virtual void SetUseLightMap(bool value) = 0;
	virtual bool GetUseLightMap() = 0;

	// д���ٶ������־
	virtual void SetWriteVelocity(int value) = 0;
	virtual int GetWriteVelocity() = 0;

	// ��ҪͶ��Ӱ��
	virtual void SetCastShadow(bool value) = 0;
	virtual bool GetCastShadow() const = 0;

	// ��Ҫ����Ͷ��Ӱ��
	virtual void SetReceiveShadow(bool value) = 0;
	virtual bool GetReceiveShadow() const  = 0;

	// ������Ϣ�Ƿ�ɹ�����
	virtual bool GetLightLoaded() = 0;

	// ���¶���ɫ
	virtual bool UpdateVertexColor(unsigned int* color_data, 
		size_t color_size) = 0;
	// ���¹���ͼ
	virtual bool UpdateLightMap(size_t material, const char* light_file, 
		unsigned int* tex_data, size_t tex_width, size_t tex_height) = 0;
	// ���ع����ļ�
	virtual bool LoadLight(const char* light_file, bool async_load) = 0;
	// ���ز����ļ�
	virtual bool LoadMaterial(const char* material_file, bool async_load) = 0;

	// ����ָ��ģ�͵ĸ�����ɫ
	virtual	void SetColor(unsigned int cr) = 0;
	// ��ȡָ��ģ�͵ĸ�����ɫ
	virtual	unsigned int GetColor() = 0;
	// ����ָ��ģ���Ƿ���ˮ��
	virtual	void SetUnderWater(bool value) = 0;
	// ��ȡָ��ģ���Ƿ���ˮ��
	virtual	bool GetUnderWater() = 0;
	// ��ȡָ��ģ�͵�֡����������־���������Max������ģ�Ͷ���֡��
	virtual unsigned int GetFrameCount() = 0;
	// ��ȡָ��ģ�͵�ԭʼFPS�����FPS����������Max������ģ�Ͷ�����FPS
	virtual float GetInternalFPS() = 0;
	// ����ģ�͵ĵ�ǰ�ٶ�
	virtual void SetSpeed(float fSpeed) = 0;
	// ��ȡģ�͵ĵ�ǰ�ٶ�
	virtual float GetSpeed() = 0;
	// �����Ƿ����ģ�Ͷ������Զ�ѭ������
	virtual void SetLoop(bool bLoop) = 0;
	// �ж��Ƿ����ģ�Ͷ������Զ�ѭ������
	virtual bool GetLoop() = 0;
	// ����ģ�Ͷ�������
	virtual void Play() = 0;
	// ֹͣģ�Ͷ�������
	virtual void Stop() = 0;
	// ��ͣģ�Ͷ�������
	virtual void Pause() = 0;
	// �ָ�ģ�Ͷ�������
	virtual void Resume() = 0;
	// ����ģ�Ͷ����ĵ�ǰ����ʱ��
	virtual void SetCurrentTimeFloat(double time) = 0;
	// ��ȡģ�Ͷ����ĵ�ǰ����ʱ��
	virtual double GetCurrentTimeFloat() = 0;
	// ����ģ�Ͷ����ĵ�ǰ֡
	virtual void SetCurrentFrame(unsigned int nFrame) = 0;
	// ��ȡģ�Ͷ����ĵ�ǰ֡
	virtual unsigned int GetCurrentFrame() = 0;
	// ��ģ�ͽ��и�������ײ���������������Ч��ѡ��ģ���ϵĸ�����
	virtual void* RayTraceHelper(const FmVec3* start, 
		const FmVec3* direction) = 0;

	// ָ���Ƿ����ص�ǰָ����ģ��
	virtual void SetHide(bool bHide) = 0;
	// ��ȡ��ǰָ����ģ���Ƿ�����
	virtual bool GetHide() = 0;

	// �Ƿ�ɻ����
	virtual void SetLive(bool value) = 0;
	virtual bool GetLive() = 0;

	// ָ���Ƿ�ǰģ��Ϊѡ��̬����ѡ��̬��ģ�ͽ���ʾ���Χ��
	virtual void SetSelection(bool bSelected) = 0;
	// ��ȡ�Ƿ�ǰģ�ʹ���ѡ��̬
	virtual bool GetSelection() = 0;

	// ָ����ǰģ��ʹ������ģʽ������Ⱦ
	//virtual void SetWireframe(bool bWireframe) = 0;
	// ��ȡ��ǰģ���Ƿ�ʹ������ģʽ������Ⱦ
	//virtual bool GetWireframe() = 0;
	// ��ʾ��ǰģ���а����ĸ�����
	virtual void SetShowHelper(bool bShow) = 0;
	// �жϵ�ǰģ���а����ĸ������Ƿ���ʾ
	virtual bool GetShowHelper() = 0;
	// ��ȡ��ǰģ���еĸ��ڵ����
	virtual unsigned int GetRootNodeCount() = 0;
	// ���ݵ�ǰģ���еĸ��ڵ�����������ȡ���������
	virtual void* GetRootNodeFromID(unsigned int index) = 0;
	// ��ȡ��ǰģ���еĽ��ĸ���
	virtual unsigned int GetNodeCount() = 0;
	// ���ݵ�ǰģ���еĽ������������ȡ���������
	virtual void* GetNodeFromID(int iNodeID) = 0;
	// ���ݵ�ǰģ���еĽ����������ȡ���������
	virtual void* GetNodeFromName(const char* pszName) = 0;
	// ���ݵ�ǰģ���еĽ�������Ļ�ȡ����������
	virtual int GetNodeID(void* node) = 0;
	// ���ݵ�ǰģ���еĽ�������Ļ�ȡ��������
	virtual const char* GetNodeName(void* node) = 0;
	// ���ݵ�ǰģ���еĽ������������ȡ��������
	virtual FXNODE_TYPE GetNodeType(void* node) = 0;
	// ���ݵ�ǰģ���еĽ������������ȡ�������Ϣ
	virtual float GetCameraNodeFov(void* node) = 0;
	virtual float GetCameraNodeFarClip(void* node) = 0;
	virtual float GetCameraNodeNearClip(void* node) = 0;
	// �Ƿ����ڵ�
	virtual bool IsAttachedNode(void* node) = 0;
	// ��ø����ڵ�ĸ��ڵ���
	virtual const char* GetAttachedParentName(void* node) = 0;
	// ��ȡ��ǰģ���еĽ��ĵ�ǰ�任��������任����������ڵ�ǰģ������ϵ��
	virtual void GetCurrentNodeTM(void* node, FmMat4* mtxTM) = 0;
	// ��ȡ��ǰģ�͵ı任����
	virtual void GetCurrentTM(FmMat4* mtxTM) = 0;

	// ��ȡ��ǰģ���Ƿ������ģ����ģ��
	virtual	bool ExistMainModel() = 0;
	// ָ����ǰģ�ͽ�����ģ����ģ��������ײ���
	virtual	void SetTraceMainModel(bool value) = 0;
	// ��ȡ��ǰģ���Ƿ������ģ����ģ��������ײ���
	virtual	bool GetTraceMainModel() = 0;

	// ָ���Ƿ���ʾ��ǰָ����ģ�ͽ��
	virtual void SetShowNode(void* node, bool bShow) = 0;
	// ��ȡ��ǰָ���Ľ���Ƿ���ʾ
	virtual bool GetShowNode(void* node) = 0;
	// ��ȡ��ǰģ�͵Ĳ��ʸ���
	//virtual unsigned int GetMaterialCount();
	// ���ݵ�ǰģ�͵Ĳ���������ȡ����������
	virtual	void* GetMaterialFromID(int iMatIndex) = 0;
	// ���ݵ�ǰģ�͵Ĳ������ƻ�ȡ����������
	virtual	void* GetMaterialFromName(const char* pszMatName) = 0;
	// ���ݲ�������������ȡ��������
	//virtual	int GetMaterialID(void* mat) = 0;
	// ��ȡ���������Ļ�ȡ��������
	virtual	const char* GetMaterialName(void* mat) = 0;
	// ��ȡָ���Ĳ��ʵ��������
	virtual	unsigned int GetMaterialTextureCount(void* mat) = 0;
	// ��ȡָ���Ĳ��ʵ�������ͼ
	virtual const char* GetMaterialTextureName(void* mat, unsigned int index) = 0;
	// ��ȡָ�����ʵĶ��㻺��
	virtual	void* GetMaterialVertexBuffer(void* mat) = 0;
	// ��ȡָ�����ʵ���������
	virtual	void* GetMaterialIndexBuffer(void* mat) = 0;
	// ��ȡָ�����ʵĶ����С
	virtual	unsigned int GetMaterialVertexSize(void* mat) = 0;
	// ��ȡָ�����ʵĶ������
	virtual	unsigned int GetMaterialVertexCount(void* mat) = 0;
	// ��ȡָ�����ʵ�������С��һ��Ϊsizeof(unsigned short)
	virtual	unsigned int GetMaterialIndexSize(void* mat) = 0;
	// ��ȡָ�����ʵ���������
	virtual unsigned int GetMaterialIndexCount(void* mat) = 0;
	// ��ȡָ�����ʵĶ���λ����Ϣ��һ��������ƫ���˶��ٸ��ֽ�
	virtual int GetPositionOffset(void* mat) = 0;
	// ��ȡָ�����ʵĶ��㷨����Ϣ��һ��������ƫ���˶��ٸ��ֽ�
	virtual int GetNormalOffset(void* mat) = 0;
	// ��ȡָ�����ʵĶ�����ɫ��Ϣ��һ��������ƫ���˶��ٸ��ֽ�
	virtual int GetDiffuseOffset(void* mat) = 0;
	// ��ȡָ�����ʵĶ���Lightmap����������Ϣ��һ��������ƫ���˶��ٸ��ֽ�
	virtual int GetLightmapUVOffset(void* mat) = 0;
	// ��ȡָ����Diffuse�����ļ���
	virtual	const char* GetTexDiffuseFileName(void* mat) = 0;
	// ��ȡָ����Bump�����ļ���
	virtual	const char* GetTexBumpFileName(void* mat) = 0;
	// ��ȡָ����Specularmap�����ļ���
	virtual	const char* GetTexSpecularmapFileName(void* mat) = 0;
	// ��ȡָ����Specularlevelmap�����ļ���
	virtual	const char* GetTexSpecularlevelmapFileName(void* mat) = 0;
	// ��ȡָ����Glossinessmap�����ļ���
	virtual	const char* GetTexGlossinessmapFileName(void* mat) = 0;
	// ��ȡָ����Reflectionmap�����ļ���
	virtual	const char* GetTexReflectionmapFileName(void* mat) = 0;
	// ��ȡָ����Lightmap�����ļ���
	virtual	const char* GetTexLightmapFileName(void* mat) = 0;
	// ���ָ�����ʵ�Ambient
	virtual	unsigned int GetMaterialAmbient(void* mat) = 0;
	// ���ָ�����ʵ�Diffuse
	virtual	unsigned int GetMaterialDiffuse(void* mat) = 0;

	// �����Զ���Ĳ���
	virtual bool FindCustomMaterial(const char* mat_name) = 0;
	// �����Զ����������
	virtual bool SetCustomMaterialValue(const char* mat_name,
		const char* key, const char* val) = 0;
	// ��ȡ�Զ����������
	virtual bool GetCustomMaterialValue(const char* mat_name,
		const char* key, char* val, int val_size) = 0;
	// ���¼����Զ�����ʵ�ָ����ͼ
	virtual bool ReloadCustomMaterialTexture(const char* mat_name, 
		const char* key, const char* tex_paths, bool async) = 0;
	// ���¼����Զ�����ʵ�������ͼ
	virtual bool ReloadCustomMaterialTextures(const char* tex_paths, 
		bool async) = 0;

	// ����ģ�Ͳ�������
	virtual bool SetModelMaterialValue(const char* mat_name, 
		const char* key, const char* val) = 0;
	// ��ȡģ�Ͳ�������
	virtual bool GetModelMaterialValue(const char* mat_name,
		const char* key, char* val, int val_size) = 0;
	// ���¼���ģ�͵�������ͼ
	virtual bool ReloadMaterialTextures(const char* tex_paths, 
		bool async) = 0;

	// ���ģ�ͽڵ������Ƿ�ɱ༭
	virtual bool GetModelNodeAllowModify(const char* key) = 0;
	// ����ģ�ͽڵ�����
	virtual bool SetModelNodeValue(const char* node_name, 
		const char* key, const char* val) = 0;
	// ��ȡģ�ͽڵ�����
	virtual bool GetModelNodeValue(const char* node_name,
		const char* key, char* val, int val_size) = 0;

	// ��ȡָ��ģ�͵���Դ�ļ�����
	virtual const char* GetModelFileName() = 0;
	// ��ȡָ��ģ�͵Ĵ洢��ʹ���������
	//virtual void GetMemoryStatus(LPFXMEMORY_STATUS status) = 0;
	// ����ǰģ����ָ��������������Ƥӳ��
	virtual bool InitSkeletonMappingTable() = 0;
	// �жϵ�ǰģ���Ƿ���ָ����������������Ƥӳ��
	virtual bool IsInitSkeletonMappingTable() = 0;
	// �ͷŵ�ǰģ�͵���ƤӰ��
	virtual void ReleaseSkeletonMappingTable() = 0;
	// ���õ�ǰģ�͵�ָ��������Ϊѡ��̬
	virtual void SetSelectHelper(int nHelper) = 0;
	// ��ȡ��ǰģ���д���ѡ��̬�ĸ�����
	virtual int GetSelectHelper() = 0;
	// ��ȡ��ǰģ���а����ĸ��������
	virtual unsigned int GetHelperCount() = 0;
	// ���ݵ�ǰģ�͵ĸ�����������ȡ�����������
	virtual const char* GetHelperIDFromIndex(unsigned int nIndex) = 0;
	// ָ���Ƿ�����ǰָ��ģ�͵�FallOff(������)Ч��
	virtual void SetEnableFallOff(bool value) = 0;
	// �жϵ�ǰָ����ģ���Ƿ�����FallOff(������)Ч��
	virtual bool GetEnableFallOff() = 0;
	// ָ���Ƿ�����ǰָ��ģ�͵ĵ��Դ����Ч��
	virtual void SetEnablePointLighting(bool value) = 0;
	// �жϵ�ǰָ����ģ���Ƿ����˵��Դ����Ч��
	virtual bool GetEnablePointLighting() = 0;
	// ģ�͵�ǰLOD�ļ���
	virtual void SetCurLODLevel(unsigned int lod_level) = 0;
	virtual unsigned int GetCurLODLevel() = 0;
	// ģ���ܹ���LOD����
	virtual unsigned int GetLODLevels() = 0;
	//ȡ�ð�ά��
	virtual void GetNodeLocationBoundingBox(FmVec3& bbDstMin, 
		FmVec3& bbDstMax, FmVec3& bbDstCenter) = 0;
	// ִ�в���
	virtual void Update(float seconds) = 0;

	// �����Ż�
	const FmVec3& GetCenter() const { return m_vCenter; }
	float GetRadius() const { return m_fRadius; }

	//�����û�����Sahder�ĳ���
	virtual void SetUserVsConstValue(const char* name, const FmVec4& value) = 0;

	virtual void EnableKeepOut(bool keepout) = 0;
	
	virtual bool IsEnableKeepOut() = 0;

	virtual void SetKeepOutTexture(const char* filename) = 0;
protected:
	// Ϊ���Ż�
	FmVec3 m_vCenter;
	float m_fRadius;
};

#endif // _VISUAL_I_MODEL_PLAYER_H
