//--------------------------------------------------------------------
// 文件名:		action_data.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月13日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "action_data.h"
#include "skeleton.h"
#include "../visual/i_render.h"
#include "../visual/i_texture.h"
#include "../visual/vis_utils.h"
#include "../public/i_file_sys.h"
#include "../public/i_ini_file.h"
#include "../public/inlines.h"
#include "../public/core_mem.h"
#include "../utils/string_pod.h"

extern IRender* g_pRender;

#define MAX_CHILD_ACTIONFILE 16 // 最大子动作文件数

static inline char* alloc_string(const char* value)
{
	size_t size = strlen(value) + 1;
	char* p = (char*)CORE_ALLOC(size);

	memcpy(p, value, size);

	return p;
}

// 设置触发参数
void set_trigger_name_string(action_trigger_t* pTrigger, const char* pszName,
	const char* pszString)
{
	Assert(pTrigger != NULL);
	Assert(pszName != NULL);
	Assert(pszString != NULL);
	
	size_t size1 = strlen(pszName) + 1;
	size_t size2 = strlen(pszString) + 1;
	size_t size = size1 + size2;
	char* p = (char*)CORE_ALLOC(size);
	
	memcpy(p, pszName, size1);
	memcpy(p + size1, pszString, size2);

	if (pTrigger->pszName)
	{
		CORE_FREE(pTrigger->pszName, pTrigger->nNameSize);
	}

	pTrigger->nNameSize = (unsigned int)size;
	pTrigger->pszName = p;
	pTrigger->pszString = p + size1;
}

// 设置动作节点参数
void set_action_node_string(action_node_t* pActionNode, 
	const char* pszActionName, const char* pszSkeletonName,
	const char* pszActionFileName)
{
	Assert(pszActionName != NULL);
	Assert(pszSkeletonName != NULL);
	Assert(pszActionFileName != NULL);

	// 此时源字符串必然是有效的
	pActionNode->nActionHash = GetHashValue(pszActionName);

	size_t size1 = strlen(pszActionName) + 1;
	size_t size2 = strlen(pszSkeletonName) + 1;
	size_t size3 = strlen(pszActionFileName) + 1;
	size_t size = size1 + size2 + size3;
	char* p = (char*)CORE_ALLOC(size);

	memcpy(p, pszActionName, size1);
	memcpy(p + size1, pszSkeletonName, size2);
	memcpy(p + size1 + size2, pszActionFileName, size3);

	if (pActionNode->pszActionName)
	{
		CORE_FREE(pActionNode->pszActionName, pActionNode->nNameSize);
	}

	pActionNode->nNameSize = (unsigned int)size;
	pActionNode->pszActionName = p;
	pActionNode->pszSkeletonName = p + size1;
	pActionNode->pszActionFileName = p + size1 + size2;
}

// 卸载动作回调触发器列表
static void release_trigger_list(array_t* pTriggerList)
{
	int nTriggerCount = array_get_size(pTriggerList);

	for (int i = 0; i < nTriggerCount; i++)
	{
		action_trigger_t* pTrigger = 
			(action_trigger_t*)array_get(pTriggerList, i);

		if (pTrigger->pszName)
		{
			CORE_FREE(pTrigger->pszName, pTrigger->nNameSize);
		}

		//if (pTrigger->pszString)
		//{
		//	delete[] pTrigger->pszString;
		//}
	}

	array_release(pTriggerList);
}

// 卸载动作集数据
bool unload_action_set(action_set_t* pActionSet)
{
	if (NULL == pActionSet)
	{
		//FXLOG_ERROR("[error 030010] FXUnloadActionSet:Can't unload the "
		//	"action set, the instance is NULL!\n");
		return false;
	}

	//FXLOG_DEVELOPER("FXUnloadActionSet\n");

	if (pActionSet->pActionNodeList != NULL)
	{
		int action_node_num = array_get_size(pActionSet->pActionNodeList);
		
		for (int i = 0; i < action_node_num; ++i)
		{
			action_node_t* pActionNode = (action_node_t*)array_get(
				pActionSet->pActionNodeList, i);

			if (pActionNode->pSkeleton != NULL)
			{
				pActionNode->pSkeleton->Release();
				//pActionNode->pSkeleton = NULL;
			}

			if (pActionNode->pTriggerList != NULL)
			{
				release_trigger_list(pActionNode->pTriggerList);
				//pActionNode->pTriggerList = NULL;
			}

			if (pActionNode->pszActionName != NULL)
			{
				CORE_FREE(pActionNode->pszActionName, pActionNode->nNameSize);
				//pActionNode->pszActionName = NULL;
			}

			//if (pActionNode->pszSkeletonName != NULL)
			//{
			//	delete[] pActionNode->pszSkeletonName;
			//}

			//if (pActionNode->pszActionFileName != NULL)
			//{
			//	delete[] pActionNode->pszActionFileName;
			//}

			if (pActionNode->pFootsteps)
			{
				CORE_FREE(pActionNode->pFootsteps, 
					sizeof(int) * pActionNode->nFootstepNum);
				//pActionNode->pFootsteps = NULL;
			}
		}

		array_release(pActionSet->pActionNodeList);
		//pActionSet->pActionNodeList = NULL;
	}

	if (pActionSet->pMainSkeleton)
	{
		pActionSet->pMainSkeleton->Release();
		//pActionSet->pMainSkeleton = NULL;
	}

	if (pActionSet->pHashBuckets)
	{
		CORE_FREE(pActionSet->pHashBuckets, 
			sizeof(int) * pActionSet->nBucketSize);
		//pActionSet->pHashBuckets = NULL;
	}

	if (pActionSet->pszName)
	{
		CORE_FREE(pActionSet->pszName, strlen(pActionSet->pszName) + 1);
	}

	if (pActionSet->pszMainSkeletonName)
	{
		CORE_FREE(pActionSet->pszMainSkeletonName,
			strlen(pActionSet->pszMainSkeletonName) + 1);
	}

	CORE_FREE(pActionSet, sizeof(action_set_t));

	return true;
}

// 加载动作信息
static bool load_action_node(action_node_t* pActionNode, 
	const char* pszItemName, const char* pszItemValue, 
	const char* pszAppendPath)
{
	if (0 == strcmp(pszItemName, "Skeleton"))
	{
		char pszName[256];

		SafeSprintf(pszName, sizeof(pszName), "%s%s", pszAppendPath, 
			pszItemValue);
		set_action_node_string(pActionNode, pActionNode->pszActionName,
			pszName, pActionNode->pszActionFileName);
    }
    else if (0 == stricmp(pszItemName, "Preload"))
    {
        pActionNode->bPreload = (0 != atoi(pszItemValue));
    }
	else if (0 == strcmp(pszItemName, "EnterTime"))
	{
		pActionNode->fEnterTime = (float)atof(pszItemValue);
	}
	else if (0 == strcmp(pszItemName, "LeaveTime"))
	{
		pActionNode->fLeaveTime = (float)atof(pszItemValue);
	}
	else if (0 == strcmp(pszItemName, "StartFrame"))
	{
		pActionNode->nStartFrame = atoi(pszItemValue);
	}
	else if (0 == strcmp(pszItemName, "EndFrame"))
	{
		pActionNode->nEndFrame = atoi(pszItemValue);
	}
	else if (0 == strcmp(pszItemName, "Footsteps"))
	{
		int step[64];
		int count = 0;
		char buf[256];

		CopyString(buf, sizeof(buf), pszItemValue);
		
		char* p = NULL;

		for (char* s = buf; *s; ++s)
		{
			if (*s == ',')
			{
				*s = 0;
				
				if (p)
				{
					if (count < sizeof(step) / sizeof(int))
					{
						step[count++] = atoi(p);
					}

					p = NULL;
				}
			}
			else
			{
				if (NULL == p)
				{
					p = s;
				}
			}
		}

		if (p)
		{
			if (count < sizeof(step) / sizeof(int))
			{
				step[count++] = atoi(p);
			}
		}

		if (count > 0)
		{
			pActionNode->nFootstepNum = count;
			pActionNode->pFootsteps = (int*)CORE_ALLOC(sizeof(int) * count);
			memcpy(pActionNode->pFootsteps, step, count * sizeof(int));
		}
	}
	else if (0 == strcmp(pszItemName, "EnableStartTrigger"))
	{
		pActionNode->bEnableStartTriggerNotify = true;
	}
	else if (0 == strcmp(pszItemName, "EnableEndTrigger"))
	{
		pActionNode->bEnableEndTriggerNotify = true;
	}
	else if (0 == strcmp(pszItemName, "EnableLoopTrigger"))
	{
		pActionNode->bEnableLoopTriggerNotify = true;
	}
	else if (0 == strcmp(pszItemName, "EnableBreakTrigger"))
	{
		pActionNode->bEnableBreakTriggerNotify = true;
	}
	else if (0 == strcmp(pszItemName, "TranslateAction"))
	{
		pActionNode->nTranslate = atoi(pszItemValue);
	}
	else if (0 == strcmp(pszItemName, "BaseSpeed"))
	{
		pActionNode->fBaseSpeed = (float)atof(pszItemValue);
	}
	else if (pszItemName == strstr(pszItemName, "TRIGGER_"))
	{
		int nTriggerFrame = atoi(pszItemValue);

		if (NULL == pActionNode->pTriggerList)
		{
			pActionNode->pTriggerList = array_create(sizeof(action_trigger_t));
		}

		int nTriggerCount = array_get_size(pActionNode->pTriggerList);
		bool bExist = false;

		for (int t = 0; t < nTriggerCount; ++t)
		{
			action_trigger_t* pTrigger = (action_trigger_t*)
				array_get(pActionNode->pTriggerList, t);

			if (pTrigger != NULL)
			{
				if ((pTrigger->nFrame == nTriggerFrame) 
					&& (0 == strcmp(pTrigger->pszName, pszItemName)))
				{
					bExist = true;
					break;
				}
			}
		}

		if (!bExist)
		{
			action_trigger_t trigger;

			memset(&trigger, 0, sizeof(trigger));
			//trigger.pszName = FXAllocString(pszItemName);
			trigger.nFrame = nTriggerFrame;

			const char* pTemp = strstr(pszItemValue, ",");

			if (pTemp != NULL)
			{
				pTemp++;

				if (*pTemp == ' ')
				{
					pTemp++;
				}

				set_trigger_name_string(&trigger, pszItemName, pTemp);
			}
			else
			{
				set_trigger_name_string(&trigger, pszItemName, "");
			}

			array_push_back(pActionNode->pTriggerList, &trigger);
		}
	}

	return true;
}

// 加载动作集子文件
static bool load_child_action(action_set_t* pActionSet, const char* pszName,
	const char* pszAppendPath, bool bPreload)
{
	Assert(pActionSet != NULL);
	Assert(pszName != NULL);
	Assert(pszAppendPath != NULL);
	
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		g_pRender->GetPathPrefix(pszName), pszName);

	IIniFile* pIni = g_pCore->CreateIniFile(fname);

	if (!pIni->LoadFromFile())
	{
		pIni->Release();
		//FXLOG_ERROR("[error 030012] FXLoadChildAction:Can't load the action, "
		//	"the action file is not exist! %s\n", fname);
		return false;
	}

	//FXLOG_DEVELOPER("FXLoadChildAction:Load child action %s\n", fname);

	size_t section_num = pIni->GetSectionCount();

	if (0 == section_num)
	{
		pIni->Release();
		return false;
	}

	for (size_t sect = 0; sect < section_num; ++sect)
	{
		const char* pszSectionName = pIni->GetSectionByIndex(sect);

		if (StringEmpty(pszSectionName))
		{
			continue;
		}

		action_node_t anode;

		memset(&anode, 0, sizeof(anode));
		//anode.pszActionName = FXAllocString(pszSectionName);
		//anode.pszActionFileName = FXAllocString(pszName);
		anode.fSpeed = 1.0F;
		anode.fScale = 1.0F;
        anode.nHashNext = -1;
        anode.bPreload = bPreload;

		set_action_node_string(&anode, pszSectionName, "", pszName);

		size_t item_num = pIni->GetSectionItemCount(sect);

		for (size_t item = 0; item < item_num; ++item)
		{
			const char* pszItemName = pIni->GetSectionItemKey(sect, item);
			const char* pszItemValue = pIni->GetSectionItemValue(sect, item);

			load_action_node(&anode, pszItemName, pszItemValue, pszAppendPath);
		}

#ifdef _DEBUG
		// 此段代码用来检查动作名是否有冲突
		int action_num = array_get_size(pActionSet->pActionNodeList);

		for (int j = 0; j < action_num; ++j)
		{
			action_node_t* pActionNode = (action_node_t*)array_get(
				pActionSet->pActionNodeList, j);

			if (0 == stricmp(pActionNode->pszActionName, pszSectionName))
			{
				//FXLOG_ERROR("[error 030019] Action Name repeat, "
				//	"The name is %s\n", pszSectionName);

				//if (NULL == pActionNode->pszActionFileName)
				if (0 == pActionNode->pszActionName[0])
				{
					//FXLOG_ERROR("Action Name repeat, Old file is %s\n", 
					//	pActionSet->pszName);
				}
				else
				{
					//FXLOG_ERROR("Action Name repeat, Old file is %s\n", 
					//	pActionNode->pszActionFileName);
				}

				//FXLOG_ERROR("Action Name repeat, Now file is %s\n", fname);
			}
		}
#endif // _DEBUG

		// 用于解决动作集中配置的文件缺失的情况下导致动作队列堵塞的bug
		//if (NULL == anode.pszSkeletonName)
		if (0 == anode.pszSkeletonName[0])
		{
			//FXLOG_ERROR("[error 030061] FXLoadAction:Can't find action [%s] "
			//	"in %s, the action file is not exist!\n", anode.pszActionName, 
			//	pszName);

			if (anode.pTriggerList)
			{
				release_trigger_list(anode.pTriggerList);
				anode.pTriggerList = NULL;
			}

			if (anode.pszActionName)
			{
				CORE_FREE(anode.pszActionName, anode.nNameSize);
				anode.pszActionName = NULL;
			}

			//FXFREEMEM(anode.pszActionFileName);
			//FXFREEMEM(anode.pszActionName);
			continue;
		}

		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", 
			g_pRender->GetPathPrefix(anode.pszSkeletonName), 
			anode.pszSkeletonName);

		if (!g_pCore->GetFileSys()->FileInPack(fname))
		{
			//FXLOG_ERROR("[error 030060] FXLoadChildAction:Can't find[%s] "
			//	"in %s, the file is not exist!\n", anode.pszSkeletonName, 
			//	pszName);

			if (anode.pTriggerList)
			{
				release_trigger_list(anode.pTriggerList);
				anode.pTriggerList = NULL;
			}
			
			if (anode.pszActionName)
			{
				CORE_FREE(anode.pszActionName, anode.nNameSize);
				anode.pszActionName = NULL;
			}

			//FXReleaseTriggerList(anode.pTriggerList);
			//FXFREEMEM(anode.pszActionFileName);
			//FXFREEMEM(anode.pszActionName);
			//FXFREEMEM(anode.pszSkeletonName);
			continue;
		}

		array_push_back(pActionSet->pActionNodeList, &anode);
	}

	pIni->Release();

	return true;
}

// 加载动作集
action_set_t* load_action_set(const char* pszActionSetName, 
	const char* pszAppendPath)
{
	if (NULL == pszActionSetName)
	{
		//FXLOG_ERROR("[error 030011] FXLoadAction:Can't load the action, "
		//	"the action name is NULL!\n");
		return NULL;
	}

	IIniFile* pIni = g_pCore->CreateIniFile(pszActionSetName);

	if (!pIni->LoadFromFile())
	{
		pIni->Release();
		//FXLOG_ERROR("[error 030012] FXLoadAction:Can't load the action, "
		//	"the action file(ini) is not exist! %s\n", pszActionSetName);
		return NULL;
	}

	// 因为可能是后台线程加载，所以不能使用缓冲的文件
	// 尝试读取预加载的缓冲文件
	//size_t file_size;
	//const void* file_data = g_pRender->GetCacheData(pszActionName, file_size);

	//if (file_data)
	//{
	//	file.LoadFromString((const char*)file_data, file_size);
	//}
	//else if (!file.LoadFromFile())
	//{
	//	FXLOG_ERROR("[error 030012] FXLoadAction:Can't load the action, "
	//		"the action file(ini) is not exist! %s\n", pszActionName);
	//	return NULL;
	//}

	//FXLOG_DEVELOPER("FXLoadAction:Load action %s\n", pszActionSetName);

	size_t section_num = pIni->GetSectionCount();

	if (0 == section_num)
	{
		pIni->Release();
		return NULL;
	}

	action_set_t* pActionSet = (action_set_t*)CORE_ALLOC(sizeof(action_set_t));

	memset(pActionSet, 0, sizeof(action_set_t));
	pActionSet->pActionNodeList = array_create(sizeof(action_node_t));
	pActionSet->pszName = alloc_string(pszActionSetName);

	int uMainAction = -1;
	bool bChildFile = false;
	bool bMappingFile = false;
	IIniFile* pMappingIni = NULL;
	const char* section0 = pIni->GetSectionByIndex(0);

	if (0 == stricmp(section0, "ACTION_BASE_FILE"))
	{
		bMappingFile = true;

		const char* strMappingFile = pIni->ReadString("ACTION_BASE_FILE", 
			"File", "");
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s%s", 
			g_pRender->GetPathPrefix(strMappingFile), pszAppendPath, 
			strMappingFile);

		pMappingIni = g_pCore->CreateIniFile(fname);

		if (!pMappingIni->LoadFromFile())
		{
			pMappingIni->Release();
			pIni->Release();
			//CORE_LOG_ERROR("FXLoadAction Mapping File is not exist in "
			//	"file(%s)! %s\n", pszActionSetName, strMappingFile);
			return false;
		}
	}
	else if (0 == stricmp(section0, "ACTION_CHILD_FILE"))
	{
		bChildFile = true;
	}

	// Construct context
	size_t sect = 0;
	
	if (bMappingFile || bChildFile)
	{
		++sect;
	}

	for (; sect < section_num; ++sect)
	{
		const char* pszSectionName = pIni->GetSectionByIndex(sect);

		if (StringEmpty(pszSectionName))
		{
			continue;
		}

		action_node_t anode;

		memset(&anode, 0, sizeof(anode));
		//anode.pszActionName = FXAllocString(pszSectionName);
		anode.fSpeed = 1.0F;
		anode.fScale = 1.0F;
		anode.nHashNext = -1;
		set_action_node_string(&anode, pszSectionName, "", "");

		size_t nItemCount = 0;
		const char* map_section = NULL;
		size_t map_sect_index = 0;

		if (bMappingFile)
		{
			if (pIni->FindItem(pszSectionName, "Mapping"))
			{
				map_section = pIni->ReadString(pszSectionName, "Mapping", "");

				if (!pMappingIni->FindSectionIndex(map_section, 
					map_sect_index))
				{
					continue;
				}

				nItemCount = pMappingIni->GetSectionItemCount(map_sect_index);
			}
		}

		if (NULL == map_section)
		{
			nItemCount = pIni->GetSectionItemCount(sect);
		}

		for (size_t item = 0; item < nItemCount; ++item)
		{
			const char* pszItemName;
			const char* pszItemValue;

			if (NULL == map_section)
			{
				pszItemName = pIni->GetSectionItemKey(sect, item);
				pszItemValue = pIni->GetSectionItemValue(sect, item);
			}
			else
			{
				pszItemName = pMappingIni->GetSectionItemKey(
					map_sect_index, item);
				pszItemValue = pMappingIni->GetSectionItemValue(
					map_sect_index, item);
			}

			if (0 == strcmp(pszItemName, "MainAction"))
			{
				if (-1 == uMainAction)
				{
					uMainAction = array_get_size(pActionSet->pActionNodeList);
				}
				else
				{
					//FXLOG_WARNING("The main action is exist! File is %s\n", 
					//	pszActionSetName);
				}
			}
			else
			{
				load_action_node(&anode, pszItemName, pszItemValue, 
					pszAppendPath);
			}
		}

		// 用于解决动作集中配置的文件缺失的情况下导致动作队列堵塞的bug
		//if (NULL == anode.pszSkeletonName)
		if (0 == anode.pszSkeletonName[0])
		{
			if (uMainAction == array_get_size(pActionSet->pActionNodeList))
			{
				uMainAction = -1;
			}

			//FXLOG_ERROR("[error 030061] FXLoadAction:Can't find action [%s] "
			//	"in %s, the action file is not exist!\n", anode.pszActionName, 
			//	pszActionSetName);

			if (anode.pTriggerList)
			{
				release_trigger_list(anode.pTriggerList);
				anode.pTriggerList = NULL;
			}
			
			if (anode.pszActionName)
			{
				CORE_FREE(anode.pszActionName, anode.nNameSize);
				anode.pszActionName = NULL;
			}

			//FXReleaseTriggerList(anode.pTriggerList);
			//FXFREEMEM(anode.pszActionName);
			continue;
		}

		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", 
			g_pRender->GetPathPrefix(anode.pszSkeletonName), 
			anode.pszSkeletonName);

		// 文件打包时只在文件包中查找以提高效率
		if (!g_pCore->GetFileSys()->FileInPack(fname))
		{
			if (uMainAction == array_get_size(pActionSet->pActionNodeList))
			{
				uMainAction = -1;
			}

			//FXLOG_ERROR("[error 030060] FXLoadAction:Can't find[%s] in %s, "
			//	"the file is not exist!\n", anode.pszSkeletonName, 
			//	pszActionSetName);

			if (anode.pTriggerList)
			{
				release_trigger_list(anode.pTriggerList);
				anode.pTriggerList = NULL;
			}

			if (anode.pszActionName)
			{
				CORE_FREE(anode.pszActionName, anode.nNameSize);
				anode.pszActionName = NULL;
			}

			//FXReleaseTriggerList(anode.pTriggerList);
			//FXFREEMEM(anode.pszActionName);
			//FXFREEMEM(anode.pszSkeletonName);
			continue;
		}

		array_push_back(pActionSet->pActionNodeList, &anode);
	}

	if (bChildFile)
    {
        bool bPreloadFileSection = false;
        typedef TStringPod<char, unsigned int, TStringTraitsCI<char> > PRELOAD_FILE_CONTAINER;
        PRELOAD_FILE_CONTAINER sPreloadFiles;
        // Preload file info
        if (pIni->GetSectionCount() > 1 && strcmp(pIni->GetSectionByIndex(1),"PRELOAD_FILE") == 0)
        {
            bPreloadFileSection = true;
            for (size_t i = 0; i < pIni->GetSectionItemCount(1); ++i)
            {
                const char* pszPreLoadFileName = pIni->GetSectionItemValue(1,i);
                sPreloadFiles.Add(pszPreLoadFileName,i);
            }
        }

		size_t nChildFileCount = pIni->GetSectionItemCount(0);

		for (size_t i = 0; i < nChildFileCount; ++i)
		{
			const char* pszChildFileName = pIni->GetSectionItemValue(0, i);
			char filename[256];

			SafeSprintf(filename, sizeof(filename), "%s%s", pszAppendPath, 
				pszChildFileName);

            bool bPreload = false;
            if (bPreloadFileSection)
            {
                const char* pszChildFileKey = pIni->GetSectionItemKey(0, i);
                unsigned int index;

                if (sPreloadFiles.GetData(pszChildFileKey, index))
                {
                    bPreload = true;
                }
            }

			load_child_action(pActionSet, filename, pszAppendPath, bPreload);
        }

        sPreloadFiles.Clear();
	}

	// Load main action
	pActionSet->pMainSkeleton = NULL;

	if (uMainAction != -1)
	{
		action_node_t* pActionNode = (action_node_t*)array_get(
			pActionSet->pActionNodeList, uMainAction);

		Assert(pActionNode != NULL);
		Assert(pActionNode->pszSkeletonName != NULL);

		pActionSet->pszMainSkeletonName = 
			alloc_string(pActionNode->pszSkeletonName);
	}
	else
	{
		//FXLOG_ERROR("[error 030015] FXLoadAction Not set MainAction in "
		//	"ActionFile's : %s\n", pszActionSetName);
	}

	// 把第一个文件作为主骨骼文件
	if (NULL == pActionSet->pszMainSkeletonName)
	{
		int uActionNodeCount = array_get_size(pActionSet->pActionNodeList);
		
		for (int i = 0; i < uActionNodeCount; ++i)
		{
			action_node_t* pActionNode = (action_node_t*)array_get(
				pActionSet->pActionNodeList, i);

			Assert(pActionNode != NULL);
			Assert(pActionNode->pszSkeletonName != NULL);

			pActionSet->pszMainSkeletonName = 
				alloc_string(pActionNode->pszSkeletonName);
			break;
		}
	}

	// 没有主骨骼文件
	if (NULL == pActionSet->pszMainSkeletonName)
	{
		//FXLOG_ERROR("[error 030017] FXLoadAction MainAction is NULL, "
		//	"Action file is %s\n", pszActionSetName);
		unload_action_set(pActionSet);
		pActionSet = NULL;
	}

	if (pActionSet)
	{
		// 创建动作名的哈希链表
		unsigned int action_num = array_get_size(pActionSet->pActionNodeList);
		
		Assert(action_num > 0);
		
		action_node_t* action_nodes = (action_node_t*)array_get(
			pActionSet->pActionNodeList, 0);
		int* pBuckets = (int*)CORE_ALLOC(sizeof(int) * action_num);
		
		memset(pBuckets, 0xFF, sizeof(int) * action_num);

		for (int k = 0; k < (int)action_num; ++k)
		{
			unsigned int bucket = action_nodes[k].nActionHash % action_num;

			action_nodes[k].nHashNext = pBuckets[bucket];
			pBuckets[bucket] = k;
		}
		
		pActionSet->pHashBuckets = pBuckets;
		pActionSet->nBucketSize = action_num;
	}

	if (pMappingIni)
	{
		pMappingIni->Release();
	}

	pIni->Release();

	return pActionSet;
}

// 保存动作信息到配置文件
bool save_action_node(IIniFile* pFile, action_set_t* pActionSet,
	action_node_t* pActionNode)
{
	const char* action_name = pActionNode->pszActionName;
	
	if (0 == action_name[0])
	{
		return false;
	}

	if (pFile->FindSection(action_name))
	{
		CVarList vItemList;
		int nItemCount = (int)pFile->GetItemList(action_name, vItemList);
		
		for (int i = nItemCount - 1; i >= 0; i--)
		{
			const char* pszItemName = vItemList.StringVal(i);
			
			if (strstr(pszItemName, "TRIGGER_") == pszItemName)
			{
				pFile->DeleteItem(action_name, pszItemName);
			}
		}
	}
	else
	{
		pFile->AddSection(action_name);
	}
	
	pFile->WriteString(action_name, "Skeleton", pActionNode->pszSkeletonName);

	char szTemp[128];

	SafeSprintf(szTemp, sizeof(szTemp), "%f", pActionNode->fEnterTime);

	pFile->WriteString(action_name, "EnterTime", szTemp);

	SafeSprintf(szTemp, sizeof(szTemp), "%f", pActionNode->fLeaveTime);

	pFile->WriteString(action_name, "LeaveTime", szTemp);

	if (pActionNode->nStartFrame > 0)
	{
		pFile->WriteInteger(action_name, "StartFrame", 
			pActionNode->nStartFrame);
	}
	else
	{
		pFile->DeleteItem(action_name, "StartFrame");
	}

	if (pActionNode->nEndFrame > 0)
	{
		pFile->WriteInteger(action_name, "EndFrame", 
			pActionNode->nEndFrame);
	}
	else
	{
		pFile->DeleteItem(action_name, "EndFrame");
	}

	if (pActionNode->nFootstepNum > 0)
	{
		char buf[256];

		buf[0] = 0;

		for (int k = 0; k < pActionNode->nFootstepNum; ++k)
		{
			size_t cur_len = strlen(buf);

			if (k != (pActionNode->nFootstepNum - 1))
			{
				SafeSprintf(buf + cur_len, sizeof(buf) - cur_len, "%d,", 
					pActionNode->pFootsteps[k]);
			}
			else
			{
				SafeSprintf(buf + cur_len, sizeof(buf) - cur_len, "%d", 
					pActionNode->pFootsteps[k]);
			}
		}

		pFile->WriteString(action_name, "Footsteps", buf);
	}
	else
	{
		pFile->DeleteItem(action_name, "Footsteps");
	}

	if (pActionNode->bEnableStartTriggerNotify)
	{
		pFile->WriteString(action_name, "EnableStartTrigger", "1");
	}
	else
	{
		pFile->DeleteItem(action_name, "EnableStartTrigger");
	}

	if (pActionNode->bEnableEndTriggerNotify)
	{
		pFile->WriteString(action_name, "EnableEndTrigger", "1");
	}
	else
	{
		pFile->DeleteItem(action_name, "EnableEndTrigger");
	}

	if (pActionNode->bEnableLoopTriggerNotify)
	{
		pFile->WriteString(action_name, "EnableLoopTrigger", "1");
	}
	else
	{
		pFile->DeleteItem(action_name, "EnableLoopTrigger");
	}

	if (pActionNode->bEnableBreakTriggerNotify)
	{
		pFile->WriteString(action_name, "EnableBreakTrigger", "1");
	}
	else
	{
		pFile->DeleteItem(action_name, "EnableBreakTrigger");
	}

	if (pActionNode->nTranslate)
	{
		pFile->WriteInteger(action_name, "TranslateAction", 
			pActionNode->nTranslate);
	}
	else
	{
		pFile->DeleteItem(action_name, "TranslateAction");
	}

	if (!FloatEqual(pActionNode->fBaseSpeed, 0.0F))
	{
		pFile->WriteFloat(action_name, "BaseSpeed", pActionNode->fBaseSpeed);
	}
	else
	{
		pFile->DeleteItem(action_name, "BaseSpeed");
	}

	if ((pActionSet->pszMainSkeletonName != NULL)
		&& (0 == stricmp(pActionSet->pszMainSkeletonName, 
		pActionNode->pszSkeletonName)))
	{
		pFile->WriteString(action_name, "MainAction", "1");
	}
	else
	{
		pFile->DeleteItem(action_name, "MainAction");
	}

	if (pActionNode->pTriggerList != NULL)
	{
		int trigger_num = array_get_size(pActionNode->pTriggerList);

		for (int i = 0; i < trigger_num; ++i)
		{
			action_trigger_t* pTrigger = 
				(action_trigger_t*)array_get(pActionNode->pTriggerList, i);

			if (pTrigger->pszName != NULL)
			{
				if ((NULL == pTrigger->pszString) 
					|| (pTrigger->pszString[0] == 0))
				{
					SafeSprintf(szTemp, sizeof(szTemp), "%d", 
						pTrigger->nFrame);
				}
				else
				{
					SafeSprintf(szTemp, sizeof(szTemp), "%d,%s", 
						pTrigger->nFrame, pTrigger->pszString);
				}

				pFile->WriteString(action_name, pTrigger->pszName, szTemp);
			}
		}
	}
	
	return true;
}

// 保存动作集配置文件
bool save_action_set(action_set_t* pActionSet, const char* pszName)
{
	Assert(pActionSet != NULL);
	Assert(pszName != NULL);

	bool bRet = true;
	char strPathName[256];

	SafeSprintf(strPathName, sizeof(strPathName), "%s%s",
		g_pRender->GetPathPrefix(pszName), pszName);

	IIniFile* pIni = g_pCore->CreateIniFile(strPathName);
	char* child_name[MAX_CHILD_ACTIONFILE] = { NULL };
	IIniFile* pChildIni[MAX_CHILD_ACTIONFILE] = { NULL }; 
	int action_num = array_get_size(pActionSet->pActionNodeList);

	for (int k = 0; k < action_num; ++k)
	{
		action_node_t* pActionNode = (action_node_t*)array_get(
			pActionSet->pActionNodeList, k);

		if (NULL == pActionNode)
		{
			continue;
		}

		//if (NULL == pActionNode->pszActionFileName)
		if (0 == pActionNode->pszActionFileName[0])
		{
			// 不是子文件里的动作
			continue;
		}

		IIniFile* pFile = NULL;

		for (int i = 0; i < MAX_CHILD_ACTIONFILE; ++i)
		{
			if (child_name[i] != NULL)
			{
				if (0 == strcmp(child_name[i], pActionNode->pszActionFileName))
				{
					pFile = pChildIni[i];
					break;
				}
			}
			else
			{
				child_name[i] = pActionNode->pszActionFileName;

				char fname[256];

				SafeSprintf(fname, sizeof(fname), "%s%s",
					g_pRender->GetPathPrefix(child_name[i]), child_name[i]);
				pChildIni[i] = g_pCore->CreateIniFile(fname);
				//pChildIni[i]->SetFileName(fname);
				pFile = pChildIni[i];
				break;
			}
		}

		if (NULL == pFile)
		{
			//FXLOG_ERROR("[error 030022] FXACTION_Save: Action child file "
			//	"MaxCount is %d!\n", MAX_CHILD_ACTIONFILE);
			continue;
		}

		save_action_node(pFile, pActionSet, pActionNode);
	}

	if (child_name[0] != NULL)
	{
		// 保存子文件列表
		pIni->AddSection("ACTION_CHILD_FILE");

		for (int i = 0; i < MAX_CHILD_ACTIONFILE; ++i)
		{
			if (NULL == child_name[i])
			{
				break;
			}
			
			char key[32];

			SafeSprintf(key, sizeof(key), "%d", i);

			pIni->WriteString("ACTION_CHILD_FILE", key, child_name[i]);

			// 写入子动作文件
			if (!pChildIni[i]->SaveToFile())
			{
				bRet = false;
			}

			pChildIni[i]->Release();
		}
	}

	// 保存主文件里的动作
	for (int k = 0; k < action_num; ++k)
	{
		action_node_t* pActionNode = (action_node_t*)array_get(
			pActionSet->pActionNodeList, k);

		if (NULL == pActionNode)
		{
			continue;
		}

		//if (pActionNode->pszActionFileName != NULL)
		if (pActionNode->pszActionFileName[0] != 0)
		{
			// 是子文件里的动作
			continue;
		}

		save_action_node(pIni, pActionSet, pActionNode);
	}

	if (!pIni->SaveToFile())
	{
		bRet = false;
	}

	pIni->Release();

	return bRet;
}

