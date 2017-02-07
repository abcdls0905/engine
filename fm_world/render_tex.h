//--------------------------------------------------------------------
// �ļ���:		render_tex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��12��8��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _RENDER_TEX_H
#define _RENDER_TEX_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"

// ��Ϊ��ȾĿ�����ͼ

class IRender;
class IRenderContext;
class ISceneView;
class IVisBase;
class Weather;
class Camera;

class RenderTex: public IEntity
{
private:
	struct object_t
	{
		PERSISTID ObjectId;
		int nPrior;
		object_t* pNext;
	};
	
public:
	RenderTex();
	virtual ~RenderTex();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	bool Load();

	void Update(float seconds);
	void Realize();

	// ��Ⱦ������
	IRenderContext* GetContext() const { return m_pContext; }
	// ��ó�����ͼ
	ISceneView* GetSceneView() const { return m_pSceneView; }

	// �����������
	PERSISTID GetWeatherID();
	Weather* GetWeather() const { return m_pWeather; }
	// ������������
	PERSISTID GetCameraID();
	Camera* GetCamera() const { return m_pCamera; }

	// ��ͼ����
	void SetName(const char* value);
	const char* GetName() const;

	// ��ͼ���
	void SetWidth(int value);
	int GetWidth() const;

	// ��ͼ�߶�
	void SetHeight(int value);
	int GetHeight() const;

	// ������ɫ
	void SetBackColorString(const char* value);
	result_string GetBackColorString() const;
	
	// Զ������ľ���
	void SetFarClipDistance(float value);
	float GetFarClipDistance() const;

	// �������Ӷ���
	PERSISTID Create(const char* name);
	// ɾ�����Ӷ���
	bool Delete(const PERSISTID& id);

	// ����Ψһʵ��
	bool SetUnique(const PERSISTID& obj);
	PERSISTID GetUnique(const char* entity_name);

	// ��ӿ��Ӷ���
	bool AddObject(const PERSISTID& obj, int priority);
	// ɾ�����Ӷ���
	bool RemoveObject(const PERSISTID& obj);
	// �����ȼ�ɾ������
	bool RemoveObjectByPrior(int priority);

private:
	// ��ÿɼ�����
	IVisBase* GetVisBase(const PERSISTID& id) const;

private:
	IRender* m_pRender;
	IRenderContext* m_pContext;
	ISceneView* m_pSceneView;
	Weather* m_pWeather;
	Camera* m_pCamera;
	object_t* m_pObjects;
	core_string m_strName;
	int m_nWidth;
	int m_nHeight;
	unsigned int m_nBackColor;
	ITexture* m_pRenderTex;
	IDepthRT* m_pDepthRT;
};

#endif // _RENDER_TEX_H
