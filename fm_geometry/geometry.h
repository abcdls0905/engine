//--------------------------------------------------------------------
// �ļ���:		geometry.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��11��25��
// ������:		Ҷ��
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"
#include "../visual/shader_list.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "../visual/i_geometry.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"

class Geometry : public IGeometry
{
private:
	struct geometry_vertex_t 
	{
		FmVec3 pos;
		FmVec3 nor;
	};

	// ������Ⱦ����
	struct BatchData
	{
		IShaderProgram* pShader;
		IStaticVB* pVBuffer;
		IStaticIB* pIBuffer;

		FmVec4 c_vDiffuse;
		FmVec4 c_vLightDirection;
		FmMat4 c_matWVP;
		FmMat4 c_matWorld;

		fm_int hDiffuse;
		fm_int hLightDirection;
		fm_int hMatWVP;
		fm_int hMatWorld;

        bool bZEnable;			//�Ƿ����д��
		bool bLineMode;

        BatchData* pNext;

        BatchData()
        {
            pShader = NULL;
            pVBuffer = NULL;
            pIBuffer = NULL;
            pNext = NULL;
        }
	};

	// ��Ⱦ����
	static void RenderGeometry(void* pdata);

public:
	Geometry();
	virtual ~Geometry();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// ����
	virtual void Realize();

    // ���õ�ǰ������ͼ
    void SetSceneView(ISceneView* pSceneView);

    // ��õ�ǰ������ͼ
    ISceneView* GetSceneView();

    // ��Ⱦ
    virtual void Draw(const FmMat4& mtxWorld);

	// λ��
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// ����
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// ����
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

	// �任����
	virtual bool SetTransform(const FmMat4& mtx);
	virtual const FmMat4& GetTransform() const;

	// ��ɫ
	virtual void SetColor(unsigned int value);
	virtual unsigned int GetColor() const;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// �Ƿ���ģʽ
	virtual void SetLineMode(bool value);
	virtual bool GetLineMode() const;

	// �Ƿ�ʹ����Ȳ���
	void SetZEnable(bool value);
	bool GetZEnable() const;

	// ����
	bool Build(const YsGraphDesc& graphDesc);
	void Unbuild();

	// �Ƿ���Ч
	bool IsReady() const;

private:
	void UpdateTransform();

    BatchData* GetBatchData();

    void UpdateBatchData(BatchData* pBatchData, const FmMat4& mtxWorld);

private:
    IRender* m_pRender;
    ISceneView* m_pSceneView;

	IStaticVB* m_pVB;
	IStaticIB* m_pTrianglesIB;
	IStaticIB* m_pLinesIB;

	fm_int m_hDiffuse;
	fm_int m_hLightDirection;
	fm_int m_hMatWVP;
	fm_int m_hMatWorld;

	FmVec3 m_vPosition;
	FmVec3 m_vAngle;
	FmVec3 m_vScale;
	FmMat4 m_matWorld;

	FmVec4 m_vColor;

	IShaderProgram* m_pShaderProgram;

    // ��Ⱦ����
    BatchData* m_listBatchData;
    long m_nBatchCount;

	bool m_bVisible;
	bool m_bLineMode;
	bool m_bZEnable;
};

#endif // _GEOMETRY_H
