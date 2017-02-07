//--------------------------------------------------------------------
// �ļ���:		image_list.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��22��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _IMAGE_LIST_H
#define _IMAGE_LIST_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "image_player.h"

// ͼԪ����

class IPainter;
class Gui;

class ImageList: public IEntity
{
public:
	ImageList();
	virtual ~ImageList();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ���ͼԪ
	bool AddImage(const char* name);
	// ɾ��ͼԪ
	bool RemoveImage(const char* name);
	// ���ͼԪ
	bool ClearImage();
	// ���ͼԪ����
	int GetImageCount() const;
	// ���ͼԪ�����б�
	void GetImageNameList(const IVarList& args, IVarList& result) const;
	// ���ָ��λ��ͼԪ
	CImagePlayer* GetImageByIndex(size_t index);

private:
	IPainter* m_pPainter;
	Gui* m_pGui;
	TArrayPod<CImagePlayer*, 1, TCoreAlloc> m_Images;
};

#endif // _IMAGE_LIST_H
