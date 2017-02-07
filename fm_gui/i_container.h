//--------------------------------------------------------------------
// �ļ���:		i_container.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _I_CONTAINER_H
#define _I_CONTAINER_H

#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "i_control.h"

// �ؼ���������

class IContainer: public IControl
{
private:
	typedef TArrayPod<IControl*, 1, TCoreAlloc> child_container_t;
	typedef TArrayPod<PERSISTID, 1, TCoreAlloc> top_container_t;
	
public:
	IContainer();
	virtual ~IContainer() = 0;

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ���ƿؼ�
	virtual void Paint(float seconds);
	
	// �����������Ϣ
	virtual void PrepareDesignInfo(IDesignInfo* di);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	// �Ƿ�����
	virtual bool IsContainer() const;
	// �ÿؼ�������
	virtual IControl* FindChild(const char* name) const;
	// �ÿؼ����ӱ��ڵ㿪ʼ���еݹ���ң��������ڵ�
	virtual IControl* QueryControl(const char* name);
	// ����ӿؼ�
	virtual bool AddChild(IControl* child);
	// ��ָ���ؼ�֮�����
	virtual bool InsertAfterChild(IControl* child, IControl* after);
	// ��ָ���ؼ�֮ǰ����
	virtual bool InsertBeforeChild(IControl* child, IControl* before);
	// �Ƴ��ӿؼ�
	virtual bool RemoveChild(IControl* child);
    virtual bool RemoveChildNoEvent(IControl* child);
	// ����ӿؼ�
	virtual void ClearChild();
	// ɾ�������ӿؼ�
	virtual void DeleteAllChild();
	// ���ӿؼ��ŵ����ZOrder��
	virtual bool SendToBack(IControl* child);
	// ���ӿؼ��ŵ���ǰ��ZOrder��
	virtual bool BringToFront(IControl* child);
	// �Ƿ��ӿؼ�
	virtual bool IsChildControl(IControl* child) const;
	// ����ӿؼ�����
	virtual size_t GetChildCount() const;
	// ���ָ��λ�õ��ӿؼ�
	virtual IControl* GetChildByIndex(size_t index) const;
	// ����ӿؼ�ID�б�
	virtual size_t GetChildList(IVarList& result) const;
	
	// ��Ҫ��ʾ�����ϲ�Ŀؼ�
	virtual bool FindTopLayer(const PERSISTID& id);
	virtual bool AddTopLayer(const PERSISTID& id);
	virtual bool RemoveTopLayer(const PERSISTID& id);
	virtual void DeleteAllTopLayer();
	// ����ӿؼ�
	virtual int OnAdd(const PERSISTID& child_id);
	// �Ƴ��ӿؼ�
	virtual int OnRemove(const PERSISTID& child_id);

	// ���Ͻ�����ı�
	virtual int OnLeftChanged(int old);
	virtual int OnTopChanged(int old);
    // 
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);

	virtual IControl* OnGetInWhat(int x, int y);

	// �Ƿ�������/ɾ���ӿؼ����¼�
	void SetAddRemoveEvent(bool value);
	bool GetAddRemoveEvent() const;
	
	// ����ӿؼ�
	bool Add(const PERSISTID& child);
	// �����ӿؼ�����ָ���ؼ�֮��
	bool InsertAfter(const PERSISTID& child, const PERSISTID& after);
	// �����ӿؼ�����ָ���ؼ�֮ǰ��
	bool InsertBefore(const PERSISTID& child, const PERSISTID& before);
	// �Ƴ��ӿؼ�
	bool Remove(const PERSISTID& child);
	// ɾ�������ӿؼ�
	bool DeleteAll();
	// ����ָ�������ӿؼ�
	PERSISTID Find(const char* name) const;
	// ���ӿؼ��ƶ���ǰ
	bool ToFront(const PERSISTID& child);
	// ���ӿؼ��ƶ����
	bool ToBack(const PERSISTID& child);
	// �Ƿ��ӿؼ�
	bool IsChild(const PERSISTID& id) const;
	
    // ��ȡ�����ӿؼ��б�
    int GetTopControlList(const IVarList& param, IVarList& result);
    // LUA��ȡ����������ֱ���ӿؼ�
    int GetChildControlList(const IVarList& param, IVarList& result);

protected:
    virtual bool RefreshInnerAbsPos();

private:
	void InitChild(IControl* child);
	void PutChild(IControl* child);

protected:
	void PaintTopLayer(float seconds);
    void RefreshChildren();

	top_container_t m_TopLayer;

private:
	bool m_bAddRemoveEvent;
	child_container_t m_Children;
};

#endif // _I_CONTAINER_H

