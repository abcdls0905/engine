//--------------------------------------------------------------------
// 文件名:		EllipseProgressBar.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年11月5日
// 创建人:		单悦
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ELLIPSEPROGRESSBAR_H
#define _ELLIPSEPROGRESSBAR_H

#include "progress_bar.h"
#include <string>

// 进度条

class EllipseProgressBar: public ProgressBar
{
public:
	EllipseProgressBar();
	virtual ~EllipseProgressBar();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	// 进度条头贴图
	void SetProgressLabImage(const char* value);
	std::string GetProgressLabImage() const;

	virtual void SetProgressImage(const char* value);
	// 
	void SetBeginDelta(int val);
	int GetBeginDelta() const;

	void SetEndDelta(int val);
	int GetEndDelta() const;

	float GetDelta( );
protected:
	PERSISTID m_TexId;
	int m_nBeginDelta;
	int m_nEndDelta;
};

#endif // _ELLIPSEPROGRESSBAR_H
