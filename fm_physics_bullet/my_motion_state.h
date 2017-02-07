
/****************************************************************

  File name   :  my_motion_state.h
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2013/12/19  
  Description :  

*****************************************************************/

#ifndef __MY_MOTION_STATE_H_
#define __MY_MOTION_STATE_H_

// --------------------------------------------------------------------------------------

#include "linear_math/bt_motion_state.h"
#include "../visual/i_vis_base.h"

// --------------------------------------------------------------------------------------

///The MyMotionState provides a common implementation to synchronize world transforms with offsets.
class MyMotionState : public btMotionState
{
public:
	btTransform m_graphicsWorldTrans;
	btTransform m_startWorldTrans;
	PERSISTID m_bindingID;
    FmVec3 m_offset;

	MyMotionState(const btTransform& startTrans = btTransform::getIdentity())
		: m_graphicsWorldTrans(startTrans),
		m_startWorldTrans(startTrans),
        m_offset(0.0f, 0.0f, 0.0f)
	{
	}

	///synchronizes world transform from user to physics
    void	getWorldTransform(btTransform& centerOfMassWorldTrans) const 
	{
		centerOfMassWorldTrans = m_graphicsWorldTrans;
	}

	///synchronizes world transform from physics to user
	///Bullet only calls the update of worldtransform for active objects
	void	setWorldTransform(const btTransform& centerOfMassWorldTrans)
    {
        m_graphicsWorldTrans = centerOfMassWorldTrans;
        updateBindingObj();
    }

    // 将物理刚体的变换矩阵设置给绑定模型
    void    updateBindingObj();
};

// --------------------------------------------------------------------------------------

#endif //BT_DEFAULT_MOTION_STATE_H
