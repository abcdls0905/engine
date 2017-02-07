//--------------------------------------------------------------------
// �ļ���:		skin.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��2��10��
// ������:		�ݾ���
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SKIN_H
#define _SKIN_H

#include "../public/core_type.h"
#include "../visual/i_model_system.h"
#include "../visual/i_vis_base.h"
// ��������������Ҫ��ͷ�ļ�
//#include "../physics/PhysicsMacros.h"
#include "../visual/i_render.h"
#include "../visual/i_skin.h"

// ������������ģ��

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

	// ���������Ƿ�ɹ�
	virtual bool GetLoaded();
	// �Ƿ�������
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

	void SetColorString(const char* value);
	result_string GetColorString() const;

	// ���ģ�Ͳ�����
	IModelPlayer* GetModelPlayer() const { return m_pModelPlayer; }

	// ����������
	void SetActionPlayer(IActionPlayer* value);
	IActionPlayer* GetActionPlayer() const;

	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value);
	virtual bool GetTraceEnable() const;

	// �Ƿ���Լ���
	virtual void SetCullEnable(bool value);
	virtual bool GetCullEnable() const;

	// ��ȡ��ǰģ���Ƿ������ģ����ģ��
	bool ExistMainModel();
	// ָ����ǰģ�ͽ�����ģ����ģ��������ײ���
	void SetTraceMainModel(bool value);
	// ��ȡ��ǰģ���Ƿ������ģ����ģ��������ײ���
	bool GetTraceMainModel();

	// ���ƣ������ã�
	const char* GetName() const { return m_strModelFile.c_str(); }

	// �Ƿ��첽����
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// �Ƿ���ʾ��Χ��
	void SetShowBoundBox(bool value) { m_bShowBoundBox = value; }
	bool GetShowBoundBox() const { return m_bShowBoundBox; }

	// ���ò����ļ�
	void SetMaterialFile(const char* value);
	const char* GetMaterialFile() const;

	// ���ز����ļ�
	bool LoadMaterial();

	// ģ���ļ�
	void SetModelFile(const char* pszModelFile);
	const char* GetModelFile();

	// ��ͼ·��
	void SetTexPaths(const char* value);
	const char* GetTexPaths() const;

	// ���ù�������ģ��
	bool AddModel(IActionPlayer* pActionPlayer, const char* pszModelFile);

	// ��ø��ڵ����б�
	void GetRootNodeList(const IVarList& args, IVarList& result);
	// ��ø����ڵ����б�
	void GetHelperNodeList(const IVarList& args, IVarList& result);
	// ��ø�����ĵ�ǰλ��
	void GetHelperPosition(const IVarList& args, IVarList& result);
	// ��ø�����ĵ�ǰ����
	void GetHelperAngle(const IVarList& args, IVarList& result);
	// �������ֻ�ýڵ�����
	int GetNodeFromName(const char* pszName);
	// ���ָ���ڵ�ľ���
	void GetCurrentNodeMatrix(int nNodeID, FmMat4* pTM);
	bool GetCurrentNodeMatrix(const char* pNodeName, FmMat4* pTM);

	// ���ָ����������ͽڵ����Ϣ
	void GetCameraInfo(const IVarList& args, IVarList& result);

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
	// ��ȡ���ʵ���ͼ��
	int GetMaterialTexCount(int index) const;
	// ��ȡָ�����ʵĵ�������ͼ�ļ�
	const char* GetMaterialTexName(int mat_index, int tex_index) const;

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

	// ����������Ƥģ�͵Ĺ���map
	bool ResetSkinMap();

private:
	// ���°�Χ��
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
