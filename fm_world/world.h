//--------------------------------------------------------------------
// �ļ���:		world.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WORLD_H
#define _WORLD_H

#include "../public/module.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"


#include "../visual/i_world.h"
#include "Scene.h"
// ����Ⱦ����

class IRender;
class ModelSystem;
class Gui;
class Scene;
class RenderTex;

class World: public IWorld
{
private:
	struct render_tex_t
	{
		render_tex_t* pNext;
		RenderTex* pRenderTex;
	};

	// �豸���󲶻�
	static bool DeviceErrorHook(void* context, int thread_id,
		int error, const char* info);

public:
	World();
	virtual ~World();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ÿ֡����
	virtual void Execute(float seconds);
	
	// ÿ֡��Ⱦ
	void Display(float offset_seconds);

	// �����ʾ�豸����
	const char* GetDeviceName() const;
	
	// ���������
	void SetMainGuiID(const PERSISTID& id);
	PERSISTID GetMainGuiID() const;
	Gui* GetMainGui() const { return m_pMainGui; }

	// ����������
	void SetMainSceneID(const PERSISTID& id);
	PERSISTID GetMainSceneID() const;
	IScene* GetMainScene() const { return m_pMainScene; }

	// �����Ⱦ����ͼ
	bool AddRenderTex(const PERSISTID& id);
	// �Ƴ���Ⱦ����ͼ
	bool RemoveRenderTex(const PERSISTID& id);

	// ��Ⱦ���ڿ��
	void SetWidth(int value);
	int GetWidth() const;

	// ��Ⱦ���ڸ߶�
	void SetHeight(int value);
	int GetHeight() const;

	// �Ƿ����������Թ���
	void SetAnisotropic(bool value);
	bool GetAnisotropic() const;

	// ���������Թ���
	void SetMaxAnisotropy(int value);
	int GetMaxAnisotropy() const;

	// ȫ�������
	void SetMultiSampleType(int value);
	int GetMultiSampleType() const;

	// ֻ��ʾ�����α���
	void SetWireFrame(bool value);
	bool GetWireFrame() const;

	// ������ɫ
	void SetBackColor(const char* value);
	result_string GetBackColor() const;

	// ��ײ���ʱ��ALPHA���Բο�ֵ
	void SetTraceAlphaRef(int value);
	int GetTraceAlphaRef() const;

	// ��Ⱦʱ��ALPHA���Բο�ֵ
	void SetRenderAlphaRef(int value);
	int GetRenderAlphaRef() const;

	// ��ͼ����
	void SetTextureLod(int value);
	int GetTextureLod() const;

	// ʹ�ö����ĸ�����ͼ���ݽ�����ײ��⣨�Ӷ���������������Ⱦ����ͼ��
	void SetTraceAloneTexture(int value);
	int GetTraceAloneTexture() const;

	// ������ѭ������ʱ��(����)
	void SetSleep(int value);
	int GetSleep() const;

	// ִ���߼�֡��
	void SetExecuteFPS(int value);
	int GetExecuteFPS() const;

	// ��Դ�����߳�����
	void SetResourceThreadNum(int value);
	int GetResourceThreadNum() const;

	// �Ƿ��첽������Դ
	void SetAsyncLoadResource(bool value);
	bool GetAsyncLoadResource() const;

	// �Ƿ�ʹ�úڰ׸�����������ͼ
	void SetBlackWhiteDiffuse(bool value);
	bool GetBlackWhiteDiffuse() const;

	// ������Ⱦ�豸
	bool CreateDevice(bool multithreading);
	// ���������豸�ߴ�
	bool ResetDevice();

	// ������Ⱦ������Դ��Ϣ
	bool DumpResource(const char* file_name);
	// ����ģ��ϵͳ����Դ��Ϣ
	bool DumpModelResource(const char* file_name);
	// ��õ�ǰ��Դ������������
	int GetResourceLoadTaskNum();
	// ���һЩ��Ⱦ������ʹ�õ���ʱ��Դ
	bool ClearRenderResource();
	// ɾ�����������õ���Դ
	bool CollectResource();
	// ���ճ�ʱ�䲻ʹ�õĶ�������
	int CollectUnuseAction(int seconds);

	// ���¼�������Ч���ļ�
	bool ReloadAllShader();
	
	// ��Ļ��ͼ
	bool ScreenShot(const char* name);

	// �Ƿ�����Ҷ��ײ
	void SetTraceTreeLeaf(bool value);
	bool GetTraceTreeLeaf() const;

	// �����Ƿ��¼�ļ�������־
	bool SetLogFileIO(bool value);

	// ��ȡ�豸Ӳ������
	PERSISTID GetDeviceCapsID() const;

	// �Ƿ�ʹ�ù���ͼ
	void SetUseLightMap(bool value);
	bool GetUseLightMap() const;

public:
	// ������Ⱦ�豸����
	bool SetDeviceParam();

private:
	IRender* m_pRender;
	ModelSystem* m_pModelSystem;
	Gui* m_pMainGui;
	Scene* m_pMainScene;
	render_tex_t* m_pRenderTex;
	unsigned int m_nBackColor;
	int m_nWidth;
	int m_nHeight;
	bool m_bAnisotropic;
	int m_nMaxAnisotropy;
	int m_nMultiSampleType;
};

#endif // _WORLD_H
