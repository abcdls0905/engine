
#ifndef __UTILITY_H__
#define __UTILITY_H__

class IVarList;
class IEntity;
class ITimeAxis;

namespace fm_movie {

// ��ָ�����ŷָ��ַ���
void ext_split_string(IVarList& result, const char *str, const char ch);

// ���ýű��ص�
int Util_RunCallback(IEntity* pEntity, const char* event, const IVarList& args);

// ���ýű��ص�
int TimeAxis_RunCallback(ITimeAxis* pTimeAxis, const char* event, const IVarList& args);

// ����Ƚ�
bool FloatEqualLocal(float v1, float v2);

}

#endif // __UTILITY_H__
