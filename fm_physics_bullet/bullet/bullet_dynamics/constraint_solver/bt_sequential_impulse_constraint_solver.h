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

#ifndef BT_SEQUENTIAL_IMPULSE_CONSTRAINT_SOLVER_H
#define BT_SEQUENTIAL_IMPULSE_CONSTRAINT_SOLVER_H

class btIDebugDraw;
class btPersistentManifold;
class btDispatcher;
class btCollisionObject;
#include "bullet_dynamics/constraint_solver/bt_typed_constraint.h"
#include "bullet_dynamics/constraint_solver/bt_contact_solver_info.h"
#include "bullet_dynamics/constraint_solver/bt_solver_body.h"
#include "bullet_dynamics/constraint_solver/bt_solver_constraint.h"
#include "bullet_collision/narrow_phase_collision/bt_manifold_point.h"
#include "bullet_dynamics/constraint_solver/bt_constraint_solver.h"

// 约束解算器,这个对象不只是处理普通约束的,还有当两个刚体发生接触时,处理二者的摩擦,穿刺,反弹属性
/*

约束分类：可积约束，不可积约束 ，摩擦力
整个bullet在动力学方面的核心就是btSequentialImpulseConstraintSolver 这个类及其实现
这个类名字可以翻译为基于顺序冲量处理方法的约束求解过程(这是个缩略的函数名不可直译）。
这个名字的含义很重要，表示了这个求解方法是按照顺序来处理约束以及接触点，摩擦力，
one by one并且求解的结果是冲量的范数。 

btSequentialImpulseConstraintSolver 的全部重心在于solveGroupCacheFriendlySetup 
负责对于三种约束进行计算准备（包括计算一些解方程要用到的常量以及中间变量等）和 
solveGroupCacheFriendlyIterations 负责应用求解。

solveGroupCacheFriendlySetup 分析
首先是关于可积约束的计算准备：首先是遍历约束数组，每一个约束都需要计算自己的jacbian,初始化一些常量和中间变量
关于接触点约束的计算准备：这里有个重要的函数convertContact， 把从碰撞检测阶段获得的接触点转化为不可积约束并初始化常量和中间变量，
以及初始化摩擦力的相关数据，摩擦力计算参照。
*/

///The btSequentialImpulseConstraintSolver is a fast SIMD implementation of the Projected Gauss Seidel (iterative LCP) method.
ATTRIBUTE_ALIGNED16(class) btSequentialImpulseConstraintSolver : public btConstraintSolver
{
protected:
	btAlignedObjectArray<btSolverBody>      m_tmpSolverBodyPool;
	btConstraintArray			m_tmpSolverContactConstraintPool;
	btConstraintArray			m_tmpSolverNonContactConstraintPool;
	btConstraintArray			m_tmpSolverContactFrictionConstraintPool;
	btConstraintArray			m_tmpSolverContactRollingFrictionConstraintPool;

	btAlignedObjectArray<int>	m_orderTmpConstraintPool;
	btAlignedObjectArray<int>	m_orderNonContactConstraintPool;
	btAlignedObjectArray<int>	m_orderFrictionConstraintPool;
	btAlignedObjectArray<btTypedConstraint::btConstraintInfo1> m_tmpConstraintSizesPool;
	int							m_maxOverrideNumSolverIterations;
	int m_fixedBodyId;
	void setupFrictionConstraint(	btSolverConstraint& solverConstraint, const btVector3& normalAxis,int solverBodyIdA,int  solverBodyIdB,
									btManifoldPoint& cp,const btVector3& rel_pos1,const btVector3& rel_pos2,
									btCollisionObject* colObj0,btCollisionObject* colObj1, btScalar relaxation, 
									btScalar desiredVelocity=0., btScalar cfmSlip=0.);

	void setupRollingFrictionConstraint(	btSolverConstraint& solverConstraint, const btVector3& normalAxis,int solverBodyIdA,int  solverBodyIdB,
									btManifoldPoint& cp,const btVector3& rel_pos1,const btVector3& rel_pos2,
									btCollisionObject* colObj0,btCollisionObject* colObj1, btScalar relaxation, 
									btScalar desiredVelocity=0., btScalar cfmSlip=0.);

	btSolverConstraint&	addFrictionConstraint(const btVector3& normalAxis,int solverBodyIdA,int solverBodyIdB,int frictionIndex,btManifoldPoint& cp,const btVector3& rel_pos1,const btVector3& rel_pos2,btCollisionObject* colObj0,btCollisionObject* colObj1, btScalar relaxation, btScalar desiredVelocity=0., btScalar cfmSlip=0.);
	btSolverConstraint&	addRollingFrictionConstraint(const btVector3& normalAxis,int solverBodyIdA,int solverBodyIdB,int frictionIndex,btManifoldPoint& cp,const btVector3& rel_pos1,const btVector3& rel_pos2,btCollisionObject* colObj0,btCollisionObject* colObj1, btScalar relaxation, btScalar desiredVelocity=0, btScalar cfmSlip=0.f);

	
	void setupContactConstraint(btSolverConstraint& solverConstraint, int solverBodyIdA, int solverBodyIdB, btManifoldPoint& cp, 
								const btContactSolverInfo& infoGlobal,btScalar& relaxation, const btVector3& rel_pos1, const btVector3& rel_pos2);

	static void	applyAnisotropicFriction(btCollisionObject* colObj,btVector3& frictionDirection, int frictionMode);

	void setFrictionConstraintImpulse( btSolverConstraint& solverConstraint, int solverBodyIdA,int solverBodyIdB, 
										 btManifoldPoint& cp, const btContactSolverInfo& infoGlobal);

	///m_btSeed2 is used for re-arranging the constraint rows. improves convergence/quality of friction
	unsigned long	m_btSeed2;

	
	btScalar restitutionCurve(btScalar rel_vel, btScalar restitution);

	virtual void convertContacts(btPersistentManifold** manifoldPtr, int numManifolds, const btContactSolverInfo& infoGlobal);

	void	convertContact(btPersistentManifold* manifold,const btContactSolverInfo& infoGlobal);


	void	resolveSplitPenetrationSIMD(
     btSolverBody& bodyA,btSolverBody& bodyB,
        const btSolverConstraint& contactConstraint);

	void	resolveSplitPenetrationImpulseCacheFriendly(
       btSolverBody& bodyA,btSolverBody& bodyB,
        const btSolverConstraint& contactConstraint);

	//internal method
	int		getOrInitSolverBody(btCollisionObject& body,btScalar timeStep);
	void	initSolverBody(btSolverBody* solverBody, btCollisionObject* collisionObject, btScalar timeStep);

	void	resolveSingleConstraintRowGeneric(btSolverBody& bodyA,btSolverBody& bodyB,const btSolverConstraint& contactConstraint);

	void	resolveSingleConstraintRowGenericSIMD(btSolverBody& bodyA,btSolverBody& bodyB,const btSolverConstraint& contactConstraint);
	
	void	resolveSingleConstraintRowLowerLimit(btSolverBody& bodyA,btSolverBody& bodyB,const btSolverConstraint& contactConstraint);
	
	void	resolveSingleConstraintRowLowerLimitSIMD(btSolverBody& bodyA,btSolverBody& bodyB,const btSolverConstraint& contactConstraint);
		
protected:
	
	
	virtual void solveGroupCacheFriendlySplitImpulseIterations(btCollisionObject** bodies,int numBodies,btPersistentManifold** manifoldPtr, int numManifolds,btTypedConstraint** constraints,int numConstraints,const btContactSolverInfo& infoGlobal,btIDebugDraw* debugDrawer);
	virtual btScalar solveGroupCacheFriendlyFinish(btCollisionObject** bodies,int numBodies,const btContactSolverInfo& infoGlobal);
	virtual btScalar solveSingleIteration(int iteration, btCollisionObject** bodies ,int numBodies,btPersistentManifold** manifoldPtr, int numManifolds,btTypedConstraint** constraints,int numConstraints,const btContactSolverInfo& infoGlobal,btIDebugDraw* debugDrawer);

	virtual btScalar solveGroupCacheFriendlySetup(btCollisionObject** bodies,int numBodies,btPersistentManifold** manifoldPtr, int numManifolds,btTypedConstraint** constraints,int numConstraints,const btContactSolverInfo& infoGlobal,btIDebugDraw* debugDrawer);
	virtual btScalar solveGroupCacheFriendlyIterations(btCollisionObject** bodies,int numBodies,btPersistentManifold** manifoldPtr, int numManifolds,btTypedConstraint** constraints,int numConstraints,const btContactSolverInfo& infoGlobal,btIDebugDraw* debugDrawer);


public:

	BT_DECLARE_ALIGNED_ALLOCATOR();
	
	btSequentialImpulseConstraintSolver();
	virtual ~btSequentialImpulseConstraintSolver();

	virtual btScalar solveGroup(btCollisionObject** bodies,int numBodies,btPersistentManifold** manifold,int numManifolds,btTypedConstraint** constraints,int numConstraints,const btContactSolverInfo& info, btIDebugDraw* debugDrawer,btDispatcher* dispatcher);
	

	
	///clear internal cached data and reset random seed
	virtual	void	reset();
	
	unsigned long btRand2();

	int btRandInt2 (int n);

	void	setRandSeed(unsigned long seed)
	{
		m_btSeed2 = seed;
	}
	unsigned long	getRandSeed() const
	{
		return m_btSeed2;
	}

	
	virtual btConstraintSolverType	getSolverType() const
	{
		return BT_SEQUENTIAL_IMPULSE_SOLVER;
	}
};




#endif //BT_SEQUENTIAL_IMPULSE_CONSTRAINT_SOLVER_H

