//--------------------------------------------------------------------
// �ļ���:		light_processor.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��11��5��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
/*
#ifndef _LIGHT_PROCESSOR_H
#define _LIGHT_PROCESSOR_H

#include "../public/macros.h"
#include "light_types.h"

// ���մ���

class Lighter;
class CLightGeometry;
class CLightSet;
class CLightOctTree;

class CLightProcessor  
{
public:
	CLightProcessor();
	~CLightProcessor();
	
	// ��ʼ��
	void Init(Lighter* lighter, CLightGeometry* g, CLightSet* lit, 
		CLightOctTree* ot);
	// ���µƹ����
	void UpdateLightsParam();
	// �������
	void Process();

	// ��ǰ����
	int GetShadowTriangleCount() const { return m_nShadowTriangleCount; }
	int GetSmoothVertexCount() const { return m_nSmoothVertexCount; }
	int GetBlurVertexCount() const { return m_nBlurVertexCount; }

private:
	// ����ͶӰ
	void CalcShadows();
	// ƽ����Ӱ
	void SmoothShadows();
	// ģ����Ӱ
	void BlurLight();
	// ������ս��
	void CalcLights(bool is_cos = true, bool is_att = true, 
		bool is_shadow = true);
	// ������ͶӰ
	void ApplyTriangleShadows(triangle_t& t);

private:
	Lighter* m_pLighter;
	CLightGeometry* m_pGeometry;
	CLightSet* m_pLights;
	CLightOctTree* m_pOctTree;
	int m_nShadowTriangleCount;
	int m_nSmoothVertexCount;
	int m_nBlurVertexCount;
};

#endif // _LIGHT_PROCESSOR_H
*/
