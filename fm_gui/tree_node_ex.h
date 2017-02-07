//--------------------------------------------------------------------
// 文件名:		TreeNodeEx.h
// 内  容:		扩展的TreeNode
// 说  明:		
// 创建日期:	2011年3月7日
// 创建人:		马志攀
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TREENODEEX_H
#define _TREENODEEX_H

#include "../public/module.h"
#include "color_data.h"
#include "image_player.h"
#include "tree_node.h"
#include <vector>
#include <string>

// 树状列表节点

class TreeViewEx;

class TreeNodeEx: public TreeNode
{
private:
	enum { MIN_LEVEL_WIDTH = 0 };
	enum { MIN_ITEM_HEIGHT = 4 };

public:
	TreeNodeEx();
	virtual ~TreeNodeEx();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 创建子节点
	virtual PERSISTID CreateNode(const wchar_t* text);

	// 设置节点背景贴图
	void SetNodeBackImage(const char* value);
	const char* GetNodeBackImage() const;

	// 设置节点定位时的贴图
	void SetNodeFocusImage(const char* value);
	const char* GetNodeFocusImage() const;

	// 设置节点按下时的贴图
	void SetNodeSelectImage(const char* value);
	const char* GetNodeSelectImage() const;

	// 设置节点遮盖贴图
    void SetNodeCoverImage(const char* value);
    const char* GetNodeCoverImage();

	// 背景图片渲染模式
	void SetDrawModeString(const char* value);
	const char* GetDrawModeString() const;
	int GetDrawMode() const;

	// 文字起始X位移
	void SetTextOffsetX(int value);
	int GetTextOffsetX() const;

	// 文字起始Y位移
	void SetTextOffsetY(int value);
	int GetTextOffsetY() const;

	// 文字起始X位移
	void SetExpandCloseOffsetX(int value);
	int GetExpandCloseOffsetX() const;

	// 文字起始Y位移
	void SetExpandCloseOffsetY(int value);
	int GetExpandCloseOffsetY() const;

	// 文字起始X位移
	void SetNodeImageOffsetX(int value);
	int GetNodeImageOffsetX() const;

	// 文字起始Y位移
	void SetNodeImageOffsetY(int value);
	int GetNodeImageOffsetY() const;

	// 行高度
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// 与上一级级之间的宽度间距
	void SetLevelWidth(int value);
	int GetLevelWidth() const;


	// 节点Y位置偏移
	void SetNodeOffsetY(int value);
	int GetNodeOffsetY() const;

	CImagePlayer& GetNodeBackImagePlayer()   { return m_NodeBackImage;   }
	CImagePlayer& GetNodeFocusImagePlayer()  { return m_NodeFocusImage;  }
	CImagePlayer& GetNodeSelectImagePlayer() { return m_NodeSelectImage; }
	CImagePlayer& GetNodeCoverImagePlayer()  { return m_NodeCoverImage;  }
	
private:
	int m_nItemHeight;
	int m_nLevelWidth;
	int m_nTextOffsetX;
	int m_nTextOffsetY;
	int m_nExpandCloseOffsetX;
	int m_nExpandCloseOffsetY;
	int m_nNodeOffsetY;
	int m_nDrawMode;
	int m_nNodeImageOffsetX;
	int m_nNodeImageOffsetY;

	CImagePlayer m_NodeBackImage;
	CImagePlayer m_NodeFocusImage;
	CImagePlayer m_NodeSelectImage;
	CImagePlayer m_NodeCoverImage;
};

#endif // _TREENODEEX_H
