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

	// ���������Ƿ�ɹ�
	virtual bool GetLoaded() = 0;
	// �Ƿ�������
	virtual bool GetLoadFinish() = 0; 

	// ������ɺ�
	virtual bool AfterLoaded() = 0;

	// λ��
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// ����
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// ����ϵ��
	virtual bool SetScale(float x, float y, float z) = 0; 
	virtual FmVec3 GetScale() const = 0;

	// ȫ�����ĵ�
	virtual FmVec3 GetCenter() = 0; 
	// ��Χ�гߴ�
	virtual FmVec3 GetBoxSize() = 0;
	// ģ�Ͱ뾶
	virtual float GetRadius() = 0; 

	// ����ת������
	virtual void SetWorldMatrix(const FmMat4& mat) = 0; 
	virtual void GetWorldMatrix(FmMat4& mat) = 0;

	// ��ײ������ϸ��Ϣ
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result) = 0;
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst) = 0;
	// ���巶Χ��ײ
	virtual bool TraceSphere(const FmVec3& center, float radius) = 0; 
	// ��ѡ
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context) = 0;

	// �Ƿ�ɼ� 
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

	// �Ƿ���ˮ��
	virtual void SetUnderWater(bool value) = 0;
	virtual bool GetUnderWater() const = 0;

	// ��ɫ
	virtual void SetColor(unsigned int value) = 0;
	virtual unsigned int GetColor() const = 0;

	// ���ģ�Ͳ�����
	virtual IModelPlayer* GetModelPlayer() = 0;

	// ���ƣ������ã�
	virtual const char* GetName() const = 0;

	// �Ƿ��Ѿ����ع���
	virtual bool GetLightLoaded() const = 0;
	// �Ƿ���ù���ͼ
	virtual bool GetHasLightMap() const = 0;
	// �Ƿ���÷�����ͼ
	virtual bool GetHasNormalMap() const = 0;
	// �Ƿ���ö���ɫ
	virtual bool GetHasVertexColor() const = 0;

	// �Ƿ��첽����
	virtual void SetAsyncLoad(bool value) = 0;
	virtual bool GetAsyncLoad() const = 0;

	// �Ƿ���ʾ��Χ��
	virtual void SetShowBoundBox(bool value) = 0;
	virtual bool GetShowBoundBox() const = 0;

	// �Ƿ�ʹ�ö������
	virtual void SetUseVertexColor(bool value) = 0;
	virtual bool GetUseVertexColor() const = 0;

	// �Ƿ�ʹ�ù���ͼ
	virtual void SetUseLightMap(bool value) = 0;
	virtual bool GetUseLightMap() const = 0;

	// �Ƿ������������λ�ð�͸����ʾ
	//void SetFocusTransparent(bool value) = 0;
	//bool GetFocusTransparent() const = 0;

	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value) = 0;
	virtual bool GetTraceEnable() const = 0;

	// �Ƿ���Լ���
	virtual void SetCullEnable(bool value) = 0;
	virtual bool GetCullEnable() const = 0;

	// �Ƿ���Ҫˮ�淴��
	virtual void SetWaterReflect(bool value) = 0;
	virtual bool GetWaterReflect() const = 0;

	// д���ٶ������־
	virtual void SetWriteVelocity(int value) = 0;
	virtual int GetWriteVelocity() const = 0;

	// ��¼��ʱ��͸���ȱ�����ֻ������ϵͳʹ�ã�
	virtual void SetAlphaValue(float value) = 0;
	virtual float GetAlphaValue() const = 0;

	// ����ҪͶ��Ӱ��
	virtual void SetCastShadow(bool value) = 0; 
	virtual bool GetCastShadow() const  = 0;

	// ����Ҫ����Ͷ��Ӱ��
	virtual void SetReceiveShadow(bool value) = 0;
	virtual bool GetReceiveShadow() const = 0;

	// ģ����ɫ
	virtual void SetColorString(const char* value) = 0;
	virtual result_string GetColorString() const = 0;

	// ������Ϣ
	virtual void SetExtraInfo(int value) = 0;
	virtual int GetExtraInfo() const = 0;

	// ģ���ļ�
	virtual void SetModelFile(const char* value) = 0; 
	virtual const char* GetModelFile() const = 0;

	// �����ļ�
	virtual void SetLightFile(const char* value) = 0;
	virtual const char* GetLightFile() const = 0;

	// �����ļ�
	virtual void SetMaterialFile(const char* value) = 0;
	virtual const char* GetMaterialFile() const = 0;

	// ��ͼ·��
	virtual void SetTexPaths(const char* value) = 0;
	virtual const char* GetTexPaths() const = 0;

	// ���ӽ����͸������
	//bool AddFocusTransparent(int mat_index) = 0;
	// ������н����͸������
	//bool ClearFocusTransparent() = 0;

	// ���ع����ļ�
	virtual bool LoadLight() = 0; 
	// ���ز����ļ�
	virtual bool LoadMaterial() = 0;
	
	// ��ײ���Ի�ø߶�
	virtual float TraceHeight(float x, float z, float high_y, float low_y) = 0; 

	// ��ø����������б�
	virtual void GetHelperGroupList(const IVarList& args, IVarList& result) = 0;
	// ��ø����������б�
	virtual void GetHelperNameList(const IVarList& args, IVarList& result) = 0;
	// ��ø�����ĵ�ǰλ��
	virtual void GetHelperPosition(const IVarList& args, IVarList& result) = 0;
	// ��ø�����ĵ�ǰ����
	virtual void GetHelperAngle(const IVarList& args, IVarList& result) = 0;
	
	// ���ָ���㣨�����������Ϣ
	virtual void GetCameraInfo(const IVarList& args, IVarList& result) = 0;

	// �Ƿ���ʾ������
	virtual void ShowHelper(bool bShow) = 0;
	virtual bool IsShowHelper() const = 0;

	// ��ýڵ����
	virtual void GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM) = 0;
	virtual bool GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM) = 0;
	// �������ֲ��ҽڵ�
	virtual int GetNodeFromName(const char* pszName) = 0;
	// ���ָ���ڵ������
	virtual const char* GetHelperName(int helpID) = 0;

	// ��ǰѡ��Ľڵ�
	virtual void SetSelectHelper(int nHelper) = 0;
	virtual int GetSelectHelper() = 0;
	
	// ����
	virtual bool Play() = 0;
	// ֹͣ
	virtual bool Stop() = 0;
	// ��ͣ
	virtual bool Pause() = 0;
	// �ָ�����
	virtual bool Resume() = 0;
	
	// �Ƿ�ѭ��
	virtual void SetLoop(bool bLoop) = 0;
	virtual bool GetLoop() = 0;
	// ���֡��
	virtual int GetFrameCount() = 0;
	// ��õ�ǰ����֡
	virtual int GetCurrentFrame() = 0;
	// ���õ�ǰ����֡
	virtual bool SetCurrentFrame(int nFrame) = 0;
	// ��õ�ǰ����ʱ��
	virtual double GetCurrentTimeFloat() = 0;
	// ���õ�ǰ����ʱ��
	virtual bool SetCurrentTimeFloat(double fTime) = 0;
	// ���ȱʡ�����ٶ�
	virtual float GetDefaultSpeed() = 0;
	// ���ò����ٶ�
	virtual bool SetSpeed(float speed) = 0;

	// �Ը����������ײ���
	virtual int	TraceHelper(const FmVec3& start, const FmVec3& direction) = 0;
	// ʹ����Ļ����Ը����������ײ���
	virtual int TraceHelperInScreen(int screen_x, int screen_y) = 0;
	// ��ø���������
	virtual int GetHelperCount() = 0;
	// ��ø���������
	virtual const char* GetHelperIDFromIndex(int nIndex) = 0;

	// �ڵ���Ϣ
	virtual size_t GetObjectCount() const = 0;
	virtual size_t GetObjectVertexCount(size_t index) const = 0;
	virtual size_t GetObjectTriangleCount(size_t index) const = 0;
	virtual int GetAllVertexCount() const = 0;
	virtual int GetAllTriangleCount() = 0;

	// ���ģ�ͽڵ����
	virtual int GetNodeCount() = 0;
	// ���ģ�͵����нڵ����б�
	virtual void GetNodeNameList(const IVarList& args, IVarList& result) = 0;
	// ���ģ�ͽڵ������Ƿ�ɱ༭
	virtual bool GetModelNodeAllowModify(const char* key) = 0;
	// ����ģ�ͽڵ�����
	virtual bool SetNodeValue(const char* node_name, const char* key, const char* val) = 0;
	// ��ȡģ�ͽڵ�����
	virtual result_string GetNodeValue(const char* node_name, const char* key) = 0;

	// ��ȡ������
	virtual int GetMaterialCount() const = 0;
	// ��ȡָ�����ʵ�������ͼ��
	virtual int GetMaterialTexCount(int index) const = 0;
	// ��ȡָ�����ʵĵ�������ͼ�ļ�
	virtual const char* GetMaterialTexName(int mat_index, int tex_index) const = 0;
	// ���ָ�����ʵ�Ambient
	virtual result_string GetMaterialAmbient(size_t mat_index) const = 0;
	// ���ָ�����ʵ�Diffuse
	virtual result_string GetMaterialDiffuse(size_t mat_index) const = 0;

	// ���ָ�����㻺���ָ���������ɫ����
	virtual result_string GetVertexDiffuse(size_t vb_index, size_t vertex_index) const = 0;

	// ���ģ�͵����в������б�
	virtual void GetMaterialNameList(const IVarList& args, IVarList& result) = 0;
	// ����ģ�Ͳ�������
	virtual bool SetMaterialValue(const char* mat_name, const char* key, 
		const char* val) = 0;
	// ��ȡģ�Ͳ�������
	virtual result_string GetMaterialValue(const char* mat_name, const char* key) = 0;
	// ���¼���ģ�Ͳ��ʵ�������ͼ
	virtual bool ReloadMaterialTextures() = 0;

	// �����Ƿ�����Զ����������
	virtual bool FindCustomMaterial(const char* mat_name) = 0;
	// �����Զ����������
	virtual bool SetCustomMaterialValue(const char* mat_name, const char* key, 
		const char* val) = 0;
	// ��ȡ�Զ����������
	virtual result_string GetCustomMaterialValue(const char* mat_name, const char* key) = 0;
	// ���¼����Զ�����ʵ�ָ����ͼ
	virtual bool ReloadCustomMaterialTexture(const char* mat_name, const char* key) = 0;
	// ���¼����Զ�����ʵ�������ͼ
	virtual bool ReloadCustomMaterialTextures() = 0;

	// ģ�͵�ǰLOD�ļ���
	virtual void SetCurLODLevel(unsigned int lod_level) = 0;
	virtual unsigned int GetCurLODLevel() = 0;
	// ģ���ܹ���LOD����
	virtual unsigned int GetLODLevels() = 0;

	// Warning:This API just for LOD tools use!
	virtual bool LOD(unsigned int lod_levels, float dec_ratio) = 0;

	virtual void OnLinkEvent(bool bLink) = 0;

	// ����ڴ�ռ��
	virtual size_t GetMemoryUsage() = 0;

};
#endif