//--------------------------------------------------------------------
// �ļ���:		light_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _LIGHT_LOADER_H
#define _LIGHT_LOADER_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_res_loader.h"

// ������ռ�����

class CTerrainZone;
class Render;

#pragma pack(push, 1)

struct tga_header_t
{
	unsigned char byte1;		// = 0
	unsigned char byte2;		// = 0
	unsigned char type;
	unsigned char byte4_9[9];	// = 0
	unsigned short width;
	unsigned short height;
	unsigned char bpp;			// bit per pixel
	union
	{
		unsigned char attr8;
		struct
		{
			unsigned char attr: 4;
			unsigned char rez: 1;
			unsigned char origin: 1;
			unsigned char storage: 2;
		};
	};
};
	
#pragma pack(pop)


class CLightLoader: public IResLoader
{
public:
	static CLightLoader* NewInstance();
	
public:
	CLightLoader();
	virtual ~CLightLoader();

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
	void SetOwner(CTerrainZone* pOwner);
	// λͼ�ļ���
	void SetBitmap(const char* file_name);
	// ����ͼ�ļ���
	void SetTexFile(const char* file_name);
	// �����ɫ�ļ���
	void SetBlendFile(const char* file_name);

	// ����ر�����ɫ
	static bool SaveBlendColors(const char* file_name, CTerrainZone* pZone);

private:
	CLightLoader(const CLightLoader&);
	CLightLoader& operator=(const CLightLoader&);

	// ���ع���λͼ�ļ�
	bool LoadBitmapFile(const char* file_name);
	// ���ع���ͼ�ļ�
	bool LoadDDSFile(const char* file_name);
	// ���ػ����ɫ�ļ�
	bool LoadBlendFile(const char* file_name);
	// ����Lightmap�����ļ�
	bool LoadConfigFile(const char* file_name);
private:
	CTerrainZone* m_pOwner;
	IRender* m_pRender;
	core_string m_strBitmap;
	core_string m_strTexFile;
	core_string m_strBlendFile;
	size_t m_nRows;
	size_t m_nCols;
	size_t m_nBmpWidth;
	size_t m_nBmpHeight;
	unsigned int* m_pBmpColors;
	size_t m_nBlendRows;
	size_t m_nBlendCols;
	unsigned int* m_pBlendColors;
	ITexture* m_pTex;
};

#endif // _LIGHT_LOADER_H
