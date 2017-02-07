//--------------------------------------------------------------------
// �ļ���:		helper.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��7��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _HELPER_H
#define _HELPER_H

#include "../public/macros.h"
#include "../visual/fm_define.h"
 

// ��������

// ������׶������
bool Helper_MakeViewPlanes(const FmMat4& view, 
	const FmMat4& proj, FmPlane* planes, size_t plane_num);
// ��Ļ��ͼ
bool Helper_ScreenShot(const char* name);
// �����ͼָ��λ�õ�ALPHAֵ
bool Helper_GetTextureAlpha(fm_uint nTex, unsigned int x, unsigned int y, 
	unsigned int& alpha);

#endif // _HELPER_H
