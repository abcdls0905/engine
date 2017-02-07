//--------------------------------------------------------------------
// 文件名:		whip_data.h
// 内  容:		
// 说  明:		
// 创建日期:	2014年4月11日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WHIP_DATA_H
#define _WHIP_DATA_H

//--------------------------------------------------------------------

// 鞭子数据
#include "../math/fm_math.h"
#include "../public/core_mem.h"
#include <stdio.h>

//--------------------------------------------------------------------

#define WHIP_MAX_VERTICES_COUNT     512     // 鞭子上的最多顶点数目

//--------------------------------------------------------------------

// 顶点格式
struct whip_vertex_t
{
    FmVec3 pos;
    float tu;
    float tv;

    void Set(float x, float y, float z, float u, float v)
    {
        pos.x = x;
        pos.y = y;
        pos.z = z;
        tu = u;
        tv = v;
    }
};

//--------------------------------------------------------------------

// 鞭子基类
class WhipData
{
public:
    friend class Whip;

    WhipData();

    virtual ~WhipData();

    virtual void Init(unsigned int slices) {};

    virtual void Update(float deltaTime, const FmVec3& vEyePos) {};

protected:
	float m_fInTime;            // 进入时间
    float m_fPauseTime;         // 暂停时间
    float m_fOutTime;           // 退出时间
    float m_fTime;              // 当前时间
    float m_uScale;             // U方向的纹理坐标缩放
    float m_fBladeWidth;
    unsigned int m_slicesCount;
    FmVec4 m_params;            // 其他参数

    FmVec3 m_vAttackPosition;
    FmVec3 m_vSufferPosition;
    FmVec3 m_vTailPosition;
    FmVec3 m_vLeft;

	whip_vertex_t* m_verticesBuffer;
    unsigned int m_verticesCount;
};

//--------------------------------------------------------------------

// 绳索
class RopeData : public WhipData
{
public:
    void Init(unsigned int slices);

    void Update(float deltaTime, const FmVec3& vEyePos);
};

//--------------------------------------------------------------------

// Sin曲线的鞭子
class SinWhipData : public WhipData
{
public:
    SinWhipData();

    ~SinWhipData();

    void Init(unsigned int slices);

    void Update(float deltaTime, const FmVec3& vEyePos);

protected:
    float* m_list_cos_0_pi;
    float* m_list_sin_0_2pi;
    FmVec3* m_list_orbit_vertices;
};

//--------------------------------------------------------------------

// Circle曲线的鞭子
class CircleWhipData : public WhipData
{
public:
    CircleWhipData();

    ~CircleWhipData();

    void Init(unsigned int slices);

    void Update(float deltaTime, const FmVec3& vEyePos);

protected:
    float* m_list_cos_0_pi;
    float* m_list_sin_0_pi;
    FmVec3* m_list_orbit_vertices;
};

//--------------------------------------------------------------------

// Sin曲线的鞭子2
class SinWhipData2 : public SinWhipData
{
public:
    void Update(float deltaTime, const FmVec3& vEyePos);
};

//--------------------------------------------------------------------

// Circle曲线的鞭子2
class CircleWhipData2 : public CircleWhipData
{
public:
    void Update(float deltaTime, const FmVec3& vEyePos);
};

//--------------------------------------------------------------------

// 圆形轨迹的刀光
class CircleSaberData : public WhipData
{
public:
    CircleSaberData();

    ~CircleSaberData();

    void Init(unsigned int slices);

    void Update(float deltaTime, const FmVec3& vEyePos);

protected:
    FmVec2* m_list_sin_cos_0_pi;
};

//--------------------------------------------------------------------

// 半圆形轨迹的刀光
class SimiCircleSaberData : public CircleSaberData
{
public:
    void Update(float deltaTime, const FmVec3& vEyePos);
};

//--------------------------------------------------------------------

#endif // _WHIP_DATA_H
