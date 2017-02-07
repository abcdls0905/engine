
/****************************************************************

  File name   :  common.h
  Author      :  Ò¶·å
  Version     :  1.0a
  Create Date :  2013/11/27  
  Description :  

*****************************************************************/

#ifndef __common_H__
#define __common_H__

// --------------------------------------------------------------------------------------

#include "../math/fm_vector.h"
#include "../math/fm_matrix.h"
#include "../math/fm_quaternion.h"
#include "linear_math/bt_vector_3.h"
#include "linear_math/bt_quaternion.h"
#include "linear_math/bt_transform.h"

// --------------------------------------------------------------------------------------

inline btVector3 FmVec3ToBtVector3(const FmVec3& v)
{
    return btVector3(v.x, v.y, v.z);
}

inline FmVec3 BtVector3ToFmVec3(const btVector3& v)
{
    return FmVec3(v.m_floats[0], v.m_floats[1], v.m_floats[2]);
}

// --------------------------------------------------------------------------------------

#endif
