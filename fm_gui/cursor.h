//--------------------------------------------------------------------
// 文件名:		cursor.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CURSOR_H
#define _CURSOR_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"


// 鼠标外观

class IPainter;

class Cursor: public IEntity
{
public:
	// 鼠标状态
	enum CURSOR_STATE
	{
		STATE_NORMAL,		// 未按下
		STATE_LEFTDOWN,		// 左键按下
		STATE_RIGHTDOWN,	// 右键按下
		STATE_MAX,
	};

	// 鼠标状态动画信息
	struct image_info_t
	{
		core_string strName;
		PERSISTID TexId;
		float fInterval;
	};

public:
	Cursor();
	virtual ~Cursor();
	
	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// 名字
	void SetName(const char* value);
	const char* GetName() const;
	unsigned int GetHash() const;
	
	// 贴图
	void SetImage(const char* value);
	const char* GetImage() const;

	// 是否是系统鼠标
	bool GetIsWinCursor() const;

	// 鼠标焦点
	void SetHotSpotX(int value);
	int GetHotSpotX() const;
	void SetHotSpotY(int value);
	int GetHotSpotY() const;

	// 获得尺寸
	int GetWidth() const;
	int GetHeight() const;
	
	// 动画状态
	bool SetState(int value);
	int GetState() const;

	// 状态动画贴图
	bool AddStateImage(int state, const char* image, float interval);
	// 创建
	bool Create();
	// 是否就绪
	bool IsReady() const;
	// 重置
	void Reset();
	// 刷新
	void Flash(float time);
	// 应用此鼠标外观
	bool Apply();
	// 显示
	bool Show();
	// 隐藏
	bool Hide();

    // 清理，并不是删除
    bool Clear();

private:
	IPainter* m_pPainter;
	core_string m_strName;
	core_string m_strImage;
	//HCURSOR m_hWinCursor;
	//HCURSOR m_hOldCursor;
	PERSISTID m_TexId;
	unsigned int m_nHash;
	int m_nHotSpotX;
	int m_nHotSpotY;
	int m_nState;
	int m_nStateImage;
	float m_fTimeCount;
	TArrayPod<image_info_t*, 1, TCoreAlloc> m_StateImages[STATE_MAX];
};

#endif // _CURSOR_H

