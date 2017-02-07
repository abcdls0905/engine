//--------------------------------------------------------------------
// �ļ���:		i_texture.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��1��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_TEXTURE_H
#define _VISUAL_I_TEXTURE_H

#include "../public/persistid.h"
#include "i_res_base.h"

// ��ͼ��Դ

class IShaderTex;

class ITexture: public IResBase
{
public:
	// �����
	virtual PERSISTID GetID() = 0;
	// �������
	virtual const char* GetName() = 0;
	// �Ƿ����
	virtual bool IsReady() = 0;
	// �Ƿ���ؽ������ɹ���ʧ�ܣ�
	virtual bool IsLoadComplete() = 0;
	// ��ͼ���
	virtual unsigned int GetWidth() = 0;
	// ��ͼ�߶�
	virtual unsigned int GetHeight() = 0;
	// ��ͼʵ�ʿ��
	virtual unsigned int GetOriginWidth() = 0;
	// ��ͼʵ�ʸ߶�
	virtual unsigned int GetOriginHeight() = 0;
	// ��ͼ��ʽ
	virtual unsigned int GetFormat() = 0;
	// �����ͼ
	virtual IShaderTex* GetShaderTex() = 0;
	// ��ȡ������ͼ(�˽ӿ�Ϊ��ģ��D3D�����ÿ�����ʽ,����Ĭ��1X1��һ����ʱ������Ϊ��������)
	virtual IShaderTex* GetCanUseShaderTex() = 0;
};

#endif // _VISUAL_I_TEXTURE_H
