
/****************************************************************

  File name   :  my_motion_state.cpp
  Author      :  Ò¶·å
  Version     :  1.0a
  Create Date :  2013/12/19  
  Description :  

*****************************************************************/

// --------------------------------------------------------------------------------------

#include "physics_bullet.h"
#include "my_motion_state.h"

// --------------------------------------------------------------------------------------

void	MyMotionState::updateBindingObj()
{
    IVisBase* pVisBase = GetVisBase(m_bindingID);
    if (pVisBase)
    {
        FmMat4 mtxObj;
        m_graphicsWorldTrans.getOpenGLMatrix((btScalar*)&mtxObj);
        FmVec3 vOffset = -m_offset;
        FmVec3TransformCoord(&vOffset, &vOffset, &mtxObj);

        pVisBase->SetWorldMatrix(mtxObj);
    }
}
