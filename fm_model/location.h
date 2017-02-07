//--------------------------------------------------------------------
// 文件名:		location.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月1日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef	_LOCATION_H
#define	_LOCATION_H



// 模型位置方向和缩放
struct location_t
{
	FmVec3 position;
	FmQuat rotate;
	FmVec3 scale;
};

#pragma pack(push, 1) 

// 压缩的欧拉角方向
struct location_cr_t
{
	unsigned short pitch;
	unsigned short yaw;
	unsigned short roll;
};

// 压缩的欧拉角方向和位置
struct location_ctr_t
{
	unsigned short pitch;
	unsigned short yaw;
	unsigned short roll;
	float px;
	float py;
	float pz;
};

// 压缩的欧拉角方向和缩放
struct location_csr_t
{
	unsigned short pitch;
	unsigned short yaw;
	unsigned short roll;
	float sx;
	float sy;
	float sz;
};

#pragma pack(pop)

#endif // _LOCATION_H
