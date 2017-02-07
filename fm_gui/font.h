//--------------------------------------------------------------------
// 文件名:		font.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _FONT_H
#define _FONT_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../math/fm_math.h"
// 字体实体

class IPainter;

class Font: public IEntity
{
private:
	typedef TArrayPod<int, 1, TCoreAlloc> char_container_t;

public:
	Font();
	virtual ~Font();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 名字
	void SetName(const char* value);
	const char* GetName() const;
	unsigned int GetHash() const;

	// 系统字体名
	//void SetSysFont(const char* value);
	//const char* GetSysFont() const;

	// 字体文件名
	void SetFontFile(const char* value);
	const char* GetFontFile() const;

	// 字体贴图名
	void SetFontTex(const char* value);
	const char* GetFontTex() const;

	// 贴图宽度
	void SetTexWidth(int value);
	int GetTexWidth() const;

	// 贴图高度
	void SetTexHeight(int value);
	int GetTexHeight() const;
	
	// 高度
	void SetHeight(int value);
	int GetHeight() const;

	// 间隔
	void SetInterval(int value);
	int GetInterval() const;

	// 粗体
	void SetBold(bool value);
	bool GetBold() const;

	// 平滑
	void SetSmooth(bool value);
	bool GetSmooth() const;

	// 斜体
	void SetItalic(bool value);
	bool GetItalic() const;

	// 下划线
	void SetUnderline(bool value);
	bool GetUnderline() const;

	// 阴影
	void SetStrikeOut(bool value);
	bool GetStrikeOut() const;

	// 是否可变尺寸
	void SetVarExtent(bool value);
	bool GetVarExtent() const;

	// 是否可变尺寸
	void SetOutline(bool value);
	bool GetOutline() const;
	void SetOutlineColor(const char* value);
	void SetOutlineNum(int value);

	// 阴影颜色
	void SetShadowColorString(const char* value);
	result_string GetShadowColorString() const;

	// 阴影距离
	void SetShadowOffsetX(int value);
	int GetShadowOffsetX() const;
	void SetShadowOffsetY(int value);
	int GetShadowOffsetY() const;

	void SetFadeType(int type);

	// 添加贴图字体信息
	bool AddTexCharInfo(int ch, int left, int top, int right, int bottom);

	// 判断是否完整路径：
	static bool CheckFullPath(const char* path);

	// 创建
	bool Create();

private:
	IPainter* m_pPainter;
	core_string m_strName;
	unsigned int m_nHash;
	//core_string m_strSysFont;
	core_string m_strFontFile;
	core_string m_strFontTex;
	int m_nTexWidth;
	int m_nTexHeight;
	int m_nHeight;
	int m_nInterval;
	bool m_bBold;
	bool m_bSmooth;
	bool m_bItalic;
	bool m_bUnderline;
	bool m_bStrikeOut;
	bool m_bVarExtent;
    bool m_bOutline;
	unsigned int m_nShadowColor;
	int m_nShadowOffsetX;
	int m_nShadowOffsetY;
	FmVec3 m_OutlineColor;
	int m_nOutlineNum;
	int m_nFadeType;
	char_container_t m_CharInfos;
};

#endif // _FONT_H

