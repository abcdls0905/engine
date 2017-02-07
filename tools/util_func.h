

#ifndef _GAME_UTIL_FUNC_H
#define _GAME_UTIL_FUNC_H

#include <string>
#include "../public/i_var_list.h"

#ifdef _WIN32
#define RAND_MAX1 (RAND_MAX + 1)
#else
#define RAND_MAX1 (RAND_MAX)
#endif


#ifndef FLT_EPSILON
#define FLT_EPSILON     1.192092896e-07F
#endif

#ifndef PI
#define	PI      (3.14159265f)
#endif

#ifndef PI2
#define PI2     (6.2831853f)
#endif

// 将-pi~pi的弧度值转换为0~2*pi的弧度值
float util_normalize_angle(float angle);

// 求两点的距离
float util_dot_distance(float x1, float y1, float x2, float y2);

#endif

