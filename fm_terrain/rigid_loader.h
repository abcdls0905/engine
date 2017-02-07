//--------------------------------------------------------------------
// �ļ���:		rigid_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��4��8��
// ������:		��Ң˳
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _RIGID_LOADER_H
#define _RIGID_LOADER_H

// ��������������Ҫ��ͷ�ļ�
//#include "../physics/physics_macros.h"

#ifdef PHYSICS_HAVOK_TERRAIN

#include "../public/macros.h"
#include "../visual/dx_header.h"
#include "../visual/i_res_loader.h"

// ���θ��������

class CTerrainZone;

class CRigidLoader: public IResLoader
{
public:
	static CRigidLoader* NewInstance();

public:
	CRigidLoader();
	virtual ~CRigidLoader();

	// �ͷ�
	virtual void Release();
	// �������
	virtual const char* GetName();
	// ����
	virtual bool Load(bool async);
	// ����
	virtual bool Create(bool async);
	// �����첽���� 
	virtual bool EndCreate(int category, bool succeed);
	
	// ��Դ����
	void SetOwner(CTerrainZone* pOwner);
	// ��¼�߶���Ϣ
	void SetHeights(float* heights);
	
private:
	CRigidLoader(const CRigidLoader&);
	CRigidLoader& operator=(const CRigidLoader&);

private:
	CTerrainZone* m_pOwner;
	float* m_pHeights;
};
#endif // PHYSICS_HAVOK_TERRAIN

#endif // _RIGID_LOADER_H
