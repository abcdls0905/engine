//--------------------------------------------------------------------
// 文件名:		ImageData.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _IMAGEDATA_H
#define _IMAGEDATA_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"

// 动画图元数据

class CImageFlip
{
public:
	struct sub_image_t
	{
		sub_image_t(){}
		float fSeconds;
		int nLeft;
		int nTop;
		int nRight;
		int nBottom;
	};
	
	typedef TArrayPod<sub_image_t, 1, TCoreAlloc> SUB_CONTAINER;

public:
	SUB_CONTAINER m_SubImages;
	float m_fCounter;
	int m_nCurrent;
};

// 界面图元数据

class CImageData
{
public:
	CImageData();
	~CImageData();

	// 是否包含图元动画
	bool IsFlip() const { return (m_pFlip != NULL); }

	// 复制
	void Copy(const CImageData& src);
	// 创建动画数据
	void CreateFlip();
	// 删除动画数据
	void DeleteFlip();
	// 复位动画
	void ResetFlip();
	// 播放动画
	void ExecuteFlip(float seconds);
	// 获得当前图元位置
	void GetCurrent(int& left, int& top, int& right, int& bottom);

private:
	CImageData(const CImageData&);
	CImageData& operator=(const CImageData&);

public:
	core_string m_strTex;
	int m_nLeft;
	int m_nTop;
	int m_nRight;
	int m_nBottom;
	CImageFlip* m_pFlip;
};

#endif // _IMAGEDATA_H

