//--------------------------------------------------------------------
// 文件名:		light_processor.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#ifndef _LIGHT_PROCESSOR_H
#define _LIGHT_PROCESSOR_H

#include "../public/macros.h"
#include "light_types.h"

// 光照处理

class Lighter;
class CLightGeometry;
class CLightSet;
class CLightOctTree;

class CLightProcessor  
{
public:
	CLightProcessor();
	~CLightProcessor();
	
	// 初始化
	void Init(Lighter* lighter, CLightGeometry* g, CLightSet* lit, 
		CLightOctTree* ot);
	// 更新灯光参数
	void UpdateLightsParam();
	// 处理光照
	void Process();

	// 当前进度
	int GetShadowTriangleCount() const { return m_nShadowTriangleCount; }
	int GetSmoothVertexCount() const { return m_nSmoothVertexCount; }
	int GetBlurVertexCount() const { return m_nBlurVertexCount; }

private:
	// 计算投影
	void CalcShadows();
	// 平滑阴影
	void SmoothShadows();
	// 模糊阴影
	void BlurLight();
	// 计算光照结果
	void CalcLights(bool is_cos = true, bool is_att = true, 
		bool is_shadow = true);
	// 三角面投影
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
