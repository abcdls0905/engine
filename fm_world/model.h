//--------------------------------------------------------------------
// �ļ���:		model.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _MODEL_H
#define _MODEL_H

#include "../public/core_type.h"

#include "../visual/i_vis_base.h"
#include "../visual/i_model_player.h"
#include "../visual/i_render.h"
#include "../visual/i_model.h"

// ��������������Ҫ��ͷ�ļ�
//#include "../physics/PhysicsMacros.h"

// 3Dģ�Ͷ���

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

	// ���������Ƿ�ɹ�
	virtual bool GetLoaded();
	// �Ƿ�������
	virtual bool GetLoadFinish(); 

	// ������ɺ�
	bool AfterLoaded();

	// λ��
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// ����
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// ����ϵ��
	virtual bool SetScale(float x, float y, float z); 
	virtual FmVec3 GetScale() const;

	// ȫ�����ĵ�
	virtual FmVec3 GetCenter(); 
	// ��Χ�гߴ�
	virtual FmVec3 GetBoxSize();
	// ģ�Ͱ뾶
	virtual float GetRadius(); 

	// ����ת������
	virtual void SetWorldMatrix(const FmMat4& mat); 
	virtual void GetWorldMatrix(FmMat4& mat);

	// ��ײ������ϸ��Ϣ
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result);
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// ���巶Χ��ײ
    virtual bool TraceSphere(const FmVec3& center, float radius); 
    // ��������ײ
    virtual bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2);
	// ��ѡ
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// �Ƿ�ɼ� 
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// �Ƿ���ˮ��
	virtual void SetUnderWater(bool value);
	virtual bool GetUnderWater() const;

	// ��ɫ
	virtual void SetColor(unsigned int value);
	virtual unsigned int GetColor() const;

	// ���ģ�Ͳ�����
	virtual IModelPlayer* GetModelPlayer() { return m_pModelPlayer; }

	// ���ƣ������ã�
	const char* GetName() const { return m_strModelFile.c_str(); }

	// �Ƿ��Ѿ����ع���
	bool GetLightLoaded() const;
	// �Ƿ���ù���ͼ
	bool GetHasLightMap() const;
	// �Ƿ���÷�����ͼ
	bool GetHasNormalMap() const;
	// �Ƿ���ö���ɫ
	bool GetHasVertexColor() const;

	// �Ƿ��첽����
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// �Ƿ���ʾ��Χ��
	void SetShowBoundBox(bool value);
	bool GetShowBoundBox() const;

	// �Ƿ�ʹ�ö������
	void SetUseVertexColor(bool value);
	bool GetUseVertexColor() const;

	// �Ƿ�ʹ�ù���ͼ
	void SetUseLightMap(bool value);
	bool GetUseLightMap() const;

	// �Ƿ������������λ�ð�͸����ʾ
	//void SetFocusTransparent(bool value);
	//bool GetFocusTransparent() const;

	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value);
	virtual bool GetTraceEnable() const;

	// �Ƿ���Լ���
	virtual void SetCullEnable(bool value);
	virtual bool GetCullEnable() const;

	// �Ƿ���Ҫˮ�淴��
	virtual void SetWaterReflect(bool value) { m_bWaterReflect = value; }
	virtual bool GetWaterReflect() const { return m_bWaterReflect; }

	// д���ٶ������־
	virtual void SetWriteVelocity(int value);
	virtual int GetWriteVelocity() const;

	// ��¼��ʱ��͸���ȱ�����ֻ������ϵͳʹ�ã�
	virtual void SetAlphaValue(float value) { m_fAlphaValue = value; }
	virtual float GetAlphaValue() const { return m_fAlphaValue; }

	// ����ҪͶ��Ӱ��
	virtual void SetCastShadow(bool value);
	virtual bool GetCastShadow() const { return m_bCastShadow; }

	// ����Ҫ����Ͷ��Ӱ��
	virtual void SetReceiveShadow(bool value);
	virtual bool GetReceiveShadow() const { return m_bReceiveShadow; }

	// ģ����ɫ
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// ������Ϣ
	void SetExtraInfo(int value);
	int GetExtraInfo() const;

	// ģ���ļ�
	void SetModelFile(const char* value); 
	const char* GetModelFile() const;

	// �����ļ�
	void SetLightFile(const char* value);
	const char* GetLightFile() const;

	// �����ļ�
	void SetMaterialFile(const char* value);
	const char* GetMaterialFile() const;

	// ��ͼ·��
	void SetTexPaths(const char* value);
	const char* GetTexPaths() const;

	// ���ӽ����͸������
	//bool AddFocusTransparent(int mat_index);
	// ������н����͸������
	//bool ClearFocusTransparent();

	// ���ع����ļ�
	bool LoadLight(); 
	// ���ز����ļ�
	bool LoadMaterial();
	
	// ��ײ���Ի�ø߶�
	float TraceHeight(float x, float z, float high_y, float low_y); 

	// ��ø����������б�
	void GetHelperGroupList(const IVarList& args, IVarList& result);
	// ��ø����������б�
	void GetHelperNameList(const IVarList& args, IVarList& result);
	// ��ø�����ĵ�ǰλ��
	void GetHelperPosition(const IVarList& args, IVarList& result);
	// ��ø�����ĵ�ǰ����
	void GetHelperAngle(const IVarList& args, IVarList& result);
	
	// ���ָ���㣨�����������Ϣ
	void GetCameraInfo(const IVarList& args, IVarList& result);

	// �Ƿ���ʾ������
	void ShowHelper(bool bShow);
	bool IsShowHelper() const;

	// ��ýڵ����
	void GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM);
	bool GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM);
	// �������ֲ��ҽڵ�
	int GetNodeFromName(const char* pszName);
	// ���ָ���ڵ������
	const char* GetHelperName(int helpID);

	// ��ǰѡ��Ľڵ�
	void SetSelectHelper(int nHelper);
	int GetSelectHelper();
	
	// ����
	bool Play();
	// ֹͣ
	bool Stop();
	// ��ͣ
	bool Pause();
	// �ָ�����
	bool Resume();
	
	// �Ƿ�ѭ��
	void SetLoop(bool bLoop);
	bool GetLoop();
	// ���֡��
	int GetFrameCount();
	// ��õ�ǰ����֡
	int GetCurrentFrame();
	// ���õ�ǰ����֡
	bool SetCurrentFrame(int nFrame);
	// ��õ�ǰ����ʱ��
	double GetCurrentTimeFloat();
	// ���õ�ǰ����ʱ��
	bool SetCurrentTimeFloat(double fTime);
	// ���ȱʡ�����ٶ�
	float GetDefaultSpeed();
	// ���ò����ٶ�
	bool SetSpeed(float speed);

	// �Ը����������ײ���
	int	TraceHelper(const FmVec3& start, const FmVec3& direction);
	// ʹ����Ļ����Ը����������ײ���
	int TraceHelperInScreen(int screen_x, int screen_y);
	// ��ø���������
	int GetHelperCount();
	// ��ø���������
	const char* GetHelperIDFromIndex(int nIndex);

	// �ڵ���Ϣ
	size_t GetObjectCount() const;
	size_t GetObjectVertexCount(size_t index) const;
	size_t GetObjectTriangleCount(size_t index) const;
	int GetAllVertexCount() const;
	int GetAllTriangleCount();

	// ���ģ�ͽڵ����
	int GetNodeCount();
	// ���ģ�͵����нڵ����б�
	void GetNodeNameList(const IVarList& args, IVarList& result);
	// ���ģ�ͽڵ������Ƿ�ɱ༭
	bool GetModelNodeAllowModify(const char* key);
	// ����ģ�ͽڵ�����
	bool SetNodeValue(const char* node_name, const char* key, const char* val);
	// ��ȡģ�ͽڵ�����
	result_string GetNodeValue(const char* node_name, const char* key);

	// ��ȡ������
	int GetMaterialCount() const;
	// ��ȡָ�����ʵ�������ͼ��
	int GetMaterialTexCount(int index) const;
	// ��ȡָ�����ʵĵ�������ͼ�ļ�
	const char* GetMaterialTexName(int mat_index, int tex_index) const;
	// ���ָ�����ʵ�Ambient
	result_string GetMaterialAmbient(size_t mat_index) const;
	// ���ָ�����ʵ�Diffuse
	result_string GetMaterialDiffuse(size_t mat_index) const;

	// ���ָ�����㻺���ָ���������ɫ����
	result_string GetVertexDiffuse(size_t vb_index, size_t vertex_index) const;

	// ���ģ�͵����в������б�
	void GetMaterialNameList(const IVarList& args, IVarList& result);
	// ����ģ�Ͳ�������
	bool SetMaterialValue(const char* mat_name, const char* key, 
		const char* val);
	// ��ȡģ�Ͳ�������
	result_string GetMaterialValue(const char* mat_name, const char* key);
	// ���¼���ģ�Ͳ��ʵ�������ͼ
	bool ReloadMaterialTextures();

	// �����Ƿ�����Զ����������
	bool FindCustomMaterial(const char* mat_name);
	// �����Զ����������
	bool SetCustomMaterialValue(const char* mat_name, const char* key, 
		const char* val);
	// ��ȡ�Զ����������
	result_string GetCustomMaterialValue(const char* mat_name, const char* key);
	// ���¼����Զ�����ʵ�ָ����ͼ
	bool ReloadCustomMaterialTexture(const char* mat_name, const char* key);
	// ���¼����Զ�����ʵ�������ͼ
	bool ReloadCustomMaterialTextures();

	// ģ�͵�ǰLOD�ļ���
	void SetCurLODLevel(unsigned int lod_level);
	unsigned int GetCurLODLevel();
	// ģ���ܹ���LOD����
	unsigned int GetLODLevels();

	// Warning:This API just for LOD tools use!
	bool LOD(unsigned int lod_levels, float dec_ratio);

	void OnLinkEvent(bool bLink);

	// ����ڴ�ռ��
	virtual size_t GetMemoryUsage();

private:
	// ����ģ��
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
