//--------------------------------------------------------------------
// �ļ���:		Pyramid.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��8��6��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __PYRAMID_H__
#define __PYRAMID_H__

#include "../visual/i_vis_base.h"
#include "../visual/i_render.h"
#include <vector>

class ICamera;

// ��׶��

// ��11���ߣ��ֱ�����׶�Ĳ����4���ߡ�Ŀ������ƽ���4���ߺ�ָ��Ŀ�����,����Ŀ���Ľ�����
#define PYRAMID_LINE_COUNT 11

class Pyramid
{
private:
	struct vertex_t 
	{
		FmVec3 pos;
		unsigned int color;
	};

public:
	Pyramid();
	virtual ~Pyramid() {}

	virtual bool Update();
	virtual void Realize(IRender* pRender);

	// �����
	void SetCamera(const PERSISTID& value);
	PERSISTID GetCamera() const;

	// �Ƿ���ʾ��׶
	void SetShowPyramid(bool value) { m_bShowPyramid = value; }
	bool GetShowPyramid() const { return m_bShowPyramid; }

	// y������Ұ�Ƕ�
	void SetFovy(float value);
	float GetFovy() const { return m_fFovy; }
	
	// �ӿڿ�߱�
	void SetAspect(float value);
	float GetAspect() const { return m_fAspect; }

	/*���ý���,�ڴ�ֻ�Ǹı�����߿�ĳ��ȡ�
	ֻ�������������Ч��Ŀ����������߿򳤶��ɸ�Ŀ��ľ��������*/
	void SetFocusDepth(float value);
	float GetFocusDepth() const {return m_fDepth;}

private:
	ICamera* GetCamera(const PERSISTID& id) const;
	// ���¶���
	bool UpdateVertex();

private:
	PERSISTID	 m_camera;

	vertex_t	 m_vertexBuffer[PYRAMID_LINE_COUNT * 2];
	bool		 m_bShowPyramid;
	unsigned int m_uLineColor;

	float m_fFovy;
	float m_fAspect;
	float m_fDepth;
	FmMat4 m_mtxWorld;
};

#endif // __PYRAMID_H__
