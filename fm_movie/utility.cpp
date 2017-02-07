
#include "utility.h"
#include "../public/i_entity.h"
#include "../visual/i_time_axis.h"
#include "../public/var_list.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include <string.h>
#define  ERROR_RANGE 0.01f

namespace fm_movie {

void ext_split_string(IVarList& result, const char *str, const char ch)
{
	if (0 == ch)
	{
		return;
	}

	const size_t SIZE1 = ::strlen(str) + 1;

	TAutoMem<char, 256> auto_buf(SIZE1);

	char* buf = auto_buf.GetBuffer();

	::CopyString(buf, SIZE1, str);

	char* s = NULL;
	char* p;

	for (p = buf; *p; ++p)
	{
		if (*p == ch)
		{
			*p = 0;

			if (s)
			{
				result.AddString(s);

				s = NULL;
			}
			else
			{
				result.AddString(p);
			}
		}
		else if (NULL == s)
		{
			s = p;
		}
	}

	if (s)
	{
		result.AddString(s);
	}
	else if (p != buf)
	{
		result.AddString(p);
	}
}

int Util_RunCallback(IEntity* pEntity, const char* event, 
							const IVarList& args)
{
	CVarList res;

	//if (!pEntity->GetCore()->ExecCallback(pEntity, event, args, &res))
	//{
	//	return 0;
	//}

	return res.IntVal(0);
}

// 调用脚本回调
int TimeAxis_RunCallback(ITimeAxis* pTimeAxis, const char* event, const IVarList& args)
{
    time_axis_run_callback func = pTimeAxis->GetCallbackFunc();
    if (!func)
    {
        return 0;
    }

    return func(pTimeAxis, event, args);
}

bool FloatEqualLocal(float v1, float v2)
{
	float temp = v1 - v2;
	return (temp > 0 ? temp : -temp) < ERROR_RANGE;
}

}