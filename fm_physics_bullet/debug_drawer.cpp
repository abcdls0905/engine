
/****************************************************************

  File name   :  debug_drawer.cpp
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/11/28  
  Description :  物理调试信息渲染

*****************************************************************/

// --------------------------------------------------------------------------------------

#include "../public/macros.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/module.h"
#include "../public/converts.h"
#include "../visual/i_primitive_3d_drawer.h"
#include "../visual/i_render.h"
#include "../visual/i_painter.h"
#include "common.h"
#include "debug_drawer.h"
#include <assert.h>

// --------------------------------------------------------------------------------------

#define MAX_TEXT_LENGTH         128

#define YD_INT_TO_BYTE(x)       ( (x) < 0 ? 0 : (((x) > 255) ? 255 : (x)) )
#define YD_REAL_TO_BYTE(x)      ( (x) < 0 ? 0 : (((x) > 1.0f) ? 255 : (unsigned char)((x) * 255)) )
#define YD_BYTE_TO_REAL(x)      ( (x) * 0.00392156862745f);           // x / 255

// 通过R,G,B生成一整数表示颜色
#define YD_RGB(r,g,b)           \
    (unsigned int) ( (b) | ( (g) << 8 ) | ( (r) << 16 ) | 0xff000000 )

// 通过A,R,G,B生成一整数表示颜色
#define YD_ARGB(a,r,g,b)        \
    (unsigned int) ( (b) | ( (g) << 8 ) | ( (r) << 16 ) | ( (a) << 24 ) )

// --------------------------------------------------------------------------------------

DebugDrawer::DebugDrawer()
            :m_debugMode(0)
{
    m_pRender = NULL;
    m_pPrimitive3DDrawer = NULL;
    m_pPainter = NULL;

    m_textColor = 0xff00ff00;
}

DebugDrawer::~DebugDrawer()
{
}

void    DebugDrawer::init(IRender* pRender, IPrimitive3DDrawer* pPrimitive3DDrawer, IPainter* pPainter)
{
    m_pRender = pRender;
    m_pPrimitive3DDrawer = pPrimitive3DDrawer;
    m_pPainter = pPainter;
}

bool    DebugDrawer::isReady() const
{
    return (m_pRender && m_pPrimitive3DDrawer && m_pPainter);
}

void    DebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor)
{
    unsigned int r = YD_REAL_TO_BYTE(fromColor.m_floats[0]);
    unsigned int g = YD_REAL_TO_BYTE(fromColor.m_floats[1]);
    unsigned int b = YD_REAL_TO_BYTE(fromColor.m_floats[2]);
    unsigned int color0 = YD_RGB(r, g, b);

    r = YD_REAL_TO_BYTE(toColor.m_floats[0]);
    g = YD_REAL_TO_BYTE(toColor.m_floats[1]);
    b = YD_REAL_TO_BYTE(toColor.m_floats[2]);
    unsigned int color1 = YD_RGB(r, g, b);

    m_pPrimitive3DDrawer->DrawLine(BtVector3ToFmVec3(from),
        BtVector3ToFmVec3(to),
        color0, color1);
}

void    DebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
    unsigned int r = YD_REAL_TO_BYTE(color.m_floats[0]);
    unsigned int g = YD_REAL_TO_BYTE(color.m_floats[1]);
    unsigned int b = YD_REAL_TO_BYTE(color.m_floats[2]);

    m_pPrimitive3DDrawer->DrawLine(BtVector3ToFmVec3(from),
        BtVector3ToFmVec3(to),
        YD_RGB(r, g, b));
}

void DebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color)
{
    FmVec3 vCenter(p.m_floats[0], p.m_floats[1], p.m_floats[2]);
    unsigned int r = YD_REAL_TO_BYTE(color.m_floats[0]);
    unsigned int g = YD_REAL_TO_BYTE(color.m_floats[1]);
    unsigned int b = YD_REAL_TO_BYTE(color.m_floats[2]);

    m_pPrimitive3DDrawer->DrawSphere(vCenter, radius, YD_RGB(r, g, b));
}

void    DebugDrawer::drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha)
{
    FmVec3 v0(a.m_floats[0], a.m_floats[1], a.m_floats[2]);
    FmVec3 v1(b.m_floats[0], b.m_floats[1], b.m_floats[2]);
    FmVec3 v2(c.m_floats[0], c.m_floats[1], c.m_floats[2]);

    unsigned int _r = YD_REAL_TO_BYTE(color.m_floats[0]);
    unsigned int _g = YD_REAL_TO_BYTE(color.m_floats[1]);
    unsigned int _b = YD_REAL_TO_BYTE(color.m_floats[2]);
    unsigned int _a = YD_REAL_TO_BYTE(alpha);

    m_pPrimitive3DDrawer->DrawTriangle(v0, v1, v2, YD_ARGB(_a, _r, _g, _b));
}

void    DebugDrawer::drawText(float posX, float posY, const char* textString)
{
    if (!textString || !textString[0])
    {
        return;
    }

    wchar_t buf[MAX_TEXT_LENGTH];
    ToWideStr(textString, buf, MAX_TEXT_LENGTH);

    m_pPainter->WriteText((int)posX, (int)posY, m_textColor, buf);
}

void    DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
    if (!textString || !textString[0])
    {
        return;
    }

    FmMat4 mtxWorld;
    FmMat4Identity(&mtxWorld);
    IRenderContext* pContext = m_pRender->GetContext();	 

    const FmMat4& mtxView = pContext->GetCamera().mtxView;
    const FmMat4& mtxProject = pContext->GetCamera().mtxDxProj;
    int view_width = pContext->GetViewWidth();
    int view_height = pContext->GetViewHeight();

    FmVec3 vPos(location.m_floats[0], location.m_floats[1], location.m_floats[2]);
    FmVec3 vDst;
    FmVec3ProjectForD3D(&vDst,&vPos,&mtxProject,&mtxView,&mtxWorld,
        0,0,view_width,view_height,pContext->GetCamera().fNearZ,pContext->GetCamera().fFarZ);
    if (vDst.z < 0.0F || vDst.z > 1.0F)
    {
        return;
    }

    drawText(vDst.x, vDst.y, textString);
}

void    DebugDrawer::reportErrorWarning(const char* warningString)
{
    CORE_TRACE(warningString);
}

void    DebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
    btVector3 to = pointOnB+normalOnB*1;//distance;
    const btVector3& from = pointOnB;

    drawLine(from, to, color);
}





