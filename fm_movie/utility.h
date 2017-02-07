
#ifndef __UTILITY_H__
#define __UTILITY_H__

class IVarList;
class IEntity;
class ITimeAxis;

namespace fm_movie {

// 用指定符号分割字符串
void ext_split_string(IVarList& result, const char *str, const char ch);

// 调用脚本回调
int Util_RunCallback(IEntity* pEntity, const char* event, const IVarList& args);

// 调用脚本回调
int TimeAxis_RunCallback(ITimeAxis* pTimeAxis, const char* event, const IVarList& args);

// 浮点比较
bool FloatEqualLocal(float v1, float v2);

}

#endif // __UTILITY_H__
