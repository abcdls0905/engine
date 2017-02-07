/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BT_MOTIONSTATE_H
#define BT_MOTIONSTATE_H

#include "bt_transform.h"

///The btMotionState interface class allows the dynamics world to synchronize and interpolate the updated world transforms with graphics
///For optimizations, potentially only moving objects get synchronized (using setWorldPosition/setWorldOrientation)
/*
与显示的整合，MotionState
一个只有数据运算的物理引擎，一般而言只能为显示引擎提供数据，这就牵涉到与图形引擎整合的问题，
像Box2D这样的物理引擎就是直接需要直接向各个物理实体去查询位置，然后更新显示，这种方式虽然简
单，但是我感觉非常不好，因为难免在update中去更新这种东西，导致游戏逻辑部分需要处理物理引擎+
图形引擎两部分的内容。（可以参考Box2D与Cocos2D for iPhone的整合）而且，对于完全没有移动的物体
也会进行一次查询和移动操作。（即使优化，对不移动物体也是进行了两次查询）
Bullet为了解决此问题，提供了新的解决方案，MotionState。其实就是当活动物体状态改变时提供一种回
调，而且就Bullet的文档中说明，此种回调还带有适当的插值以优化显示。通过这种方法，在MotionState
部分就已经可以完成显示的更新，不用再需要在update中添加这种更新的代码。而且，注意，仅仅对活动
物体状态改变时才会进行回调，这样完全避免了不活动物体的性能损失。
motion state 是对渲染引擎提供的一个接口, 便于渲染引擎更新渲染对象的位置.
*/
class btMotionState
{
public:
	virtual ~btMotionState()
	{
		
	}
	
	virtual void	getWorldTransform(btTransform& worldTrans ) const =0;

	//Bullet only calls the update of worldtransform for active objects
	virtual void	setWorldTransform(const btTransform& worldTrans)=0;
};

#endif //BT_MOTIONSTATE_H
