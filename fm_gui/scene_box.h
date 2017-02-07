//--------------------------------------------------------------------
// 文件名:		SceneBox.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月31日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SCENEBOX_H
#define _SCENEBOX_H

#include "i_control.h"

// SceneBox
// 嵌入场景框

class SceneBox: public IControl
{
public:
	SceneBox();
	virtual ~SceneBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual int GetMinWidth() const { return 16; }
	virtual int GetMinHeight() const { return 16; }
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnRButtonDown(int x, int y);
	virtual int OnRButtonUp(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnDragLeave(int x, int y);
	virtual int OnMouseMove(int x, int y);
	
	virtual bool TestTransparent(int x, int y);

	// 嵌入场景对象
	void SetSceneID(const PERSISTID& id);
	PERSISTID GetSceneID() const;

    //// 透明
    //void SetTransparent(bool bTrans);
    //bool GetTransparent();

	// 获得上一次的鼠标位置
	int GetOldX() const;
	int GetOldY() const;

private:
	// 获得实际在显示设备上的鼠标位置
	int GetRealDeviceX(int x);
	int GetRealDeviceY(int y);

private:
	PERSISTID m_SceneID;
    //bool m_bTransparent;
	int m_nOldX;
	int m_nOldY;
};

#endif // _SCENEBOX_H
