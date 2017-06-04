
#ifndef _TEX_LOADER_H
#define _TEX_LOADER_H

#include "../public/macros.h"
#include "../public/core_type.h"


#include "../visual/i_res_loader.h"
#include "../visual/vis_utils.h"
#include <assert.h>

/*
#define FREEIMAGE_LIB

#include "freeimage.h"
*/
#ifndef __APPLE__
	#include "libpvr.h"
#endif

// ��ͼ������

class CTexture;
class Render;

#include <math.h>
#include "tex_tools.h"

class CTexLoader: public IResLoader
{
public:
	static CTexLoader* NewInstance();
public:
	CTexLoader();
	virtual ~CTexLoader();

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
	// �Ƿ񱣳�ԭ�гߴ�
	void SetOriginSize(bool value);
	
private:
	CTexLoader(const CTexLoader&);
	CTexLoader& operator=(const CTexLoader&);

	// ����DDS������ͨ�ø�ʽ��ͼ
	//bool LoadDDS(const char* file_name);
	//bool LoadDDSData(const char* file_name, const void* dds_data, 
	//	size_t dds_size);

	bool load_file(const char* name);
private:
	CTexture* m_pOwner;
	Render* m_pRender;
	core_string m_strFileName;
	bool m_bOriginSize;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nFormat;
	unsigned int m_nOriginWidth;
	unsigned int m_nOriginHeight;
	unsigned char* m_pData;
	unsigned int m_nDataSize;
	unsigned char* m_pDataMask;
	unsigned int m_nDataMaskSize;

	IMAGE_INFO m_ImageInfo;
	fm_uint m_pTexture;
};

#endif // _TEX_LOADER_H
