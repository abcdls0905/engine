//--------------------------------------------------------------------
// 文件名:		helper.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年7月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _HELPER_H
#define _HELPER_H

#include "../public/macros.h"
#include "../visual/fm_define.h"
 

// 辅助函数

// 生成视锥剪裁面
bool Helper_MakeViewPlanes(const FmMat4& view, 
	const FmMat4& proj, FmPlane* planes, size_t plane_num);
// 屏幕截图
bool Helper_ScreenShot(const char* name);
// 获得贴图指定位置的ALPHA值
bool Helper_GetTextureAlpha(fm_uint nTex, unsigned int x, unsigned int y, 
	unsigned int& alpha);

#endif // _HELPER_H
