
#include "../public/auto_mem.h"
#include "../public/var_list.h"
#include "util_func.h"
#include <string>
#include <math.h>
#include <time.h>
#include <vector>
#include <algorithm>

float util_normalize_angle(float angle)
{
  float value = ::fmod(angle, PI2);
  if (value < 0)
  {
    value = value + PI2;
  }

  if (value < FLT_EPSILON && ::fabs(angle) > PI)
  {
    value = PI2;
  }
  return value;
}

float util_dot_distance(float x1, float y1, float x2, float y2)
{
  float sx = x1 - x2;
  float sy = y1 - y2;

  return ::sqrt(sx * sx + sy * sy);
}
