//--------------------------------------------------------------------
// �ļ���:		tex_factory.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��7��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TEX_FACTORY_H
#define _TEX_FACTORY_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/core_mem.h"
#include "../public/i_var_list.h"
#include "../utils/array_pod.h"


class TTexFactoryAlloc
{
public:
	TTexFactoryAlloc() {}
	~TTexFactoryAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TTexFactoryAlloc& src) {}
};

// ��ͼ��Դ����

class Render;
class CTexture;
class IStaticTex;

class CTexFactory
{
public:
	static CTexFactory* NewInstance(Render* pRender);

public:
	CTexFactory(Render* pRender);
	~CTexFactory();

	// �ͷ�
	void Release();
	// ������ͼ��Դ
	CTexture* Create();
	// ������Դ
	void Destroy(CTexture* pResource);
	// ���ݶ���ID�����Դ
	CTexture* Get(const PERSISTID& id) const;
	// ȫ��ɾ��
	void ReleaseAll();
	// ��������
	bool Dump(const char* filename);

	// �������
	size_t GetCount() const { return m_nCount; }

private:
	CTexFactory();
	CTexFactory(const CTexFactory&);
	CTexFactory& operator=(const CTexFactory&);

	// �������к�
	unsigned int GenSerial();

	// Ĭ��һ����ʱ��������(ģ��D3D����������ʽ)
    IStaticTex* CreateTempStaticTex();
private:
	Render* m_pRender;
	IStaticTex* m_pTempTex;
	TArrayPod<CTexture*, 1, TTexFactoryAlloc> m_Resources;
	TArrayPod<unsigned int, 1, TTexFactoryAlloc> m_FreeList;
	unsigned int m_nSerial;
	size_t m_nCount;
};

#endif // _TEX_FACTORY_H
