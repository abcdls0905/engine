//--------------------------------------------------------------------
// 文件名:		balloon.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月21日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BALLOON_H
#define _BALLOON_H

#include "../public/core_type.h"
#include "../public/module.h"

// 气球

class IControl;

class Balloon: public IEntity
{
public:
	Balloon();
	virtual ~Balloon();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 是否加载完成
	bool GetLoadFinish();

    // 名称
	void SetName(const char* val);
	const char* GetName() const;

	// 前一个节点
	void SetPrevNode(Balloon* node) { m_pPrev = node; }
	Balloon* GetPrevNode() const { return m_pPrev; }
	
	// 后一个节点
	void SetNextNode(Balloon* node) { m_pNext = node; }
	Balloon* GetNextNode() const { return m_pNext; }
	
	// 在指定节点之后插入
	void AttachTo(Balloon* link_after, Balloon** first, Balloon** last);
	// 插入节点
	void Attach(Balloon** first, Balloon** last);
	// 移出节点
	void Detach(Balloon** first, Balloon** last);
	// 排序以保证渲染的顺序
	inline bool ProcessOrder(Balloon** first, Balloon** last)
    {
	    if (m_pNext) 
	    {
		    // 远处的先画
		    if (m_fDepth < m_pNext->m_fDepth)
		    {
			    Detach(first, last);
			    AttachTo(m_pNext, first, last);
			    return true;
		    }
	    }

	    return false;
    }
	// 控件对象
	void SetControl(IControl* pControl);
	IControl* GetControl() const;
	PERSISTID GetControlID() const;

	// 绑定的模型对象
	void SetModelID(const PERSISTID& value, const char* szNodeName = 0);
	PERSISTID GetModelID() const;
	const char* GetNodeName()const;

	// Z深度
	void SetDepth(float value);
	float GetDepth() const;

	// 绑定位置
	void SetBindX(float value);
	float GetBindX() const;
	void SetBindY(float value);
	float GetBindY() const;
	void SetBindZ(float value);
	float GetBindZ() const;

	// 控件偏移
	void SetOffsetLeft(float value);
	float GetOffsetLeft() const;
	void SetOffsetTop(float value);
	float GetOffsetTop() const;

	// 距离范围
	void SetNearDistance(float value);
	float GetNearDistance() const;
	void SetFarDistance(float value);
	float GetFarDistance() const;

	// 是否强制显示
	void SetMustVisible(bool value);
	bool GetMustVisible() const;
	
    // 设置汽球类型
    bool SetType(int type);
    int GetType();

	bool IsTemped() {return m_bTemped;}
	void SetTemped(bool value)
	{
		m_bTemped = value;
	}
private:
	Balloon* m_pPrev;
	Balloon* m_pNext;
    PERSISTID m_ControlID;  // 控件
	PERSISTID m_ModelID;	// 模型对象
	float m_fDepth;
	float m_fBindX;			// 绑定点的相对位置
	float m_fBindY;
	float m_fBindZ;
	float m_fOffsetLeft;	// 控件中心点与绑定点的相对偏移
	float m_fOffsetTop;
	float m_fNearDistance;	// 最近距离
	float m_fFarDistance;	// 最远距离
    int m_nType;            // 类型
	bool m_bMustVisible;	// 强制显示
	bool m_bTemped;			// 是否已删除,被缓存了
	core_string m_strName;
	core_string m_strNodeName;//模型对象的挂接点
};

#endif // _BALLOON_H

