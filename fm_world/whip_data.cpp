//--------------------------------------------------------------------
// 文件名:		whip_data.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2014年4月11日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "../visual/fm_header.h"
#include "whip_data.h"

//--------------------------------------------------------------------

WhipData::WhipData()
{
    m_fInTime = 0.5f;
    m_fPauseTime = 0.0f;
    m_fOutTime = 0.5f;
    m_fTime = 0.0F;
    m_uScale = 1.0f;
    m_fBladeWidth = 0.1f;
    m_slicesCount = 2;
    memset(&m_params, 0, sizeof(m_params));

    m_vAttackPosition = FmVec3(0.0F, 0.0F, 0.0F);
    m_vSufferPosition = FmVec3(0.0F, 0.0F, 0.0F);
    m_vLeft = FmVec3(0.0f, 1.0f, 0.0f);

    m_verticesBuffer = 0;
    m_verticesCount = 0;
}

WhipData::~WhipData()
{
    if (m_verticesBuffer)
    {
        CORE_FREE(m_verticesBuffer, sizeof(whip_vertex_t) * m_verticesCount);
        m_verticesBuffer = 0;
        m_verticesCount = 0;
    }
}

//--------------------------------------------------------------------

void RopeData::Init(unsigned int slices)
{
    m_verticesCount = 4;
    m_verticesBuffer = (whip_vertex_t*)CORE_ALLOC(sizeof(whip_vertex_t) * m_verticesCount);
}

void RopeData::Update(float deltaTime, const FmVec3& vEyePos)
{
    m_fTime += deltaTime;

    FmVec3 vDis = m_vSufferPosition - m_vAttackPosition;
    float dis = vDis.length();

    float length;
    float r;
    if (m_fTime < m_fInTime)
    {
        r = m_fTime / m_fInTime;
        r = powf(r, m_params.w);
    }
    else if (m_fTime > m_fInTime + m_fPauseTime)
    {
        r = (m_fInTime + m_fPauseTime + m_fOutTime - m_fTime) / m_fOutTime;
        r = powf(r, m_params.w);
    }
    else
    {
        r = 1.0f;
    }
    length = dis * r;

    m_uScale = length / m_fBladeWidth;

    FmVec3 vPos;
    FmVec3 eyeDir;
    FmVec3 vLeft;

    // 起点
    eyeDir = m_vAttackPosition - vEyePos;   
    FmVec3Cross(&vLeft, &vDis, &eyeDir);
    vLeft.normalize();
    vLeft *= m_fBladeWidth*0.5f;
    
    vPos = m_vAttackPosition + vLeft;
    m_verticesBuffer[0].Set(vPos.x, vPos.y, vPos.z, 0.0f, 1.0f);
    vPos = m_vAttackPosition - vLeft;            
    m_verticesBuffer[1].Set(vPos.x, vPos.y, vPos.z, 1.0f, 1.0f);

    // 终点
    m_vTailPosition = m_vAttackPosition + vDis/dis*length;

    eyeDir = m_vTailPosition - vEyePos;  
    FmVec3Cross(&vLeft, &vDis, &eyeDir);
    vLeft.normalize();
    vLeft *= m_fBladeWidth*0.5f;

    vPos = m_vTailPosition + vLeft;
    m_verticesBuffer[2].Set(vPos.x, vPos.y, vPos.z, 0.0f, 0.0f);
    vPos = m_vTailPosition - vLeft;            
    m_verticesBuffer[3].Set(vPos.x, vPos.y, vPos.z, 1.0f, 0.0f);
}

//--------------------------------------------------------------------

SinWhipData::SinWhipData()
{
    m_slicesCount = 32;
    m_list_orbit_vertices = NULL;
    m_list_cos_0_pi = NULL;
    m_list_sin_0_2pi = NULL;
}

SinWhipData::~SinWhipData()
{
    if (m_list_orbit_vertices)
    {
        CORE_FREE(m_list_orbit_vertices, sizeof(FmVec3) * m_slicesCount);
        m_list_orbit_vertices = NULL;
    }

    if (m_list_cos_0_pi)
    {
        CORE_FREE(m_list_cos_0_pi, sizeof(float) * m_slicesCount);
        m_list_cos_0_pi = NULL;
    }

    if (m_list_sin_0_2pi)
    {
        CORE_FREE(m_list_sin_0_2pi, sizeof(float) * m_slicesCount);
        m_list_sin_0_2pi = NULL;
    }
}

void SinWhipData::Init(unsigned int slices)
{
    if (slices < 1)
    {
        slices = 32;
    }
    else if (slices > WHIP_MAX_VERTICES_COUNT / 2)
    {
        slices = WHIP_MAX_VERTICES_COUNT / 2;
    }

    m_slicesCount = slices;
    m_list_orbit_vertices = (FmVec3*)CORE_ALLOC(sizeof(FmVec3) * m_slicesCount);
    m_list_cos_0_pi  = (float*)CORE_ALLOC(sizeof(float) * m_slicesCount);
    m_list_sin_0_2pi = (float*)CORE_ALLOC(sizeof(float) * m_slicesCount);

    for (unsigned int i = 0; i < m_slicesCount; i++)
    {
        m_list_cos_0_pi[i]  = cosf(i*FLOAT_PI/(m_slicesCount - 1));
        m_list_sin_0_2pi[i] = sinf(i*2.0f*FLOAT_PI/(m_slicesCount - 1));
    }

    m_verticesCount = m_slicesCount*2;
    m_verticesBuffer = (whip_vertex_t*)CORE_ALLOC(sizeof(whip_vertex_t) * m_verticesCount);
}

void SinWhipData::Update(float deltaTime, const FmVec3& vEyePos)
{
    m_fTime += deltaTime;

    FmVec3 vCenter = (m_vSufferPosition + m_vAttackPosition) * 0.5f;

    FmVec3 vDis = m_vSufferPosition - m_vAttackPosition;
    float dis = vDis.length();
    if (dis < 0.000001f)
    {
        memset(m_verticesBuffer, 0, sizeof(whip_vertex_t) * m_verticesCount);
        return;
    }

    for (unsigned int i = 0; i < m_slicesCount; i++)
    {
        m_list_orbit_vertices[i] = vCenter;
        m_list_orbit_vertices[i] -= vDis * 0.5f * m_list_cos_0_pi[i];
        m_list_orbit_vertices[i] += m_vLeft * 0.5f * m_list_sin_0_2pi[i];
    }

    FmVec3 vPos;

    FmVec3 eyeDir;
    FmVec3 vEye;

    float r;
    float t;

    if (m_fTime < m_fInTime)
    {
        r = m_fTime / m_fInTime;
        r = powf(r, m_params.w);
        t = r * m_fInTime;

        // 前进
        float d = m_fInTime / 2.0f / (m_slicesCount - 1);
        for (unsigned int i = 0; i < m_slicesCount; i++)
        {
            if (t >= d*i)
            {
                vPos = m_list_orbit_vertices[i];
            }
            else
            {
                float a = t / (d*i);

                FmVec3 vDis2 = m_list_orbit_vertices[i] - m_vAttackPosition;

                float angle = a * FLOAT_PI;
                float _s = sinf(angle);
                float _c = cosf(angle);

                vPos = m_vAttackPosition;
                vPos += vDis2 * (1.0f - _c) * 0.5f;
                vPos -= m_vLeft * 0.5f * _s;
            }

            float v = i*1.0f/(m_slicesCount - 1);

            FmVec3 eyeDir = m_vAttackPosition - vEyePos;   
            FmVec3Cross(&vEye, &vDis, &eyeDir);
            vEye.normalize();
            vEye *= m_fBladeWidth*0.5f;

            m_verticesBuffer[i*2].Set(vPos.x + vEye.x, vPos.y + vEye.y, vPos.z + vEye.z, 0.0f, v);
            m_verticesBuffer[i*2 + 1].Set(vPos.x - vEye.x, vPos.y - vEye.y, vPos.z - vEye.z, 1.0f, v);
        }
    }
    else 
    {
        // 暂停与后退
        float r = 1.0f;
        if (m_fTime > m_fInTime + m_fPauseTime)
        {
            r = (m_fInTime + m_fPauseTime + m_fOutTime - m_fTime) / m_fOutTime;
        }

        for (unsigned int i = 0; i < m_slicesCount; i++)
        {
            vPos = m_list_orbit_vertices[i] - m_vAttackPosition;
            vPos *= r;
            vPos += m_vAttackPosition;

            float v = i*1.0f/(m_slicesCount - 1);

            FmVec3 eyeDir = m_vAttackPosition - vEyePos;   
            FmVec3Cross(&vEye, &vDis, &eyeDir);
            vEye.normalize();
            vEye *= m_fBladeWidth*0.5f;

            m_verticesBuffer[i*2].Set(vPos.x + vEye.x, vPos.y + vEye.y, vPos.z + vEye.z, 0.0f, v);
            m_verticesBuffer[i*2 + 1].Set(vPos.x - vEye.x, vPos.y - vEye.y, vPos.z - vEye.z, 1.0f, v);
        }
    }
}

//--------------------------------------------------------------------

CircleWhipData::CircleWhipData()
{
    m_slicesCount = 32;
    m_list_orbit_vertices = NULL;
    m_list_cos_0_pi = NULL;
    m_list_sin_0_pi = NULL;
}

CircleWhipData::~CircleWhipData()
{
    if (m_list_orbit_vertices)
    {
        CORE_FREE(m_list_orbit_vertices, sizeof(FmVec3) * m_slicesCount);
        m_list_orbit_vertices = NULL;
    }

    if (m_list_cos_0_pi)
    {
        CORE_FREE(m_list_cos_0_pi, sizeof(float) * m_slicesCount);
        m_list_cos_0_pi = NULL;
    }

    if (m_list_sin_0_pi)
    {
        CORE_FREE(m_list_sin_0_pi, sizeof(float) * m_slicesCount);
        m_list_sin_0_pi = NULL;
    }
}

void CircleWhipData::Init(unsigned int slices)
{
    if (slices < 1)
    {
        slices = 32;
    }
    else if (slices > WHIP_MAX_VERTICES_COUNT / 2)
    {
        slices = WHIP_MAX_VERTICES_COUNT / 2;
    }

    m_slicesCount = slices;
    m_list_orbit_vertices = (FmVec3*)CORE_ALLOC(sizeof(FmVec3) * m_slicesCount);
    m_list_cos_0_pi  = (float*)CORE_ALLOC(sizeof(float) * m_slicesCount);
    m_list_sin_0_pi = (float*)CORE_ALLOC(sizeof(float) * m_slicesCount);

    for (unsigned int i = 0; i < m_slicesCount; i++)
    {
        m_list_cos_0_pi[i] = cosf(i*FLOAT_PI/(m_slicesCount - 1));
        m_list_sin_0_pi[i] = sinf(i*FLOAT_PI/(m_slicesCount - 1));
    }

    m_verticesCount = m_slicesCount*2;
    m_verticesBuffer = (whip_vertex_t*)CORE_ALLOC(sizeof(whip_vertex_t) * m_verticesCount);
}

void CircleWhipData::Update(float deltaTime, const FmVec3& vEyePos)
{
    m_fTime += deltaTime;

    FmVec3 vCenter = (m_vSufferPosition + m_vAttackPosition) * 0.5f;

    FmVec3 vDis = m_vSufferPosition - m_vAttackPosition;
    float dis = vDis.length();
    if (dis < 0.000001f)
    {
        memset(m_verticesBuffer, 0, sizeof(whip_vertex_t) * m_verticesCount);
        return;
    }

    for (unsigned int i = 0; i < m_slicesCount; i++)
    {
        m_list_orbit_vertices[i] = vCenter;
        m_list_orbit_vertices[i] -= vDis * 0.5f * m_list_cos_0_pi[i];
        m_list_orbit_vertices[i] += m_vLeft * m_list_sin_0_pi[i] * m_params.x;
    }

    FmVec3 vPos;

    FmVec3 eyeDir;
    FmVec3 vEye;

    float r;
    float t;

    if (m_fTime < m_fInTime)
    {
        r = m_fTime / m_fInTime;
        r = powf(r, m_params.w);
        t = r * m_fInTime;

        // 前进

        float d = m_fInTime / 2.0f / (m_slicesCount - 1);
        for (unsigned int i = 0; i < m_slicesCount; i++)
        {
            if (t >= d*i)
            {
                vPos = m_list_orbit_vertices[i];
            }
            else
            {
                float a = t / (d*i);

                FmVec3 vDis2 = m_list_orbit_vertices[i] - m_vAttackPosition;

                float angle = a * FLOAT_PI;
                float _s = sinf(angle);
                float _c = cosf(angle);

                vPos = m_vAttackPosition;
                vPos += vDis2 * (1.0f - _c) * 0.5f;
                vPos -= m_vLeft * 0.5f * _s;
            }

            float v = i*1.0f/(m_slicesCount - 1);

            FmVec3 eyeDir = m_vAttackPosition - vEyePos;   
            FmVec3Cross(&vEye, &vDis, &eyeDir);
            vEye.normalize();
            vEye *= m_fBladeWidth*0.5f;

            m_verticesBuffer[i*2].Set(vPos.x + vEye.x, vPos.y + vEye.y, vPos.z + vEye.z, 0.0f, v);
            m_verticesBuffer[i*2 + 1].Set(vPos.x - vEye.x, vPos.y - vEye.y, vPos.z - vEye.z, 1.0f, v);
        }
    }
    else 
    {
        // 暂停与后退
        float r = 1.0f;
        if (m_fTime > m_fInTime + m_fPauseTime)
        {
            r = (m_fInTime + m_fPauseTime + m_fOutTime - m_fTime) / m_fOutTime;
        }

        for (unsigned int i = 0; i < m_slicesCount; i++)
        {
            vPos = m_list_orbit_vertices[i] - m_vAttackPosition;
            vPos *= r;
            vPos += m_vAttackPosition;

            float v = i*1.0f/(m_slicesCount - 1);

            FmVec3 eyeDir = m_vAttackPosition - vEyePos;   
            FmVec3Cross(&vEye, &vDis, &eyeDir);
            vEye.normalize();
            vEye *= m_fBladeWidth*0.5f;

            m_verticesBuffer[i*2].Set(vPos.x + vEye.x, vPos.y + vEye.y, vPos.z + vEye.z, 0.0f, v);
            m_verticesBuffer[i*2 + 1].Set(vPos.x - vEye.x, vPos.y - vEye.y, vPos.z - vEye.z, 1.0f, v);
        }
    }
}

//--------------------------------------------------------------------

void SinWhipData2::Update(float deltaTime, const FmVec3& vEyePos)
{
    m_fTime += deltaTime;

    FmVec3 vCenter = (m_vSufferPosition + m_vAttackPosition) * 0.5f;

    FmVec3 vDis = m_vSufferPosition - m_vAttackPosition;
    float dis = vDis.length();
    if (dis < 0.000001f)
    {
        memset(m_verticesBuffer, 0, sizeof(whip_vertex_t) * m_verticesCount);
        return;
    }

    for (unsigned int i = 0; i < m_slicesCount; i++)
    {
        m_list_orbit_vertices[i] = vCenter;
        m_list_orbit_vertices[i] -= vDis * 0.5f * m_list_cos_0_pi[i];
        m_list_orbit_vertices[i] += m_vLeft * 0.5f * m_list_sin_0_2pi[i];
    }

    FmVec3 vPos;
    FmVec3 vNorLeft = m_vLeft.normalized();

    float r;
    float t;

    if (m_fTime < m_fInTime)
    {
        r = m_fTime / m_fInTime;
        r = powf(r, m_params.w);
        t = r * m_fInTime;

        // 前进
        float d = m_fInTime / 2.0f / (m_slicesCount - 1);
        for (unsigned int i = 0; i < m_slicesCount; i++)
        {
            if (t >= d*i)
            {
                vPos = m_list_orbit_vertices[i];
            }
            else
            {
                float a = t / (d*i);

                FmVec3 vDis2 = m_list_orbit_vertices[i] - m_vAttackPosition;

                float angle = a * FLOAT_PI;
                float _s = sinf(angle);
                float _c = cosf(angle);

                vPos = m_vAttackPosition;
                vPos += vDis2 * (1.0f - _c) * 0.5f;
                vPos -= m_vLeft * 0.5f * _s;
            }

            float v = i*1.0f/(m_slicesCount - 1);

            m_verticesBuffer[i*2].Set(vPos.x, vPos.y, vPos.z, 0.0f, v);
            vPos -= vNorLeft * m_fBladeWidth;
            m_verticesBuffer[i*2 + 1].Set(vPos.x, vPos.y, vPos.z, 1.0f, v);
        }
    }
    else 
    {
        // 暂停与后退
        float r = 1.0f;
        if (m_fTime > m_fInTime + m_fPauseTime)
        {
            r = (m_fInTime + m_fPauseTime + m_fOutTime - m_fTime) / m_fOutTime;
        }

        for (unsigned int i = 0; i < m_slicesCount; i++)
        {
            vPos = m_list_orbit_vertices[i] - m_vAttackPosition;
            vPos *= r;
            vPos += m_vAttackPosition;

            float v = i*1.0f/(m_slicesCount - 1);

            m_verticesBuffer[i*2].Set(vPos.x, vPos.y, vPos.z, 0.0f, v);
            vPos -= vNorLeft * m_fBladeWidth;
            m_verticesBuffer[i*2 + 1].Set(vPos.x, vPos.y, vPos.z, 1.0f, v);
        }
    }
}

//--------------------------------------------------------------------

void CircleWhipData2::Update(float deltaTime, const FmVec3& vEyePos)
{
    m_fTime += deltaTime;

    FmVec3 vCenter = (m_vSufferPosition + m_vAttackPosition) * 0.5f;

    FmVec3 vDis = m_vSufferPosition - m_vAttackPosition;
    float dis = vDis.length();
    if (dis < 0.000001f)
    {
        memset(m_verticesBuffer, 0, sizeof(whip_vertex_t) * m_verticesCount);
        return;
    }

    for (unsigned int i = 0; i < m_slicesCount; i++)
    {
        m_list_orbit_vertices[i] = vCenter;
        m_list_orbit_vertices[i] -= vDis * 0.5f * m_list_cos_0_pi[i];
        m_list_orbit_vertices[i] += m_vLeft * m_list_sin_0_pi[i] * m_params.x;
    }

    FmVec3 vPos;
    FmVec3 vNorLeft = m_vLeft.normalized();

    float r;
    float t;

    if (m_fTime < m_fInTime)
    {
        r = m_fTime / m_fInTime;
        r = powf(r, m_params.w);
        t = r * m_fInTime;

        // 前进
        float d = m_fInTime / 2.0f / (m_slicesCount - 1);
        for (unsigned int i = 0; i < m_slicesCount; i++)
        {
            if (t >= d*i)
            {
                vPos = m_list_orbit_vertices[i];
            }
            else
            {
                float a = t / (d*i);

                FmVec3 vDis2 = m_list_orbit_vertices[i] - m_vAttackPosition;

                float angle = a * FLOAT_PI;
                float _s = sinf(angle);
                float _c = cosf(angle);

                vPos = m_vAttackPosition;
                vPos += vDis2 * (1.0f - _c) * 0.5f;
                vPos -= m_vLeft * 0.5f * _s;
            }

            float v = i*1.0f/(m_slicesCount - 1);

            m_verticesBuffer[i*2].Set(vPos.x, vPos.y, vPos.z, 0.0f, v);
            vPos -= vNorLeft * m_fBladeWidth;
            m_verticesBuffer[i*2 + 1].Set(vPos.x, vPos.y, vPos.z, 1.0f, v);
        }
    }
    else 
    {
        // 暂停与后退
        float r = 1.0f;
        if (m_fTime > m_fInTime + m_fPauseTime)
        {
            r = (m_fInTime + m_fPauseTime + m_fOutTime - m_fTime) / m_fOutTime;
        }

        for (unsigned int i = 0; i < m_slicesCount; i++)
        {
            vPos = m_list_orbit_vertices[i] - m_vAttackPosition;
            vPos *= r;
            vPos += m_vAttackPosition;

            float v = i*1.0f/(m_slicesCount - 1);

            m_verticesBuffer[i*2].Set(vPos.x, vPos.y, vPos.z, 0.0f, v);
            vPos -= vNorLeft * m_fBladeWidth;
            m_verticesBuffer[i*2 + 1].Set(vPos.x, vPos.y, vPos.z, 1.0f, v);
        }
    }
}

//--------------------------------------------------------------------

CircleSaberData::CircleSaberData()
{
    m_slicesCount = 32;
    m_list_sin_cos_0_pi = NULL;
}

CircleSaberData::~CircleSaberData()
{
    if (m_list_sin_cos_0_pi)
    {
        CORE_FREE(m_list_sin_cos_0_pi, sizeof(FmVec2) * (m_slicesCount + 1));
        m_list_sin_cos_0_pi = NULL;
    }
}

void CircleSaberData::Init(unsigned int slices)
{
    if (slices < 1)
    {
        slices = 32;
    }
    else if (slices > WHIP_MAX_VERTICES_COUNT / 2)
    {
        slices = WHIP_MAX_VERTICES_COUNT / 2;
    }

    m_slicesCount = slices;
    m_list_sin_cos_0_pi  = (FmVec2*)CORE_ALLOC(sizeof(FmVec2) * (m_slicesCount + 1));
    for (unsigned int i = 0; i <= m_slicesCount; i++)
    {
        m_list_sin_cos_0_pi[i].x = sinf(i*FLOAT_PI/m_slicesCount);
        m_list_sin_cos_0_pi[i].y = cosf(i*FLOAT_PI/m_slicesCount);
    }

    m_verticesCount = m_slicesCount*2 + 2;
    m_verticesBuffer = (whip_vertex_t*)CORE_ALLOC(sizeof(whip_vertex_t) * m_verticesCount);
}

void CircleSaberData::Update(float deltaTime, const FmVec3& vEyePos)
{
    m_fTime += deltaTime;

    float angle;

    float r;
    if (m_fTime < m_fInTime)
    {
        r = m_fTime / m_fInTime;
        r = powf(r, m_params.w);
        angle = r * FLOAT_PI;
    }
    else if (m_fTime > m_fInTime + m_fPauseTime)
    {
        r = (m_fInTime + m_fPauseTime + m_fOutTime - m_fTime) / m_fOutTime;
        r = powf(r, m_params.w);
        angle = FLOAT_PI*(2.0f - r);
    }
    else
    {
        r = 1.0f;
        angle = FLOAT_PI;
    }

    FmVec3 vDis = m_vSufferPosition - m_vAttackPosition;

    float _s = sinf(angle);
    float _c = cosf(angle);

    FmVec3 vOrbit = m_vAttackPosition;    
    vOrbit += vDis * (1.0f - _c) * 0.5f;
    vOrbit += m_vLeft * _s;

    FmVec3 vCenter = (m_vAttackPosition + vOrbit) * 0.5f;

    FmVec3 vDir = vOrbit - m_vAttackPosition;
    FmVec3 vUp;
    FmVec3Cross(&vUp, &vDis, &m_vLeft);

    FmVec3 vNormal;
    FmVec3Cross(&vNormal, &vDir, &vUp);
    vNormal.normalize();

    FmVec3 vPos;
    for (unsigned int i = 0; i <= m_slicesCount; i++)
    {
        vPos = vCenter - vDir * m_list_sin_cos_0_pi[i].y * 0.5f;
        vPos += vNormal * m_list_sin_cos_0_pi[i].x * m_params.x;
        m_verticesBuffer[i*2].Set(vPos.x, vPos.y, vPos.z, 0.0f, i*1.0f/m_slicesCount);
        //m_verticesBuffer[i*2 + 1].Set(vPos.x, vPos.y + 1.0f, vPos.z, 1.0f, i*1.0f/m_slicesCount);
    }

    if (FLOAT_PI*2.0f - angle < m_params.y)
    {
        angle -= (FLOAT_PI*2.0f - angle);
    }
    else
    {
        angle -= m_params.y;
    }
    if (angle < 0.0f)
    {
        angle = 0.0f;
    }

    _s = sinf(angle);
    _c = cosf(angle);

    vOrbit = m_vAttackPosition;    
    vOrbit += vDis * (1.0f - _c) * 0.5f;
    vOrbit += m_vLeft * _s;

    vCenter = (m_vAttackPosition + vOrbit) * 0.5f;

    vDir = vOrbit - m_vAttackPosition;
    vDir += vDis * 0.001f;
    FmVec3Cross(&vUp, &vDis, &m_vLeft);

    FmVec3Cross(&vNormal, &vDir, &vUp);
    vNormal.normalize();

    for (unsigned int i = 0; i <= m_slicesCount; i++)
    {
        vPos = vCenter - vDir * m_list_sin_cos_0_pi[i].y * 0.5f;
        vPos += vNormal * m_list_sin_cos_0_pi[i].x * m_params.x;
        m_verticesBuffer[i*2 + 1].Set(vPos.x, vPos.y, vPos.z, 1.0f, i*1.0f/m_slicesCount);
    }
}

//--------------------------------------------------------------------

void SimiCircleSaberData::Update(float deltaTime, const FmVec3& vEyePos)
{
    m_fTime += deltaTime;

    float angle;

    float r;
    if (m_fTime < m_fInTime)
    {
        r = m_fTime / m_fInTime;
        r = powf(r, m_params.w);
        angle = r * FLOAT_PI*0.5f;
    }
    else if (m_fTime > m_fInTime + m_fPauseTime)
    {
        r = (m_fInTime + m_fPauseTime + m_fOutTime - m_fTime) / m_fOutTime;
        r = powf(r, m_params.w);
        angle = FLOAT_PI*(2.0f - r)*0.5f;
    }
    else
    {
        r = 1.0f;
        angle = FLOAT_PI*0.5f;
    }

    FmVec3 vDis = m_vSufferPosition - m_vAttackPosition;

    float _s = sinf(angle);
    float _c = cosf(angle);

    FmVec3 vOrbit = m_vAttackPosition;    
    vOrbit += vDis * _s;
    vOrbit += m_vLeft * _c;

    FmVec3 vDir = vOrbit - m_vAttackPosition;
    FmVec3 vUp;
    FmVec3Cross(&vUp, &vDis, &m_vLeft);

    FmVec3 vNormal;
    FmVec3Cross(&vNormal, &vDir, &vUp);
    vNormal.normalize();

    FmVec3 vPos;
    for (unsigned int i = 0; i <= m_slicesCount; i++)
    {
        vPos = m_vAttackPosition + vDir * (1.0f - m_list_sin_cos_0_pi[i].y) * 0.5f;
        vPos += vNormal * m_list_sin_cos_0_pi[i].x * m_params.x;
        m_verticesBuffer[i*2].Set(vPos.x, vPos.y, vPos.z, 0.0f, i*1.0f/m_slicesCount);
    }

    if (FLOAT_PI - angle < m_params.y)
    {
        angle -= (FLOAT_PI - angle);
    }
    else
    {
        angle -= m_params.y;
    }
    if (angle < 0.0f)
    {
        angle = 0.0f;
    }

    _s = sinf(angle);
    _c = cosf(angle);

    vOrbit = m_vAttackPosition;    
    vOrbit += vDis * _s;
    vOrbit += m_vLeft * _c;

    vDir = vOrbit - m_vAttackPosition;
    vDir += vDis * 0.001f;
    FmVec3Cross(&vUp, &vDis, &m_vLeft);

    FmVec3Cross(&vNormal, &vDir, &vUp);
    vNormal.normalize();

    for (unsigned int i = 0; i <= m_slicesCount; i++)
    {
        vPos = m_vAttackPosition + vDir * (1.0f - m_list_sin_cos_0_pi[i].y) * 0.5f;
        vPos += vNormal * m_list_sin_cos_0_pi[i].x * m_params.x;
        m_verticesBuffer[i*2 + 1].Set(vPos.x, vPos.y, vPos.z, 1.0f, i*1.0f/m_slicesCount);
    }
}
