
/****************************************************************

  File name   :  debug_drawer.h
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/11/28  
  Description :  物理调试信息渲染

*****************************************************************/

#ifndef __debug_drawer_H__
#define __debug_drawer_H__

// --------------------------------------------------------------------------------------

#include "linear_math/bt_i_debug_draw.h"

// --------------------------------------------------------------------------------------

class IRender;
class IPrimitive3DDrawer;
class IPainter;

// --------------------------------------------------------------------------------------

class DebugDrawer : public btIDebugDraw
{
public:
    DebugDrawer();
    ~DebugDrawer(); 

    void    init(IRender* pRender, IPrimitive3DDrawer* pPrimitive3DDrawer, IPainter* pPainter);

    bool    isReady() const;

    void    drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

    void    drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

    void    drawSphere(const btVector3& p, btScalar radius, const btVector3& color);

    void    drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha);
    
    void    drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

    void    reportErrorWarning(const char* warningString);

    void    drawText(float posX, float posY, const char* textString);

    void    draw3dText(const btVector3& location, const char* textString);

    void    setTextColor(unsigned int color) { m_textColor = color; }

    void    setDebugMode(int debugMode) { m_debugMode = debugMode; }

    int     getDebugMode() const { return m_debugMode; }

private:
    IRender* m_pRender;
    IPrimitive3DDrawer* m_pPrimitive3DDrawer;
    IPainter* m_pPainter;
    
    unsigned int m_textColor;

    int m_debugMode;
};

// --------------------------------------------------------------------------------------

#endif  //__debug_drawer_H__
