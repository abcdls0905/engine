//--------------------------------------------------------------------
// �ļ���:		TreeNodeEx.h
// ��  ��:		��չ��TreeNode
// ˵  ��:		
// ��������:	2011��3��7��
// ������:		��־��
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TREENODEEX_H
#define _TREENODEEX_H

#include "../public/module.h"
#include "color_data.h"
#include "image_player.h"
#include "tree_node.h"
#include <vector>
#include <string>

// ��״�б�ڵ�

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

	// �����ӽڵ�
	virtual PERSISTID CreateNode(const wchar_t* text);

	// ���ýڵ㱳����ͼ
	void SetNodeBackImage(const char* value);
	const char* GetNodeBackImage() const;

	// ���ýڵ㶨λʱ����ͼ
	void SetNodeFocusImage(const char* value);
	const char* GetNodeFocusImage() const;

	// ���ýڵ㰴��ʱ����ͼ
	void SetNodeSelectImage(const char* value);
	const char* GetNodeSelectImage() const;

	// ���ýڵ��ڸ���ͼ
    void SetNodeCoverImage(const char* value);
    const char* GetNodeCoverImage();

	// ����ͼƬ��Ⱦģʽ
	void SetDrawModeString(const char* value);
	const char* GetDrawModeString() const;
	int GetDrawMode() const;

	// ������ʼXλ��
	void SetTextOffsetX(int value);
	int GetTextOffsetX() const;

	// ������ʼYλ��
	void SetTextOffsetY(int value);
	int GetTextOffsetY() const;

	// ������ʼXλ��
	void SetExpandCloseOffsetX(int value);
	int GetExpandCloseOffsetX() const;

	// ������ʼYλ��
	void SetExpandCloseOffsetY(int value);
	int GetExpandCloseOffsetY() const;

	// ������ʼXλ��
	void SetNodeImageOffsetX(int value);
	int GetNodeImageOffsetX() const;

	// ������ʼYλ��
	void SetNodeImageOffsetY(int value);
	int GetNodeImageOffsetY() const;

	// �и߶�
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// ����һ����֮��Ŀ�ȼ��
	void SetLevelWidth(int value);
	int GetLevelWidth() const;


	// �ڵ�Yλ��ƫ��
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
