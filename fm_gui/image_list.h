//--------------------------------------------------------------------
// 文件名:		image_list.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月22日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _IMAGE_LIST_H
#define _IMAGE_LIST_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "image_player.h"

// 图元集合

class IPainter;
class Gui;

class ImageList: public IEntity
{
public:
	ImageList();
	virtual ~ImageList();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 添加图元
	bool AddImage(const char* name);
	// 删除图元
	bool RemoveImage(const char* name);
	// 清除图元
	bool ClearImage();
	// 获得图元数量
	int GetImageCount() const;
	// 获得图元名字列表
	void GetImageNameList(const IVarList& args, IVarList& result) const;
	// 获得指定位置图元
	CImagePlayer* GetImageByIndex(size_t index);

private:
	IPainter* m_pPainter;
	Gui* m_pGui;
	TArrayPod<CImagePlayer*, 1, TCoreAlloc> m_Images;
};

#endif // _IMAGE_LIST_H
