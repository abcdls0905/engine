//--------------------------------------------------------------------
// �ļ���:		whip_data.h
// ��  ��:		
// ˵  ��:		
// ��������:	2014��4��11��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WHIP_DATA_H
#define _WHIP_DATA_H

//--------------------------------------------------------------------

// ��������
#include "../math/fm_math.h"
#include "../public/core_mem.h"
#include <stdio.h>

//--------------------------------------------------------------------

#define WHIP_MAX_VERTICES_COUNT     512     // �����ϵ���ඥ����Ŀ

//--------------------------------------------------------------------

// �����ʽ
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

// ���ӻ���
class WhipData
{
public:
    friend class Whip;

    WhipData();

    virtual ~WhipData();

    virtual void Init(unsigned int slices) {};

    virtual void Update(float deltaTime, const FmVec3& vEyePos) {};

protected:
	float m_fInTime;            // ����ʱ��
    float m_fPauseTime;         // ��ͣʱ��
    float m_fOutTime;           // �˳�ʱ��
    float m_fTime;              // ��ǰʱ��
    float m_uScale;             // U�����������������
    float m_fBladeWidth;
    unsigned int m_slicesCount;
    FmVec4 m_params;            // ��������

    FmVec3 m_vAttackPosition;
    FmVec3 m_vSufferPosition;
    FmVec3 m_vTailPosition;
    FmVec3 m_vLeft;

	whip_vertex_t* m_verticesBuffer;
    unsigned int m_verticesCount;
};

//--------------------------------------------------------------------

// ����
class RopeData : public WhipData
{
public:
    void Init(unsigned int slices);

    void Update(float deltaTime, const FmVec3& vEyePos);
};

//--------------------------------------------------------------------

// Sin���ߵı���
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

// Circle���ߵı���
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

// Sin���ߵı���2
class SinWhipData2 : public SinWhipData
{
public:
    void Update(float deltaTime, const FmVec3& vEyePos);
};

//--------------------------------------------------------------------

// Circle���ߵı���2
class CircleWhipData2 : public CircleWhipData
{
public:
    void Update(float deltaTime, const FmVec3& vEyePos);
};

//--------------------------------------------------------------------

// Բ�ι켣�ĵ���
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

// ��Բ�ι켣�ĵ���
class SimiCircleSaberData : public CircleSaberData
{
public:
    void Update(float deltaTime, const FmVec3& vEyePos);
};

//--------------------------------------------------------------------

#endif // _WHIP_DATA_H
