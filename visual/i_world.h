#ifndef _I_WORLD_H
#define _I_WORLD_H

#include "../public/module.h"

class Gui;
class IScene;
class IWorld : public IEntity
{
public:
		// ÿ֡��Ⱦ
	virtual void Display(float offset_seconds) = 0;

	// ���������
	virtual void SetMainGuiID(const PERSISTID& id) = 0;
	
	virtual PERSISTID GetMainGuiID() const = 0;
	
	virtual Gui* GetMainGui() const  = 0;

	// ����������
	virtual void SetMainSceneID(const PERSISTID& id) = 0;
	virtual PERSISTID GetMainSceneID() const = 0;
	virtual IScene* GetMainScene() const = 0;

	// �����Ⱦ����ͼ
	virtual bool AddRenderTex(const PERSISTID& id) = 0;
	// �Ƴ���Ⱦ����ͼ
	virtual bool RemoveRenderTex(const PERSISTID& id) = 0;

	// ��Ⱦ���ڿ��
	virtual void SetWidth(int value) = 0;
	virtual int GetWidth() const = 0;

	// ��Ⱦ���ڸ߶�
	virtual void SetHeight(int value) = 0;
	virtual int GetHeight() const = 0;

	// �Ƿ����������Թ���
	virtual void SetAnisotropic(bool value) = 0;
	virtual bool GetAnisotropic() const = 0;

	// ���������Թ���
	virtual void SetMaxAnisotropy(int value) = 0;
	virtual int GetMaxAnisotropy() const = 0;

	// ȫ�������
	virtual void SetMultiSampleType(int value) = 0;
	virtual int GetMultiSampleType() const = 0;

	// ֻ��ʾ�����α���
	virtual void SetWireFrame(bool value) = 0;
	virtual bool GetWireFrame() const = 0;

	// ������ɫ
	virtual void SetBackColor(const char* value) = 0;
	virtual result_string GetBackColor() const = 0;

	// ��ײ���ʱ��ALPHA���Բο�ֵ
	virtual void SetTraceAlphaRef(int value) = 0;
	virtual int GetTraceAlphaRef() const = 0;

	// ��Ⱦʱ��ALPHA���Բο�ֵ
	virtual void SetRenderAlphaRef(int value) = 0;
	virtual int GetRenderAlphaRef() const = 0;

	// ��ͼ����
	virtual void SetTextureLod(int value) = 0;
	virtual int GetTextureLod() const = 0;

	// ʹ�ö����ĸ�����ͼ���ݽ�����ײ��⣨�Ӷ���������������Ⱦ����ͼ��
	virtual void SetTraceAloneTexture(int value) = 0;
	virtual int GetTraceAloneTexture() const = 0;

	// ������ѭ������ʱ��(����)
	virtual void SetSleep(int value) = 0;
	virtual int GetSleep() const = 0;

	// ִ���߼�֡��
	virtual void SetExecuteFPS(int value) = 0;
	virtual int GetExecuteFPS() const = 0;

	// ��Դ�����߳�����
	virtual void SetResourceThreadNum(int value) = 0;
	virtual int GetResourceThreadNum() const = 0;

	// �Ƿ��첽������Դ
	virtual void SetAsyncLoadResource(bool value) = 0;
	virtual bool GetAsyncLoadResource() const = 0;

	// �Ƿ�ʹ�úڰ׸�����������ͼ
	virtual void SetBlackWhiteDiffuse(bool value) = 0;
	virtual bool GetBlackWhiteDiffuse() const = 0;

	// ������Ⱦ�豸
	virtual bool CreateDevice(bool multithreading =false) = 0;
	// ���������豸�ߴ�
	virtual bool ResetDevice() = 0;

	// ������Ⱦ������Դ��Ϣ
	virtual bool DumpResource(const char* file_name) = 0;
	// ����ģ��ϵͳ����Դ��Ϣ
	virtual bool DumpModelResource(const char* file_name) = 0;
	// ��õ�ǰ��Դ������������
	virtual int GetResourceLoadTaskNum() = 0;
	// ���һЩ��Ⱦ������ʹ�õ���ʱ��Դ
	virtual bool ClearRenderResource() = 0;
	// ɾ�����������õ���Դ
	virtual bool CollectResource() = 0;
	// ���ճ�ʱ�䲻ʹ�õĶ�������
	virtual int CollectUnuseAction(int seconds) = 0;

	// ���¼�������Ч���ļ�
	virtual bool ReloadAllShader() = 0;
	
	// ��Ļ��ͼ
	virtual bool ScreenShot(const char* name) = 0;

	// �Ƿ�����Ҷ��ײ
	virtual void SetTraceTreeLeaf(bool value) = 0;
	virtual bool GetTraceTreeLeaf() const = 0;

	// �����Ƿ��¼�ļ�������־
	virtual bool SetLogFileIO(bool value) = 0;

	// ��ȡ�豸Ӳ������
	virtual PERSISTID GetDeviceCapsID() const = 0;

	// �Ƿ�ʹ�ù���ͼ
	virtual void SetUseLightMap(bool value) = 0;
	virtual bool GetUseLightMap() const = 0;
};
#endif