//-------------------------------------------------------------------- 
// 文件名:		linux_lockval.h 
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_LINUX_LOCKVAL_H
#define _SYSTEM_LINUX_LOCKVAL_H

#ifdef __ANDROID__
#include <sys/atomics.h>
class CLockVal
{
public:
	CLockVal()
	{
		m_atom = 0;
	}
    
	// 加一
	int Inc()
	{
		return __atomic_inc(&m_atom);
	}
    
	// 减一
	int Dec()
	{
		return __atomic_dec(&m_atom);
	}
    
	// 设置
	int Set(int val)
	{
		return __atomic_swap(val, &m_atom);
	}
    
	// 获取
	int Get() const
	{
		return m_atom;
	}
    
	// 比较后交换，返回原值(若当前值等于oldv，则将当前值设为newv，并返回oldv)
	int CompareExchange(int oldv, int newv)
	{
        return __atomic_cmpxchg(oldv, newv, &m_atom);
    }
	
public:
	volatile int m_atom;
};
#else
#include <unistd.h>

typedef struct { volatile int counter; } atomic_t;

#ifdef CONFIG_SMP
	#define LOCK_PREFIX \
		".section .smp_locks,\"a\"\n"   \
		"  .align 8\n"                  \
		"  .quad 661f\n" /* address */  \
		".previous\n"                   \
		"661:\n\tlock; "
#else
	#define LOCK_PREFIX ""
#endif // CONFIG_SMP

inline void atomic_inc(atomic_t* v)
{
	__asm__ __volatile__(
		LOCK_PREFIX "incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

inline void atomic_dec(atomic_t* v)
{
	__asm__ __volatile__(
		LOCK_PREFIX "decl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

#define atomic_set(v, i) (((v)->counter) = (i))

#define __xg(x) ((volatile long*)(x))

inline unsigned long __cmpxchg(volatile void *ptr, unsigned long old_value,
	unsigned long new_value, int size)
{
	unsigned long prev;
	
	switch (size)
	{
	case 1:
		__asm__ __volatile__(LOCK_PREFIX "cmpxchgb %b1,%2"
			: "=a"(prev)
			: "q"(new_value), "m"(*__xg(ptr)), "0"(old_value)
			: "memory");
		return prev;
	case 2:
		__asm__ __volatile__(LOCK_PREFIX "cmpxchgw %w1,%2"
			: "=a"(prev)
			: "r"(new_value), "m"(*__xg(ptr)), "0"(old_value)
			: "memory");
		return prev;
	case 4:
		__asm__ __volatile__(LOCK_PREFIX "cmpxchgl %k1,%2"
			: "=a"(prev)
			: "r"(new_value), "m"(*__xg(ptr)), "0"(old_value)
			: "memory");
		return prev;
	case 8:
		__asm__ __volatile__(LOCK_PREFIX "cmpxchgq %1,%2"
			: "=a"(prev)
			: "r"(new_value), "m"(*__xg(ptr)), "0"(old_value)
			: "memory");
		return prev;
	}
	
	return old_value;
}

#define cmpxchg(ptr, o, n)\
	((__typeof__(*(ptr)))__cmpxchg((ptr),(unsigned long)(o),\
	(unsigned long)(n), sizeof(*(ptr))))

#define atomic_cmpxchg(v, oldv, newv) ((int)cmpxchg(&((v)->counter), oldv, newv))

// 带锁的整数

class CLockVal
{
public:
	CLockVal()
	{
		m_atom.counter = 0;
	}

	// 加一
	int Inc()
	{
		atomic_inc(&m_atom);
		return m_atom.counter;
	}

	// 减一
	int Dec()
	{
		atomic_dec(&m_atom);
		return m_atom.counter;
	}

	// 设置
	int Set(int val)
	{
		atomic_set(&m_atom, val);
		return m_atom.counter;
	}

	// 获取
	int Get() const
	{
		return m_atom.counter;
	}

	// 比较后交换，返回原值(若当前值等于oldv，则将当前值设为newv，并返回oldv)
	int CompareExchange(int oldv, int newv)
	{
		return atomic_cmpxchg(&m_atom, oldv, newv);
	}
	
public:
	atomic_t m_atom;
};

#endif // _SYSTEM_LINUX_LOCKVAL_H
#endif