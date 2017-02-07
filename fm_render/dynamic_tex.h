//--------------------------------------------------------------------
// �ļ���:		dynamic_tex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��8��11��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DYNAMIC_TEX_H
#define _DYNAMIC_TEX_H



#include "../visual/i_resource.h"
#include "../public/core_mem.h"
// ��̬�仯����ͼ
#include "render.h"
class Render;

class CDynamicTex: public IDynamicTex
{
	int m_PixelFormat;
	int m_InternalPixelFormat;
	int m_size;
	GLenum m_BufferType;
#ifdef PROCESS_LOST_DEVICE
	void* m_pdata;
	void Blit(void* dst_data, int dx, int dy, const void* src_data, int src_width, int src_height);
#endif
public:
	CDynamicTex(Render* pRender);
	virtual ~CDynamicTex();

	// �ͷ�
	virtual void Release();

	// ����
	bool Create(unsigned int width, unsigned int height, unsigned int levels, 
		TEX_FORMAT format);

	// ���
	unsigned int GetWidth() const { return m_nWidth; }
	// �߶�
	unsigned int GetHeight() const { return m_nHeight; }
	// ��ʽ
	TEX_FORMAT GetFormat() const { return m_nFormat; }

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }

		// ��ȡ��ͼ����������
	ITextureSampler* GetTextureSampler() const 
	{
		return m_pTexSampler;
	}

	bool Update(int level, unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata);
private:
	CDynamicTex();
	CDynamicTex(const CDynamicTex&);
	CDynamicTex& operator=(const CDynamicTex&);
	
	// ����
	virtual void Destroy();

#ifdef PROCESS_LOST_DEVICE
	// �豸����ʱ�Ĵ���
	virtual bool Restore();
	// �豸��ʧʱ�Ĵ���
	virtual bool Invalidate();
#endif

private:
	Render* m_pRender;
	ITextureSampler* m_pTexSampler;
	int m_nIndex;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	TEX_FORMAT m_nFormat;
};

#endif // _DYNAMIC_TEX_H
