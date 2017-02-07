//--------------------------------------------------------------------
// �ļ���:		cube_tex_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��7��22��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CUBE_TEX_LOADER_H
#define _CUBE_TEX_LOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/i_res_loader.h"

#include "tex_tools.h"
// ��������ͼ������

class CTexture;
class Render;

class CCubeTexLoader: public IResLoader
{
public:
	static CCubeTexLoader* NewInstance();

public:
	CCubeTexLoader();
	virtual ~CCubeTexLoader();

	// �ͷ�
	virtual void Release();
	// �������
	virtual const char* GetName();
	// ����
	virtual bool Load(bool async);
	// ����
	virtual bool Create(bool async);
	// �����첽���� 
	virtual bool EndCreate(int category, bool succeed);

	// ��Դ����
	void SetOwner(CTexture* pOwner);
	// ��ͼ�ļ���
	void SetFileName(const char* file_name);
	
private:
	CCubeTexLoader(const CCubeTexLoader&);
	CCubeTexLoader& operator=(const CCubeTexLoader&);

	// ����DDS������ͨ�ø�ʽ��ͼ
	//bool LoadDDS(const char* file_name);
	//bool LoadDDSData(const char* file_name, const void* dds_data, 
	//	size_t dds_size);

private:
	CTexture* m_pOwner;
	Render* m_pRender;
	core_string m_strFileName;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nFormat;
	IMAGE_INFO m_ImageInfo;
	fm_uint m_pTexture;
	unsigned char* m_pData;
	unsigned int m_nDataSize;
};

#endif // _CUBE_TEX_LOADER_H
