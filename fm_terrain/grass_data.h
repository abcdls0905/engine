//--------------------------------------------------------------------
// 文件名:		grass_data.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年12月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GRASS_DATA_H
#define _GRASS_DATA_H

// 花草数据

struct grass_data_t
{
	float fPosiX;				// 位置
	float fPosiZ;
	unsigned int nGrassIndex;	// 花草索引
	unsigned int nScale;		// 范围(1-10)
	unsigned int nAmount;		// 数量(1-15)
	unsigned int nSeed;			// 随机种子
};
	
#endif // _GRASS_DATA_H
