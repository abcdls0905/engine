//--------------------------------------------------------------------
// �ļ���:		int_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��8��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _INT_MANAGER_H
#define _INT_MANAGER_H

#include "../public/macros.h"
#include "../utils/array_pod.h"

// ���ܽӿڹ���

class IInterface;
class IIntCreator;
class CCore;

class CIntManager
{
private:
	struct int_creator_t
	{
		IIntCreator* pCreator;
		unsigned int nHash;
		IInterface* pInterface;
		int nRefCount;
		int64_t nOrder;
	};

	// ����ȽϺ���
	static int SortCompare(const void* elem1, const void* elem2);
	
public:
	explicit CIntManager(CCore* pCore);
	~CIntManager();

	// �ͷ����ж��ܽӿ�
	void ReleaseAll();

	// ��Ӵ�����
	bool AddCreator(IIntCreator* pCreator);

	// ����
	bool Find(const char* name) const;
	bool FindIndex(const char* name, size_t& index) const;
	// �������
	size_t GetCount() const;
	// �������
	const char* GetName(size_t index) const;
	// ��ô�����
	IIntCreator* GetCreator(size_t index) const;
	// ��ýӿڣ�����ΪNULL��
	IInterface* GetInterface(size_t index) const;
	// ��ýӿڵ�������
	int GetRefCount(size_t index) const;
	
	// �����ӿڻ����ӽӿڵ�����
	IInterface* GetReference(const char* name);
	// �ͷŽӿ�
	void ReleaseReference(IInterface* pInterface);

	// ������Ҫÿ֡���еĽӿں���
	void ExecuteFrameBegin();
	void ExecuteFrameEnd();

	// �������й��ܽӿ���Ϣ���ļ�
	bool Dump(const char* file_name);
	
private:
	CIntManager();
	CIntManager(const CIntManager&);
	CIntManager& operator=(const CIntManager&);

	bool AddFrameExecute(IInterface* pInterface);
	bool RemoveFrameExecute(IInterface* pInterface);

private:
	CCore* m_pCore;
	TArrayPod<int_creator_t, 1> m_IntCreators;
	TArrayPod<IInterface*, 1> m_FrameExecute;
	int64_t m_nOrderCount;
};

#endif // _INT_MANAGER_H
