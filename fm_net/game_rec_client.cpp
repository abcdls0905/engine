//--------------------------------------------------------------------
// 文件名:		game_rec_client.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月19日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "game_receiver.h"
#include "game_client.h"
#include "game_object.h"
#include "game_scene.h"
#include "game_scene_obj.h"
#include "game_view.h"
#include "game_view_obj.h"
#include "game_prop_set.h"
#include "game_record.h"
#include "game_record_set.h"
#include "../public/outer_msg.h"
#include "../visual/i_game_client.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/i_solo_stub.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif

#include "../visual/i_event_manager.h"

#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime

// 客户端数据相关消息接收

GameSceneObj* GameReceiver::GetSceneObj(const char* ident)
{
	Assert(ident != NULL);
	
	if (NULL == m_pClient)
	{
		return NULL;
	}

	GameScene* pScene = m_pClient->GetCurrentScene();

	if (NULL == pScene)
	{
		return NULL;
	}

	return pScene->GetSceneObjByIdent(ident);
}

GameView* GameReceiver::GetView(const char* view_ident)
{
	Assert(view_ident != NULL);

	if (NULL == m_pClient)
	{
		return NULL;
	}

	return m_pClient->GetViewByIdent(view_ident);
}

GameViewObj* GameReceiver::GetViewObj(const char* view_ident, 
	const char* item_ident)
{
	Assert(view_ident != NULL);
	Assert(item_ident != NULL);

	if (NULL == m_pClient)
	{
		return NULL;
	}

	GameView* pView = m_pClient->GetViewByIdent(view_ident);

	if (NULL == pView)
	{
		return NULL;
	}

	return pView->GetViewObjByIdent(item_ident);
}

bool GameReceiver::RecvProperty(GameObject* pObj, CLoadArchive& ar, 
	size_t count, bool change)
{
	try
	{
		for (size_t i = 0; i < count; ++i)
		{
			unsigned short index = ar.ReadWord();

			if (index >= m_PropertyTable.size())
			{
				return false;
			}
			
			const char* name = m_PropertyTable[index]->strName.c_str();
			int type = m_PropertyTable[index]->nType;

			m_PropertyTable[index]->nCount++;

			TVar<4, TCoreAlloc> value;

			switch (type)
			{
			case OUTER_TYPE_BYTE:
				value.SetInt(ar.ReadByte());
				break;
			case OUTER_TYPE_WORD:
				value.SetInt(ar.ReadWord());
				break;
			case OUTER_TYPE_DWORD:
				value.SetInt(ar.ReadInt());
				break;
			case OUTER_TYPE_QWORD:
				value.SetInt64(ar.ReadInt64());
				break;
			case OUTER_TYPE_FLOAT:
				value.SetFloat(ar.ReadFloat());
				break;
			case OUTER_TYPE_DOUBLE:
				value.SetDouble(ar.ReadDouble());
				break;
			case OUTER_TYPE_STRING:
				value.SetString(ar.ReadString());
				break;
			case OUTER_TYPE_WIDESTR:
				value.SetWideStr(ar.ReadWideStr());
				break;
			case OUTER_TYPE_OBJECT:
				value.SetObject(ar.ReadObject());
				break;
			default:
				CORE_TRACE("(GameReceiver::RecvProperty)type error");
				return false;
			}

			if (NULL == pObj)
			{
				continue;
			}

			CGamePropSet* pPropSet = pObj->GetPropSet();
			size_t k;
				
			if (pPropSet->FindIndex(name, k))
			{
				pPropSet->SetValue(k, value);
			}
			else
			{
				pPropSet->Add(name, value);
			}

			if (change)
			{
                pObj->PropertyChanged(name);

				// 属性发生变化
                if (m_pMsgHandler)
	            {
                    m_pMsgHandler->OnObjectPropertyChange(
						pObj->GetIdentQuick(), name);
	            }
                else
                {
				    event_send(GetID(), "on_object_property_change", 
				        CVarList() << pObj->GetIdentQuick() << name);
                }
			}
		}
	}
	catch (CExceptMsg& e)
	{
		CORE_TRACE("(GameReceiver::RecvProperty)get message error");
		CORE_TRACE(e.GetMsg());
		return false;
	}

	return true;
}

// 接收容器对像属数据
bool GameReceiver::RecvViewObjProperty(GameObject* pObj, CLoadArchive& ar,
									   size_t count, bool change, 
									   const char* view_ident, const char* item_ident)
{
	try
	{
		for (size_t i = 0; i < count; ++i)
		{
			unsigned short index = ar.ReadWord();

			if (index >= m_PropertyTable.size())
			{
				return false;
			}

			const char* name = m_PropertyTable[index]->strName.c_str();
			int type = m_PropertyTable[index]->nType;

			m_PropertyTable[index]->nCount++;

			TVar<4, TCoreAlloc> value;

			switch (type)
			{
			case OUTER_TYPE_BYTE:
				value.SetInt(ar.ReadByte());
				break;
			case OUTER_TYPE_WORD:
				value.SetInt(ar.ReadWord());
				break;
			case OUTER_TYPE_DWORD:
				value.SetInt(ar.ReadInt());
				break;
			case OUTER_TYPE_QWORD:
				value.SetInt64(ar.ReadInt64());
				break;
			case OUTER_TYPE_FLOAT:
				value.SetFloat(ar.ReadFloat());
				break;
			case OUTER_TYPE_DOUBLE:
				value.SetDouble(ar.ReadDouble());
				break;
			case OUTER_TYPE_STRING:
				value.SetString(ar.ReadString());
				break;
			case OUTER_TYPE_WIDESTR:
				value.SetWideStr(ar.ReadWideStr());
				break;
			case OUTER_TYPE_OBJECT:
				value.SetObject(ar.ReadObject());
				break;
			default:
				CORE_TRACE("(GameReceiver::RecvProperty)type error");
				return false;
			}

			if (NULL == pObj)
			{
				continue;
			}

			CGamePropSet* pPropSet = pObj->GetPropSet();
			size_t k;

			if (pPropSet->FindIndex(name, k))
			{
				pPropSet->SetValue(k, value);
			}
			else
			{
				pPropSet->Add(name, value);
			}

			if (change)
			{
				pObj->PropertyChanged(name);

				// 属性发生变化
				if (m_pMsgHandler)
				{
					//m_pMsgHandler->OnViewObjPropertyChange(view_ident, 
					//item_ident, name);
				}
				else
				{
					event_send(GetID(), "on_view_object_property_change", 
						CVarList() << view_ident << item_ident << name);
				}
			}
		}
	}
	catch (CExceptMsg& e)
	{
		CORE_TRACE("(GameReceiver::RecvProperty)get message error");
		CORE_TRACE(e.GetMsg());
		return false;
	}

	return true;
}

bool GameReceiver::RecvRecordRow(CGameRecord* pRec, size_t index, 
	CLoadArchive& ar, size_t row, size_t count)
{
	Assert(pRec != NULL);

	size_t col_count = m_RecordTable[index]->nCols;
	
	try
	{
		for (size_t r = row; r < (row + count); ++r)
		{
			// 增加一行
			pRec->InsertRow(r);

			for (size_t c = 0; c < col_count; ++c)
			{
				TVar<4, TCoreAlloc> value;
				
				switch (m_RecordTable[index]->ColTypes[c])
				{
				case OUTER_TYPE_BYTE:
					value.SetInt(ar.ReadByte());
					break;
				case OUTER_TYPE_WORD:
					value.SetInt(ar.ReadWord());
					break;
				case OUTER_TYPE_DWORD:
					value.SetInt(ar.ReadInt());
					break;
				case OUTER_TYPE_QWORD:
					value.SetInt64(ar.ReadInt64());
					break;
				case OUTER_TYPE_FLOAT:
					value.SetFloat(ar.ReadFloat());
					break;
				case OUTER_TYPE_DOUBLE:
					value.SetDouble(ar.ReadDouble());
					break;
				case OUTER_TYPE_STRING:
					value.SetString(ar.ReadString());
					break;
				case OUTER_TYPE_WIDESTR:
					value.SetWideStr(ar.ReadWideStr());
					break;
				case OUTER_TYPE_OBJECT:
					value.SetObject(ar.ReadObject());
					break;
				default:
					CORE_TRACE("(GameReceiver::RecvRecordRow)type error");
					CORE_TRACE(pRec->GetName());
					return false;
				}

				if (!pRec->SetValue(r, c, value))
				{
					CORE_TRACE("(GameReceiver::RecvRecordRow)set value error");
					CORE_TRACE(pRec->GetName());
					return false;
				}
			}
		}
	}
	catch (CExceptMsg& e)
	{
		CORE_TRACE("(GameReceiver::RecvRecordRow)get message error");
		CORE_TRACE(e.GetMsg());
		CORE_TRACE(pRec->GetName());
		return false;
	}
	
	return true;
}

bool GameReceiver::RecvRecordGrid(CGameRecord* pRec, int is_viewobj, 
	int nIdent, int nSerial, size_t index, CLoadArchive& ar, size_t count)
{
	Assert(pRec != NULL);
	
	try
	{
		for (size_t i = 0; i < count; ++i)
		{
			unsigned int row = ar.ReadWord();
			unsigned int col = ar.ReadByte();

			if (col >= m_RecordTable[index]->nCols)
			{
				CORE_TRACE("(GameReceiver::RecvRecordGrid)col error");
				CORE_TRACE(pRec->GetName());
				return false;
			}
			
			TVar<4, TCoreAlloc> value;
			
			switch (m_RecordTable[index]->ColTypes[col])
			{
			case OUTER_TYPE_BYTE:
				value.SetInt(ar.ReadByte());
				break;
			case OUTER_TYPE_WORD:
				value.SetInt(ar.ReadWord());
				break;
			case OUTER_TYPE_DWORD:
				value.SetInt(ar.ReadInt());
				break;
			case OUTER_TYPE_QWORD:
				value.SetInt64(ar.ReadInt64());
				break;
			case OUTER_TYPE_FLOAT:
				value.SetFloat(ar.ReadFloat());
				break;
			case OUTER_TYPE_DOUBLE:
				value.SetDouble(ar.ReadDouble());
				break;
			case OUTER_TYPE_STRING:
				value.SetString(ar.ReadString());
				break;
			case OUTER_TYPE_WIDESTR:
				value.SetWideStr(ar.ReadWideStr());
				break;
			case OUTER_TYPE_OBJECT:
				value.SetObject(ar.ReadObject());
				break;
			default:
				CORE_TRACE("(GameReceiver::RecvRecordGrid)type error");
				CORE_TRACE(pRec->GetName());
				return false;
			}

			if (!pRec->SetValue(row, col, value))
			{
				CORE_TRACE("(GameReceiver::RecvRecordGrid)set value error");
				CORE_TRACE(pRec->GetName());
				return false;
			}

            if (is_viewobj == 0)
			{
				char ident[32];

				SafeSprintf(ident, sizeof(ident), "%u-%u", nIdent, nSerial);

				// 将某行某列的更新加入回调
				if (m_pMsgHandler)
				{
					m_pMsgHandler->OnRecordSingleGrid(ident, pRec->GetName(),
						row, col);
				}
				else
				{
					event_send(GetID(), "on_record_single_grid", 
						CVarList() << ident << pRec->GetName() << row << col);
				}
			}
			else if (is_viewobj == 1)
			{
				char view_ident[32];
				char item_ident[32];

				SafeSprintf(view_ident, sizeof(view_ident), "%d", nIdent);
				SafeSprintf(item_ident, sizeof(item_ident), "%d", nSerial);

				// 将某行某列的更新加入回调
				if (m_pMsgHandler)
				{
					m_pMsgHandler->OnViewObjRecordSingleGrid(view_ident,
						item_ident, pRec->GetName(), row, col);
				}
				else
				{
					event_send(GetID(), 
						"on_viewobj_record_single_grid", 
						CVarList() << view_ident << item_ident 
						<< pRec->GetName() << row << col);
				}
			}
			else if (is_viewobj == 2)
            {
                // 将某行某列的更新加入回调
                if (m_pMsgHandler)
				{
					m_pMsgHandler->OnSceneRecordSingleGrid(pRec->GetName(),
						row, col);
				}
				else
				{
					event_send(GetID(), 
						"on_scene_record_single_grid", 
						CVarList() << pRec->GetName() << row << col);
				}
            }
			else if (is_viewobj == 3)
			{
				char view_ident[32];

				SafeSprintf(view_ident, sizeof(view_ident), "%d", nIdent);

				// 将某行某列的更新加入回调
				if (m_pMsgHandler)
				{
					m_pMsgHandler->OnViewRecordSingleGrid(view_ident,
						pRec->GetName(), row, col);
				}
				else
				{
					event_send(GetID(), 
						"on_view_record_single_grid", 
						CVarList() << view_ident << pRec->GetName() << row 
						<< col);
				}
			}
		}
	}
	catch (CExceptMsg& e)
	{
		CORE_TRACE("(GameReceiver::RecvRecordGrid)get message error");
		CORE_TRACE(e.GetMsg());
		CORE_TRACE(pRec->GetName());
		return false;
	}

	return true;
}

bool GameReceiver::AddRecord(GameObject* pObj, size_t index)
{
	if (index >= m_RecordTable.size())
	{
		return false;
	}
	
	rec_data_t* data = m_RecordTable[index];
	CGameRecord* pRecord = CORE_NEW(CGameRecord);
	
	pRecord->SetName(data->strName.c_str());
	pRecord->SetColCount(data->nCols);
	
	for (size_t c = 0; c < data->nCols; ++c)
	{
		int type = data->ColTypes[c];

		switch (type)
		{
		case OUTER_TYPE_BYTE:
		case OUTER_TYPE_WORD:
		case OUTER_TYPE_DWORD:
			pRecord->SetColType(c, VTYPE_INT);
			break;
		case OUTER_TYPE_QWORD:
			pRecord->SetColType(c, VTYPE_INT64);
			break;
		case OUTER_TYPE_FLOAT:
			pRecord->SetColType(c, VTYPE_FLOAT);
			break;
		case OUTER_TYPE_DOUBLE:
			pRecord->SetColType(c, VTYPE_DOUBLE);
			break;
		case OUTER_TYPE_STRING:
			pRecord->SetColType(c, VTYPE_STRING);
			break;
		case OUTER_TYPE_WIDESTR:
			pRecord->SetColType(c, VTYPE_WIDESTR);
			break;
		case OUTER_TYPE_OBJECT:
			pRecord->SetColType(c, VTYPE_OBJECT);
			break;
		default:
			Assert(0);
			break;
		}
	}
	
	pObj->GetRecordSet()->Add(pRecord);
	
	return true;
}

bool GameReceiver::ServerEntryScene(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_entry_scene_t))
	{
		CORE_TRACE("(GameReceiver::ServerEntryScene)size error");
		return false;
	}

	server_msg_entry_scene_t* pmsg = (server_msg_entry_scene_t*)pdata;
	char ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);

	if (m_pClient)
	{
		GameScene* pScene = m_pClient->CreateScene(ident);

		if (pScene)
		{
			CLoadArchive arLoad(
				(char*)pdata + sizeof(server_msg_entry_scene_t),
				len - sizeof(server_msg_entry_scene_t));

			if (!RecvProperty(pScene, arLoad, pmsg->nCount, false))
			{
				CORE_TRACE("(GameReceiver::ServerEntryScene)property error");
			}
		}
		else
		{			
			CORE_TRACE("(GameReceiver::ServerEntryScene)no scene");
		}
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnEntryScene(ident, pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_entry_scene", 
			CVarList() << ident << pmsg->nCount);
	}
	
	return true;
}

bool GameReceiver::ServerEntrySceneEx(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_entry_scene_ex_t))
	{
		CORE_TRACE("(GameReceiver::ServerEntrySceneEx)size error");
		return false;
	}

	server_msg_entry_scene_ex_t* pmsg = (server_msg_entry_scene_ex_t*)pdata;

	char ident[32];
	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);

	if (m_pClient)
	{
		GameScene* pScene = m_pClient->CreateScene(ident);

		if (pScene)
		{
			CLoadArchive arLoad(
				(char*)pdata + sizeof(server_msg_entry_scene_ex_t),
				len - sizeof(server_msg_entry_scene_ex_t));

			if (!RecvProperty(pScene, arLoad, pmsg->nCount, false))
			{
				CORE_TRACE("(GameReceiver::ServerEntrySceneEx)property error");
			}
		}
		else
		{			
			CORE_TRACE("(GameReceiver::ServerEntrySceneEx)no scene");
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnEntrySceneEx(ident, pmsg->nCount, pmsg->scene_motion_mode_, pmsg->frame_time_delta_,
			pmsg->move_line_diff_ratio_, pmsg->move_rotate_diff_ratio_);
	}
	else
	{
		event_send(GetID(), "on_entry_scene_ex", 
			CVarList() << ident << pmsg->nCount << pmsg->scene_motion_mode_);
	}

	return true;
}

bool GameReceiver::ServerExitScene(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_exit_scene_t))
	{
		CORE_TRACE("(GameReceiver::ServerExitScene)size error");
		return false;
	}

	server_msg_exit_scene_t* pmsg = (server_msg_exit_scene_t*)pdata;

	if (m_pClient)
	{
		m_pClient->DeleteScene();
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnExitScene();
	}
	else
	{
		event_send(GetID(), "on_exit_scene", CVarList());
	}
	
	return true;
}

bool GameReceiver::ServerAddObject(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_add_object_t))
	{
		CORE_TRACE("(GameReceiver::ServerAddObject)size error");
		return false;
	}

	server_msg_add_object_t* pmsg = (server_msg_add_object_t*)pdata;
	char ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);

	if (m_pClient)
	{
		GameScene* pScene = m_pClient->GetCurrentScene();

		if (pScene)
		{
			GameSceneObj* pObj = pScene->AddSceneObj(ident);

			// 属性
			if (pmsg->nCount > 0)
			{
				CLoadArchive arLoad(
					(char*)pdata + sizeof(server_msg_add_object_t),
					len - sizeof(server_msg_add_object_t));

				if (!RecvProperty(pObj, arLoad, pmsg->nCount, false))
				{
					CORE_TRACE("(GameReceiver::ServerAddObject)property error");
				}
			}
			
			// 位置
			pObj->SetLocation(pmsg->posi.X, pmsg->posi.Y, pmsg->posi.Z, 
				pmsg->posi.Orient);
			pObj->SetDestination(pmsg->dest.X, pmsg->dest.Y, pmsg->dest.Z, 
				pmsg->dest.Orient, pmsg->dest.MoveSpeed, 
				pmsg->dest.RotateSpeed, pmsg->dest.JumpSpeed, 
				pmsg->dest.Gravity);
			pObj->SetMode(pmsg->dest.Mode);
		}
		else
		{			
			CORE_TRACE("(GameReceiver::ServerAddObject)no scene");
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnAddObject(ident, pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_add_object", 
			CVarList() << ident << pmsg->nCount);
	}
	
	return true;
}

bool GameReceiver::ServerRemoveObject(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_remove_object_t))
	{
		CORE_TRACE("(GameReceiver::ServerRemoveObject)size error");
		return false;
	}

	server_msg_remove_object_t* pmsg = (server_msg_remove_object_t*)pdata;
	char ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);

    if (m_pMsgHandler)
	{
		m_pMsgHandler->OnBeforeRemoveObject(ident);
	}
	else
	{
		event_send(GetID(), "on_before_remove_object", 
			CVarList() << ident);
	}

	if (m_pClient)
	{
		GameScene* pScene = m_pClient->GetCurrentScene();
		
		if (pScene)
		{
			pScene->RemoveSceneObj(ident);
		}
		else
		{			
			CORE_TRACE("(GameReceiver::ServerRemoveObject)no scene");
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnRemoveObject(ident);
	}
	else
	{
		event_send(GetID(), "on_remove_object", CVarList() << ident);
	}

	return true;
}

bool GameReceiver::ServerSceneProperty(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_scene_property_t))
	{
		CORE_TRACE("(GameReceiver::ServerSceneProperty)size error");
		return false;
	}

	server_msg_scene_property_t* pmsg = (server_msg_scene_property_t*)pdata;
	
	if (m_pClient)
	{
		GameScene* pScene = m_pClient->GetCurrentScene();
		
		if (pScene)
		{
			CLoadArchive arLoad(
				(char*)pdata + sizeof(server_msg_scene_property_t),
				len - sizeof(server_msg_scene_property_t));
			
			if (!RecvProperty(pScene, arLoad, pmsg->nCount, true))
			{
				CORE_TRACE(
					"(GameReceiver::ServerSceneProperty)property error");
			}
		}
		else
		{			
			CORE_TRACE("(GameReceiver::ServerSceneProperty)no scene");
		}
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnSceneProperty(pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_scene_property", 
			CVarList() << pmsg->nCount);
	}
	
	return true;
}

bool GameReceiver::ServerObjectProperty(int src,const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_object_property_t))
	{
		CORE_TRACE("(GameReceiver::ServerObjectProperty)size error");
		return false;
	}

	server_msg_object_property_t* pmsg = (server_msg_object_property_t*)pdata;

	if(m_pSoloStub && m_pSoloStub->IsRunning() && src == 1)
	{
		CORE_TRACE_EX("(GameReceiver::ServerCustom)Main player in local clone scene Ident:%u-%u,not dispose", pmsg->ObjectId.nIdent, 
			pmsg->ObjectId.nSerial);
		return false;
	}	

	if (pmsg->nIsViewObj == 0)
	{
		char ident[32];

		SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
			pmsg->ObjectId.nSerial);

		if(m_pSoloStub && m_pSoloStub->IsRunning() && src == 1)
		{
			CORE_TRACE_EX("(GameReceiver::ServerCustom)Main player in local clone scene Ident:%s,not dispose",ident);
			return false;
		}

		if (m_pClient)
		{
			GameSceneObj* pObj = GetSceneObj(ident);

			if (pObj)
			{
				CLoadArchive arLoad(
					(char*)pdata + sizeof(server_msg_object_property_t),
					len - sizeof(server_msg_object_property_t));
				
				if (!RecvProperty(pObj, arLoad, pmsg->nCount, true))
				{
					CORE_TRACE(
						"(GameReceiver::ServerObjectProperty)property error");
				}
			}
			else
			{
				CORE_TRACE_EX("(GameReceiver::ServerObjectProperty)no object:%s",ident);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnObjectProperty(ident, pmsg->nCount);
		}
		else
		{
			event_send(GetID(), "on_object_property", 
				CVarList() << ident << int(pmsg->nCount));
		}
	}
	else
	{
		char view_ident[32];
		char item_ident[32];

		SafeSprintf(view_ident, sizeof(view_ident), "%d", 
			pmsg->ObjectId.nIdent);
		SafeSprintf(item_ident, sizeof(item_ident), "%d", 
			pmsg->ObjectId.nSerial);

		if (m_pClient)
		{
			GameViewObj* pObj = GetViewObj(view_ident, item_ident);

			if (pObj)
			{
				CLoadArchive arLoad(
					(char*)pdata + sizeof(server_msg_object_property_t),
					len - sizeof(server_msg_object_property_t));

				if (!RecvViewObjProperty(pObj, arLoad, pmsg->nCount, true, view_ident, item_ident))
				{
					CORE_TRACE(
						"(GameReceiver::ServerObjectProperty)property error");
				}
			}
			else
			{
				CORE_TRACE_EX("(GameReceiver::ServerObjectProperty)no object view:%s item:%s",view_ident,item_ident);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewObjProperty(view_ident, item_ident, 
				pmsg->nCount);
		}
		else
		{
			event_send(GetID(), "on_viewobj_property", 
				CVarList() << view_ident << item_ident << int(pmsg->nCount));
		}
	}
	
	return true;
}

bool GameReceiver::ServerCreateView(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_create_view_t))
	{
		CORE_TRACE("(GameReceiver::ServerCreateView)size error");
		return false;
	}

	server_msg_create_view_t* pmsg = (server_msg_create_view_t*)pdata;
	char view_ident[32];

	SafeSprintf(view_ident, sizeof(view_ident), "%d", pmsg->nViewId);

	if (m_pClient)
	{
		GameView* pView = m_pClient->CreateView(view_ident, pmsg->nCapacity);
		
		if (pView)
		{
			CLoadArchive arLoad(
				(char*)pdata + sizeof(server_msg_create_view_t),
				len - sizeof(server_msg_create_view_t));

			if (!RecvProperty(pView, arLoad, pmsg->nCount, false))
			{
				CORE_TRACE("(GameReceiver::ServerCreateView)property error");
			}
		}
		else
		{
			CORE_TRACE("(GameReceiver::ServerCreateView)no view");
		}
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnCreateView(view_ident, pmsg->nCapacity, pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_create_view", 
			CVarList() << view_ident << int(pmsg->nCapacity) 
			<< int(pmsg->nCount));
	}
	
	return true;
}

bool GameReceiver::ServerDeleteView(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_delete_view_t))
	{
		CORE_TRACE("(GameReceiver::ServerDeleteView)size error");
		return false;
	}

	server_msg_delete_view_t* pmsg = (server_msg_delete_view_t*)pdata;
	char view_ident[32];

	SafeSprintf(view_ident, sizeof(view_ident), "%d", pmsg->nViewId);

	if (m_pClient)
	{
		if (!m_pClient->DeleteView(view_ident))
		{
			CORE_TRACE("(GameReceiver::ServerDeleteView)delete failed");
		}
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnDeleteView(view_ident);
	}
	else
	{
		event_send(GetID(), "on_delete_view", 
			CVarList() << view_ident);
	}
	
	return true;
}

bool GameReceiver::ServerViewProperty(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_view_property_t))
	{
		CORE_TRACE("(GameReceiver::ServerViewProperty)size error");
		return false;
	}

	server_msg_view_property_t* pmsg = (server_msg_view_property_t*)pdata;
	char view_ident[32];

	SafeSprintf(view_ident, sizeof(view_ident), "%d", pmsg->nViewId);

	if (m_pClient)
	{
		GameView* pView = m_pClient->GetViewByIdent(view_ident);

		if (pView)
		{
			CLoadArchive arLoad(
				(char*)pdata + sizeof(server_msg_view_property_t),
				len - sizeof(server_msg_view_property_t));
			
			if (!RecvProperty(pView, arLoad, pmsg->nCount, true))
			{
				CORE_TRACE("(GameReceiver::ServerViewProperty)property error");
			}
		}
		else
		{
			CORE_TRACE("(GameReceiver::ServerViewProperty)no view");
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnViewProperty(view_ident, pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_view_property", 
			CVarList() << view_ident << int(pmsg->nCount));
	}
	
	return true;
}

bool GameReceiver::ServerViewAdd(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_view_add_t))
	{
		CORE_TRACE("(GameReceiver::ServerViewAdd)size error");
		return false;
	}

	server_msg_view_add_t* pmsg = (server_msg_view_add_t*)pdata;
	char view_ident[32];
	char item_ident[32];

	SafeSprintf(view_ident, sizeof(view_ident), "%d", pmsg->nViewId);
	SafeSprintf(item_ident, sizeof(item_ident), "%d", pmsg->nObjectId);

	if (m_pClient)
	{
		GameView* pView = m_pClient->GetViewByIdent(view_ident);
		
		if (pView)
		{
			GameViewObj* pObj = pView->AddViewObj(item_ident);
			CLoadArchive arLoad(
				(char*)pdata + sizeof(server_msg_view_add_t),
				len - sizeof(server_msg_view_add_t));

			if (!RecvProperty(pObj, arLoad, pmsg->nCount, false))
			{
				CORE_TRACE("(GameReceiver::ServerViewAdd)property error");
			}
		}
		else
		{
			CORE_TRACE("(GameReceiver::ServerViewAdd)no view");
		}
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnViewAdd(view_ident, item_ident, pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_view_add", 
			CVarList() << view_ident << item_ident << int(pmsg->nCount));
	}
	
	return true;
}

bool GameReceiver::ServerViewRemove(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_view_remove_t))
	{
		CORE_TRACE("(GameReceiver::ServerViewRemove)size error");
		return false;
	}

	server_msg_view_remove_t* pmsg = (server_msg_view_remove_t*)pdata;
	char view_ident[32];
	char item_ident[32];

	SafeSprintf(view_ident, sizeof(view_ident), "%d", pmsg->nViewId);
	SafeSprintf(item_ident, sizeof(item_ident), "%d", pmsg->nObjectId);

	if (m_pClient)
	{
		GameView* pView = m_pClient->GetViewByIdent(view_ident);
		
		if (pView)
		{
			if (!pView->RemoveViewObj(item_ident))
			{
				CORE_TRACE("(GameReceiver::ServerViewRemove)remove failed");
			}
		}
		else
		{
			CORE_TRACE("(GameReceiver::ServerViewRemove)no view");
		}
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnViewRemove(view_ident, item_ident);
	}
	else
	{
		event_send(GetID(), "on_view_remove", 
			CVarList() << view_ident << item_ident);
	}
	
	return true;
}

bool GameReceiver::ServerViewChange(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_view_change_t))
	{
		CORE_TRACE("(GameReceiver::ServerViewChange)size error");
		return false;
	}

	server_msg_view_change_t* pmsg = (server_msg_view_change_t*)pdata;
	char view_ident[32];
	char old_item_ident[32];
	char new_item_ident[32];

	SafeSprintf(view_ident, sizeof(view_ident), "%d", pmsg->nViewId);
	SafeSprintf(old_item_ident, sizeof(old_item_ident), "%d", 
		pmsg->nOldObjectId);
	SafeSprintf(new_item_ident, sizeof(new_item_ident), "%d", 
		pmsg->nNewObjectId);

	if (m_pClient)
	{
		GameView* pView = m_pClient->GetViewByIdent(view_ident);

		if (pView)
		{
			if (!pView->ChangeViewObj(old_item_ident, new_item_ident))
			{
				CORE_TRACE("(GameReceiver::ServerViewChange)change failed");
			}
		}
		else
		{
			CORE_TRACE("(GameReceiver::ServerViewChange)no view");
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnViewChange(view_ident, old_item_ident, 
			new_item_ident);
	}
	else
	{
		event_send(GetID(), "on_view_change", 
			CVarList() << view_ident << old_item_ident << new_item_ident);
	}

	return true;
}

bool GameReceiver::ServerRecordAddRow(int src,const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_record_add_row_t))
	{
		CORE_TRACE("(GameReceiver::ServerRecordAddRow)size error");
		return false;
	}

	server_msg_record_add_row_t* pmsg = (server_msg_record_add_row_t*)pdata;

	if(m_pSoloStub && m_pSoloStub->IsRunning() && src == 1)
	{
		CORE_TRACE_EX("(GameReceiver::ServerRecordAddRow)Main player in local clone scene Ident:%u-%u,not dispose", pmsg->ObjectId.nIdent, 
			pmsg->ObjectId.nSerial);
		return false;
	}

	const char* record_name = GetRecordName(pmsg->nIndex);

	double dtime = performance_time();

	if (pmsg->nIsViewObj == 0)
	{
		char ident[32];

		SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
			pmsg->ObjectId.nSerial);

		if (m_pClient)
		{
			GameSceneObj* pObj = GetSceneObj(ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);
				
				if (NULL == pRecord)
				{
					if (!AddRecord(pObj, pmsg->nIndex))
					{
						CORE_TRACE("(GameReceiver::ServerRecordAddRow)add failed");
						CORE_TRACE(record_name);
					}
					
					pRecord = pObj->GetRecordSet()->Get(record_name);
				}
				
				if (pRecord)
				{
					CLoadArchive arLoad(
						(char*)pdata + sizeof(server_msg_record_add_row_t),
						len - sizeof(server_msg_record_add_row_t));

					if (!RecvRecordRow(pRecord, pmsg->nIndex, arLoad, pmsg->nRow, 
						pmsg->nRows))
					{
						CORE_TRACE("(GameReceiver::ServerRecordAddRow)recv failed");
						CORE_TRACE(record_name);
					}

					pObj->RecordChanged(record_name, "add");
				}
			}
			else
			{
				CORE_TRACE_EX("(GameReceiver::ServerRecordAddRow)no object:%s",ident);
				CORE_TRACE(record_name);
			}
		}
		
		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnRecordAddRow(ident, record_name, pmsg->nRow, 
				pmsg->nRows);
		}
		else
		{
			event_send(GetID(), "on_record_add_row", 
				CVarList() << ident << record_name << pmsg->nRow 
				<< pmsg->nRows);
		}
	}
	else if (pmsg->nIsViewObj == 3)
	{
		char view_ident[32];

		SafeSprintf(view_ident, sizeof(view_ident), "%d", 
			pmsg->ObjectId.nIdent);

		if (m_pClient)
		{
			GameView* pObj = GetView(view_ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (NULL == pRecord)
				{
					if (!AddRecord(pObj, pmsg->nIndex))
					{
						CORE_TRACE("(GameReceiver::ServerRecordAddRow)add failed");
						CORE_TRACE(record_name);
					}

					pRecord = pObj->GetRecordSet()->Get(record_name);
				}

				if (pRecord)
				{
					CLoadArchive arLoad(
						(char*)pdata + sizeof(server_msg_record_add_row_t),
						len - sizeof(server_msg_record_add_row_t));

					if (!RecvRecordRow(pRecord, pmsg->nIndex, arLoad, 
						pmsg->nRow, pmsg->nRows))
					{
						CORE_TRACE("(GameReceiver::ServerRecordAddRow)recv failed");
						CORE_TRACE(record_name);
					}

					pObj->RecordChanged(record_name, "add");
				}
			}
			else
			{
				CORE_TRACE_EX("(GameReceiver::ServerRecordAddRow)no object view ident:%s",view_ident);
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewRecordAddRow(view_ident, record_name, 
				pmsg->nRow, pmsg->nRows);
		}
		else
		{
			event_send(GetID(), "on_view_record_add_row", 
				CVarList() << view_ident << record_name << pmsg->nRow 
				<< pmsg->nRows);
		}
	}
	else if (pmsg->nIsViewObj == 1)
	{
		char view_ident[32];
		char item_ident[32];

		SafeSprintf(view_ident, sizeof(view_ident), "%d", 
			pmsg->ObjectId.nIdent);
		SafeSprintf(item_ident, sizeof(item_ident), "%d", 
			pmsg->ObjectId.nSerial);

		if (m_pClient)
		{
			GameViewObj* pObj = GetViewObj(view_ident, item_ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (NULL == pRecord)
				{
					if (!AddRecord(pObj, pmsg->nIndex))
					{
						CORE_TRACE("(GameReceiver::ServerRecordAddRow)add failed");
						CORE_TRACE(record_name);
					}

					pRecord = pObj->GetRecordSet()->Get(record_name);
				}

				if (pRecord)
				{
					CLoadArchive arLoad(
						(char*)pdata + sizeof(server_msg_record_add_row_t),
						len - sizeof(server_msg_record_add_row_t));

					if (!RecvRecordRow(pRecord, pmsg->nIndex, arLoad, pmsg->nRow, 
						pmsg->nRows))
					{
						CORE_TRACE("(GameReceiver::ServerRecordAddRow)recv failed");
						CORE_TRACE(record_name);
					}

					pObj->RecordChanged(record_name, "add");
				}
			}
			else
			{
				CORE_TRACE_EX("(GameReceiver::ServerRecordAddRow)no object view ident:%s,item ident:%s",view_ident,item_ident);
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewObjRecordAddRow(view_ident, item_ident,
				record_name, pmsg->nRow, pmsg->nRows);
		}
		else
		{
			event_send(GetID(), "on_viewobj_record_add_row", 
				CVarList() << view_ident << item_ident << record_name 
				<< pmsg->nRow << pmsg->nRows);
		}
	}
	else if (pmsg->nIsViewObj == 2)
	{
		if (m_pClient)
		{
			GameScene* pObj = m_pClient->GetCurrentScene();

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (NULL == pRecord)
				{
					if (!AddRecord(pObj, pmsg->nIndex))
					{
						CORE_TRACE("(GameReceiver::ServerRecordAddRow)add failed");
						CORE_TRACE(record_name);
					}

					pRecord = pObj->GetRecordSet()->Get(record_name);
				}

				if (pRecord)
				{
					CLoadArchive arLoad(
						(char*)pdata + sizeof(server_msg_record_add_row_t),
						len - sizeof(server_msg_record_add_row_t));

					if (!RecvRecordRow(pRecord, pmsg->nIndex, arLoad, 
						pmsg->nRow, pmsg->nRows))
					{
						CORE_TRACE("(GameReceiver::ServerRecordAddRow)recv failed");
						CORE_TRACE(record_name);
					}

					pObj->RecordChanged(record_name, "add");
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerRecordAddRow)no object form current scene");
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnSceneRecordAddRow(record_name, pmsg->nRow, 
				pmsg->nRows);
		}
		else
		{
			event_send(GetID(), "on_scene_record_add_row", 
				CVarList() << record_name << pmsg->nRow << pmsg->nRows);
		}
	}

    dtime = performance_time() - dtime;

	if(strlen(record_name) != 0)
	{
		m_RecordTable[pmsg->nIndex]->nRecordAddRowCount++;
		m_RecordTable[pmsg->nIndex]->dRecordAddRowTime+=dtime;
	}

	return true;
}

bool GameReceiver::ServerRecordDelRow(int src,const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_record_remove_row_t))
	{
		CORE_TRACE("(GameReceiver::ServerRecordDelRow)size error");
		return false;
	}

	server_msg_record_remove_row_t* pmsg = 
		(server_msg_record_remove_row_t*)pdata;
	
	if(m_pSoloStub && m_pSoloStub->IsRunning()&& src == 1)
	{
		CORE_TRACE_EX("(GameReceiver::ServerRecordDelRow)Main player in local clone scene Ident:%u-%u,not dispose", pmsg->ObjectId.nIdent, 
			pmsg->ObjectId.nSerial);
		return false;
	}

	const char* record_name = GetRecordName(pmsg->nIndex);
	double dtime = performance_time();

	if (pmsg->nIsViewObj == 0)
	{
		char ident[32];

		SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
			pmsg->ObjectId.nSerial);

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnRecordBeforeRemoveRow(ident, record_name,
				pmsg->nRow);
		}
		else
		{
			event_send(GetID(), "on_record_before_remove_row", 
				CVarList() << ident << record_name << pmsg->nRow);
		}

		if (m_pClient)
		{
			GameSceneObj* pObj = GetSceneObj(ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);
				
				if (pRecord)
				{
					if (!pRecord->DeleteRow(pmsg->nRow))
					{
						CORE_TRACE("(GameReceiver::ServerRecordDelRow)"
							"delete row failed");
						CORE_TRACE(record_name);
					}
					
					pObj->RecordChanged(record_name, "del");
				}
				else
				{
					CORE_TRACE("(GameReceiver::ServerRecordDelRow)no record");
					CORE_TRACE(record_name);
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerRecordDelRow)no object");
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnRecordRemoveRow(ident, record_name, pmsg->nRow);
		}
		else
		{
			event_send(GetID(), "on_record_remove_row", 
				CVarList() << ident << record_name << pmsg->nRow);
		}
	}
	else if (pmsg->nIsViewObj == 1)
	{
		char view_ident[32];
		char item_ident[32];

		SafeSprintf(view_ident, sizeof(view_ident), "%d", 
			pmsg->ObjectId.nIdent);
		SafeSprintf(item_ident, sizeof(item_ident), "%d", 
			pmsg->ObjectId.nSerial);

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewObjRecordBeforeRemoveRow(view_ident, 
				item_ident,	record_name, pmsg->nRow);
		}
		else
		{
			event_send(GetID(), "on_viewobj_record_before_remove_row", 
				CVarList() << view_ident << item_ident << record_name 
				<< pmsg->nRow);
		}

		if (m_pClient)
		{
			GameViewObj* pObj = GetViewObj(view_ident, item_ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (pRecord)
				{
					if (!pRecord->DeleteRow(pmsg->nRow))
					{
						CORE_TRACE("(GameReceiver::ServerRecordDelRow)"
							"delete row failed");
						CORE_TRACE(record_name);
					}

					pObj->RecordChanged(record_name, "del");
				}
				else
				{
					CORE_TRACE("(GameReceiver::ServerRecordDelRow)no record");
					CORE_TRACE(record_name);
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerRecordDelRow)no object");
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewObjRecordRemoveRow(view_ident, item_ident,
				record_name, pmsg->nRow);
		}
		else
		{
			event_send(GetID(), "on_viewobj_record_remove_row", 
				CVarList() << view_ident << item_ident << record_name 
				<< pmsg->nRow);
		}
	}
	else if (pmsg->nIsViewObj == 2)
	{
		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnSceneRecordBeforeRemoveRow(record_name, 
				pmsg->nRow);
		}
		else
		{
			event_send(GetID(), "on_scene_record_before_remove_row", 
				CVarList() << record_name << pmsg->nRow);
		}

		if (m_pClient)
		{
			GameScene* pObj = m_pClient->GetCurrentScene();

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (pRecord)
				{
					if (!pRecord->DeleteRow(pmsg->nRow))
					{
						CORE_TRACE("(GameReceiver::ServerRecordDelRow)"
							"delete row failed");
						CORE_TRACE(record_name);
					}

					pObj->RecordChanged(record_name, "del");
				}
				else
				{
					CORE_TRACE("(GameReceiver::ServerRecordDelRow)no record");
					CORE_TRACE(record_name);
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerRecordDelRow)no object");
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnSceneRecordRemoveRow(record_name, pmsg->nRow);
		}
		else
		{
			event_send(GetID(), "on_scene_record_remove_row", 
				CVarList() << record_name << pmsg->nRow);
		}
	}
	else if (pmsg->nIsViewObj == 3)
	{
		char view_ident[32];

		SafeSprintf(view_ident, sizeof(view_ident), "%d", 
			pmsg->ObjectId.nIdent);

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewRecordBeforeRemoveRow(view_ident, 
				record_name, pmsg->nRow);
		}
		else
		{
			event_send(GetID(), "on_view_record_before_remove_row", 
				CVarList() << view_ident << record_name << pmsg->nRow);
		}

		if (m_pClient)
		{
			GameView* pObj = GetView(view_ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (pRecord)
				{
					if (!pRecord->DeleteRow(pmsg->nRow))
					{
						CORE_TRACE("(GameReceiver::ServerRecordDelRow)"
							"delete row failed");
						CORE_TRACE(record_name);
					}

					pObj->RecordChanged(record_name, "del");
				}
				else
				{
					CORE_TRACE("(GameReceiver::ServerRecordDelRow)no record");
					CORE_TRACE(record_name);
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerRecordDelRow)no object");
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewRecordRemoveRow(view_ident, record_name, 
				pmsg->nRow);
		}
		else
		{
			event_send(GetID(), "on_view_record_remove_row", 
				CVarList() << view_ident << record_name << pmsg->nRow);
		}
	}

    dtime = performance_time()-dtime;

	if(strlen(record_name) != 0)
	{
		m_RecordTable[pmsg->nIndex]->nRecordDelRowCount++;
		m_RecordTable[pmsg->nIndex]->dRecordDelRowTime+=dtime;
	}
	
	return true;
}

bool GameReceiver::ServerRecordGrid(int src,const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_record_grid_t))
	{
		CORE_TRACE("(GameReceiver::ServerRecordGrid)size error");
		return false;
	}

	server_msg_record_grid_t* pmsg = (server_msg_record_grid_t*)pdata;
	
	if(m_pSoloStub && m_pSoloStub->IsRunning() && src == 1)
	{
		CORE_TRACE_EX("(GameReceiver::ServerRecordGrid)Main player in local clone scene Ident:%u-%u,not dispose", pmsg->ObjectId.nIdent, 
			pmsg->ObjectId.nSerial);
		return false;
	}

	const char* record_name = GetRecordName(pmsg->nIndex);
	double dtime = performance_time();

	if (pmsg->nIsViewObj == 0)
	{
		char ident[32];

		SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
			pmsg->ObjectId.nSerial);

		if (m_pClient)
		{
			GameSceneObj* pObj = GetSceneObj(ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);
				
				if (pRecord)
				{
					CLoadArchive arLoad(
						(char*)pdata + sizeof(server_msg_record_grid_t),
						len - sizeof(server_msg_record_grid_t));

					if (!RecvRecordGrid(pRecord, pmsg->nIsViewObj, 
						pmsg->ObjectId.nIdent, pmsg->ObjectId.nSerial, 
						pmsg->nIndex, arLoad, pmsg->nCount))
					{
						CORE_TRACE("(GameReceiver::ServerRecordGrid)recv failed");
						CORE_TRACE(record_name);
					}
					
					pObj->RecordChanged(record_name, "grid");
				}
				else
				{
					CORE_TRACE("(GameReceiver::ServerRecordGrid)no record");
					CORE_TRACE(record_name);
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerRecordGrid)no object");
				CORE_TRACE(record_name);
			}
		}
		
		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnRecordGrid(ident, record_name, pmsg->nCount);
		}
		else
		{
			event_send(GetID(), "on_record_grid", 
				CVarList() << ident << record_name << pmsg->nCount);
		}
	}
	else if (pmsg->nIsViewObj == 1)
	{
		char view_ident[32];
		char item_ident[32];

		SafeSprintf(view_ident, sizeof(view_ident), "%d", 
			pmsg->ObjectId.nIdent);
		SafeSprintf(item_ident, sizeof(item_ident), "%d", 
			pmsg->ObjectId.nSerial);

		if (m_pClient)
		{
			GameViewObj* pObj = GetViewObj(view_ident, item_ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (pRecord)
				{
					CLoadArchive arLoad(
						(char*)pdata + sizeof(server_msg_record_grid_t),
						len - sizeof(server_msg_record_grid_t));

					if (!RecvRecordGrid(pRecord, pmsg->nIsViewObj, 
						pmsg->ObjectId.nIdent, pmsg->ObjectId.nSerial, 
						pmsg->nIndex, arLoad, pmsg->nCount))
					{
						CORE_TRACE("(GameReceiver::ServerRecordGrid)recv failed");
						CORE_TRACE(record_name);
					}

					pObj->RecordChanged(record_name, "grid");
				}
				else
				{
					CORE_TRACE("(GameReceiver::ServerRecordGrid)no record");
					CORE_TRACE(record_name);
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerRecordGrid)no object");
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewObjRecordGrid(view_ident, item_ident, 
				record_name, pmsg->nCount);
		}
		else
		{
			event_send(GetID(), "on_viewobj_record_grid", 
				CVarList() << view_ident << item_ident << record_name 
				<< pmsg->nCount);
		}
	}
	else if (pmsg->nIsViewObj == 2)
	{
		if (m_pClient)
		{
			GameScene* pObj = m_pClient->GetCurrentScene();

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (pRecord)
				{
					CLoadArchive arLoad(
						(char*)pdata + sizeof(server_msg_record_grid_t),
						len - sizeof(server_msg_record_grid_t));

					if (!RecvRecordGrid(pRecord, pmsg->nIsViewObj, 
						pmsg->ObjectId.nIdent, pmsg->ObjectId.nSerial, 
						pmsg->nIndex, arLoad, pmsg->nCount))
					{
						CORE_TRACE("(GameReceiver::ServerRecordGrid)recv failed");
						CORE_TRACE(record_name);
					}

					pObj->RecordChanged(record_name, "grid");
				}
				else
				{
					CORE_TRACE("(GameReceiver::ServerRecordGrid)no record");
					CORE_TRACE(record_name);
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerRecordGrid)no object");
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnSceneRecordGrid(record_name, pmsg->nCount);
		}
		else
		{
			event_send(GetID(), "on_scene_record_grid", 
				CVarList() << record_name << pmsg->nCount);
		}
	}
	else if (pmsg->nIsViewObj == 3)
	{
		char view_ident[32];

		SafeSprintf(view_ident, sizeof(view_ident), "%d", 
			pmsg->ObjectId.nIdent);

		if (m_pClient)
		{
			GameView* pObj = GetView(view_ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (pRecord)
				{
					CLoadArchive arLoad(
						(char*)pdata + sizeof(server_msg_record_grid_t),
						len - sizeof(server_msg_record_grid_t));

					if (!RecvRecordGrid(pRecord, pmsg->nIsViewObj, 
						pmsg->ObjectId.nIdent, pmsg->ObjectId.nSerial, 
						pmsg->nIndex, arLoad, pmsg->nCount))
					{
						CORE_TRACE("(GameReceiver::ServerRecordGrid)recv failed");
						CORE_TRACE(record_name);
					}

					pObj->RecordChanged(record_name, "grid");
				}
				else
				{
					CORE_TRACE("(GameReceiver::ServerRecordGrid)no record");
					CORE_TRACE(record_name);
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerRecordGrid)no object");
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewRecordGrid(view_ident, record_name, 
				pmsg->nCount);
		}
		else
		{
			event_send(GetID(), "on_view_record_grid", 
				CVarList() << view_ident << record_name << pmsg->nCount);
		}
	}

    dtime = performance_time() - dtime;

	if(strlen(record_name) != 0)
	{
		m_RecordTable[pmsg->nIndex]->nRecordGridCount++;
		m_RecordTable[pmsg->nIndex]->dRecordGridTime+=dtime;
	}

	return true;
}

bool GameReceiver::ServerRecordClear(int src,const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_record_clear_t))
	{
		CORE_TRACE("(GameReceiver::ServerRecordClear)size error");
		return false;
	}

	server_msg_record_clear_t* pmsg = (server_msg_record_clear_t*)pdata;

	if(m_pSoloStub && m_pSoloStub->IsRunning()&& src == 1)
	{
		CORE_TRACE_EX("(GameReceiver::ServerRecordClear)Main player in local clone scene Ident:%u-%u,not dispose", pmsg->ObjectId.nIdent, 
			pmsg->ObjectId.nSerial);
		return false;
	}

	const char* record_name = GetRecordName(pmsg->nIndex);

	double dtime = performance_time();

	//记录表格清空数目
	
	if (pmsg->nIsViewObj == 0)
	{
		char ident[32];

		SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
			pmsg->ObjectId.nSerial);

		if (m_pClient)
		{
			GameSceneObj* pObj = GetSceneObj(ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (pRecord)
				{
					pRecord->ClearRow();
					pObj->RecordChanged(record_name, "clear");
				}
				else
				{
					if (!AddRecord(pObj, pmsg->nIndex))
					{
						CORE_TRACE("(GameReceiver::ServerRecordClear)add failed");
						CORE_TRACE(record_name);
					}
				}
			}
			else
			{
				CORE_TRACE_EX("(GameReceiver::ServerRecordClear)no object:%s",ident);
				CORE_TRACE(record_name);
			}
		}
		
		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnRecordClear(ident, record_name);
		}
		else
		{
			event_send(GetID(), "on_record_clear", 
				CVarList() << ident << record_name);
		}
	}
	else if (pmsg->nIsViewObj == 1)
	{
		char view_ident[32];
		char item_ident[32];

		SafeSprintf(view_ident, sizeof(view_ident), "%d", 
			pmsg->ObjectId.nIdent);
		SafeSprintf(item_ident, sizeof(item_ident), "%d", 
			pmsg->ObjectId.nSerial);

		if (m_pClient)
		{
			GameViewObj* pObj = GetViewObj(view_ident, item_ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (pRecord)
				{
					pRecord->ClearRow();

					pObj->RecordChanged(record_name, "clear");
				}
				else
				{
					if (!AddRecord(pObj, pmsg->nIndex))
					{
						CORE_TRACE("(GameReceiver::ServerRecordClear)add failed");
						CORE_TRACE(record_name);
					}
				}
			}
			else
			{
				CORE_TRACE_EX("(GameReceiver::ServerRecordClear)no object view ident:%s,item ident:%s",view_ident,item_ident);
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewObjRecordClear(view_ident, item_ident,
				record_name);
		}
		else
		{
			event_send(GetID(), "on_viewobj_record_clear", 
				CVarList() << view_ident << item_ident << record_name);
		}
	}
	else if (pmsg->nIsViewObj == 2)
	{
		if (m_pClient)
		{
			GameScene* pObj = m_pClient->GetCurrentScene();

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (pRecord)
				{
					pRecord->ClearRow();

					pObj->RecordChanged(record_name, "clear");
				}
				else
				{
					if (!AddRecord(pObj, pmsg->nIndex))
					{
						CORE_TRACE("(GameReceiver::ServerRecordClear)add failed");
						CORE_TRACE(record_name);
					}
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerRecordClear)no object form current scene");
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnSceneRecordClear(record_name);
		}
		else
		{
			event_send(GetID(), "on_scene_record_clear", 
				CVarList() << record_name);
		}
	}
	else if (pmsg->nIsViewObj == 3)
	{
		char view_ident[32];

		SafeSprintf(view_ident, sizeof(view_ident), "%d", 
			pmsg->ObjectId.nIdent);

		if (m_pClient)
		{
			GameView* pObj = GetView(view_ident);

			if (pObj)
			{
				CGameRecord* pRecord = pObj->GetRecordSet()->Get(record_name);

				if (pRecord)
				{
					pRecord->ClearRow();
					pObj->RecordChanged(record_name, "clear");
				}
				else
				{
					if (!AddRecord(pObj, pmsg->nIndex))
					{
						CORE_TRACE("(GameReceiver::ServerRecordClear)add failed");
						CORE_TRACE(record_name);
					}
				}
			}
			else
			{
				CORE_TRACE_EX("(GameReceiver::ServerRecordClear)no object view ident:%s",view_ident);
				CORE_TRACE(record_name);
			}
		}

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnViewRecordClear(view_ident, record_name);
		}
		else
		{
			event_send(GetID(), "on_view_record_clear", 
				CVarList() << view_ident << record_name);
		}
	}
    
	dtime = performance_time() - dtime;

	if(strlen(record_name) != 0)
	{
		m_RecordTable[pmsg->nIndex]->nRecordClearCount++;
		m_RecordTable[pmsg->nIndex]->dRecordClearTime+=dtime;
	}
	
	return true;
}

bool GameReceiver::ServerLocation(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_location_t))
	{
		CORE_TRACE("(GameReceiver::ServerLocation)size error");
		return false;
	}

	server_msg_location_t* pmsg = (server_msg_location_t*)pdata;

	char ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);

	if (m_pClient)
	{
		GameSceneObj* pObj = GetSceneObj(ident);
		
		if (pObj)
		{
			pObj->SetLocation(pmsg->posi.X, pmsg->posi.Y, pmsg->posi.Z, 
				pmsg->posi.Orient);
			pObj->SetDestination(pmsg->posi.X, pmsg->posi.Y, pmsg->posi.Z,
				pmsg->posi.Orient, 0.0F, 0.0F, 0.0F, 0.0F);
		}
		else
		{
			CORE_TRACE("(GameReceiver::ServerLocation)no object");
		}
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnLocation(ident);
	}
	else
	{
		event_send(GetID(), "on_location", CVarList() << ident);
	}
	
	return true;
}

bool GameReceiver::ServerMoving(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_moving_t))
	{
		CORE_TRACE("(GameReceiver::ServerMoving)size error");
		return false;
	}

	server_msg_moving_t* pmsg = (server_msg_moving_t*)pdata;

	char ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);

	if (m_pClient)
	{
		GameSceneObj* pObj = GetSceneObj(ident);

		if (pObj)
		{
			pObj->SetDestination(pmsg->dest.X, pmsg->dest.Y, pmsg->dest.Z, 
				pmsg->dest.Orient, pmsg->dest.MoveSpeed, pmsg->dest.RotateSpeed, 
				pmsg->dest.JumpSpeed, pmsg->dest.Gravity);
			pObj->SetMode(pmsg->dest.Mode);
		}
		else
		{
			CORE_TRACE("(GameReceiver::ServerMoving)no object");
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnMoving(ident);
	}
	else
	{
		event_send(GetID(), "on_moving", CVarList() << ident);
	}

	return true;
}

bool GameReceiver::ServerMovingEx(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_moving_ex_t))
	{
		CORE_TRACE("(GameReceiver::ServerMovingEx)size error");
		return false;
	}

	server_msg_moving_ex_t* pmsg = (server_msg_moving_ex_t*)pdata;

	char ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);

	if (m_pClient)
	{
		GameSceneObj* pObj = GetSceneObj(ident);

		if (NULL == pObj)
		{
			// 可能是因为视野消息优化发送的信息，不需要报错 
			return false;
		}

		pObj->SetDestination(pmsg->dest.X, pmsg->dest.Y, pmsg->dest.Z, 
			pmsg->dest.Orient, pmsg->dest.MoveSpeed, pmsg->dest.RotateSpeed, 
			pmsg->dest.JumpSpeed, pmsg->dest.Gravity);
		pObj->SetMode(pmsg->dest.Mode);
		pObj->SetDelay(pmsg->nDelayTime);
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnMoving(ident);
	}
	else
	{
		event_send(GetID(), "on_moving", CVarList() << ident);
	}

	return true;
}

bool GameReceiver::ServerAllDest(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_all_dest_t))
	{
		CORE_TRACE("(GameReceiver::ServerAllDest)size error");
		return false;
	}

	server_msg_all_dest_t* pmsg = (server_msg_all_dest_t*)pdata;

	if (m_pClient)
	{
		GameScene* pScene = m_pClient->GetCurrentScene();
		
		if (pScene)
		{
			for (size_t i = 0; i < pmsg->nCount; ++i)
			{
				char ident[32];

				SafeSprintf(ident, sizeof(ident), "%u-%u", 
					pmsg->info[i].ObjectId.nIdent, 
					pmsg->info[i].ObjectId.nSerial);

				GameSceneObj* pObj = pScene->GetSceneObjByIdent(ident);

				if (pObj)
				{
					outer_dest_t* pDest = &(pmsg->info[i].dest);
					
					pObj->SetDestination(pDest->X, pDest->Y, pDest->Z,
						pDest->Orient, pDest->MoveSpeed, pDest->RotateSpeed,
						pDest->JumpSpeed, pDest->Gravity);
					pObj->SetMode(pDest->Mode);
					
					if (m_pMsgHandler)
					{
						m_pMsgHandler->OnMoving(ident);
					}
					else
					{
						event_send(GetID(), "on_moving", 
							CVarList() << ident);
					}
				}
				else
				{
					CORE_TRACE("(GameReceiver::ServerAllDest)no object");
				}
			}
		}
		else
		{			
			CORE_TRACE("(GameReceiver::ServerAllDest)no scene");
		}
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnAllDestination(pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_all_dest", 
			CVarList() << pmsg->nCount);
	}
	
	return true;
}

bool GameReceiver::ServerAllDestEx(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_all_dest_ex_t))
	{
		CORE_TRACE("(GameReceiver::ServerAllDestEx)size error");
		return false;
	}

	server_msg_all_dest_ex_t* pmsg = (server_msg_all_dest_ex_t*)pdata;

	if (m_pClient)
	{
		GameScene* pScene = m_pClient->GetCurrentScene();

		if (pScene)
		{
			for (size_t i = 0; i < pmsg->nCount; ++i)
			{
				char ident[32];

				SafeSprintf(ident, sizeof(ident), "%u-%u", 
					pmsg->info[i].ObjectId.nIdent, 
					pmsg->info[i].ObjectId.nSerial);

				GameSceneObj* pObj = pScene->GetSceneObjByIdent(ident);

				if (NULL == pObj)
				{
					continue;
				}

				outer_dest_t* pDest = &(pmsg->info[i].dest);

				pObj->SetDestination(pDest->X, pDest->Y, pDest->Z,
					pDest->Orient, pDest->MoveSpeed, pDest->RotateSpeed,
					pDest->JumpSpeed, pDest->Gravity);
				pObj->SetMode(pDest->Mode);
				pObj->SetDelay(pmsg->info[i].nDelayTime);

				if (m_pMsgHandler)
				{
					m_pMsgHandler->OnMoving(ident);
				}
				else
				{
					event_send(GetID(), "on_moving", 
						CVarList() << ident);
				}
			}
		}
		else
		{			
			CORE_TRACE("(GameReceiver::ServerAllDest)no scene");
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnAllDestination(pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_all_dest", 
			CVarList() << pmsg->nCount);
	}

	return true;
}

bool GameReceiver::ServerLinkTo(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_link_to_t))
	{
		CORE_TRACE("(GameReceiver::ServerLinkTo)size error");
		return false;
	}

	server_msg_link_to_t* pmsg = (server_msg_link_to_t*)pdata;
	char ident[32];
	char link_ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);
	SafeSprintf(link_ident, sizeof(link_ident), "%u-%u", pmsg->LinkId.nIdent, 
		pmsg->LinkId.nSerial);

	if (m_pClient)
	{
		GameSceneObj* pObj = GetSceneObj(ident);

		if (pObj)
		{
			pObj->SetLinkIdent(link_ident);
			pObj->SetLinkPos(pmsg->LinkPos.X, pmsg->LinkPos.Y, pmsg->LinkPos.Z, 
				pmsg->LinkPos.Orient);
		}
		else
		{
			CORE_TRACE("(GameReceiver::ServerLinkTo)no object");
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnLinkTo(ident, link_ident, pmsg->LinkPos.X,
			pmsg->LinkPos.Y, pmsg->LinkPos.Z, pmsg->LinkPos.Orient);
	}
	else
	{
		event_send(GetID(), "on_link_to", 
			CVarList() << ident << link_ident << pmsg->LinkPos.X
			<< pmsg->LinkPos.Y << pmsg->LinkPos.Z << pmsg->LinkPos.Orient);
	}

	return true;
}

bool GameReceiver::ServerLinkMove(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_link_move_t))
	{
		CORE_TRACE("(GameReceiver::ServerLinkMove)size error");
		return false;
	}

	server_msg_link_move_t* pmsg = (server_msg_link_move_t*)pdata;
	char ident[32];
	char link_ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);
	SafeSprintf(link_ident, sizeof(link_ident), "%u-%u", pmsg->LinkId.nIdent, 
		pmsg->LinkId.nSerial);

	if (m_pClient)
	{
		GameSceneObj* pObj = GetSceneObj(ident);

		if (pObj)
		{
			pObj->SetLinkIdent(link_ident);
			pObj->SetLinkPos(pmsg->LinkPos.X, pmsg->LinkPos.Y, pmsg->LinkPos.Z, 
				pmsg->LinkPos.Orient);
		}
		else
		{
			CORE_TRACE("(GameReceiver::ServerLinkMove)no object");
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnLinkMove(ident, link_ident, pmsg->LinkPos.X,
			pmsg->LinkPos.Y, pmsg->LinkPos.Z, pmsg->LinkPos.Orient);
	}
	else
	{
		event_send(GetID(), "on_link_move", 
			CVarList() << ident << link_ident << pmsg->LinkPos.X
			<< pmsg->LinkPos.Y << pmsg->LinkPos.Z << pmsg->LinkPos.Orient);
	}

	return true;
}

bool GameReceiver::ServerUnlink(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_unlink_t))
	{
		CORE_TRACE("(GameReceiver::ServerUnlink)size error");
		return false;
	}

	server_msg_unlink_t* pmsg = (server_msg_unlink_t*)pdata;
	char ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);

	if (m_pClient)
	{
		GameSceneObj* pObj = GetSceneObj(ident);

		if (pObj)
		{
			pObj->SetLinkIdent("");
		}
		else
		{
			CORE_TRACE("(GameReceiver::ServerUnlink)no object");
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnUnlink(ident);
	}
	else
	{
		event_send(GetID(), "on_unlink", CVarList() << ident);
	}

	return true;
}

bool GameReceiver::ServerAllProp(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_all_prop_t))
	{
		CORE_TRACE("(GameReceiver::ServerAllProp)size error");
		return false;
	}

	server_msg_all_prop_t* pmsg = (server_msg_all_prop_t*)pdata;

	if (m_pClient)
	{
		GameScene* pScene = m_pClient->GetCurrentScene();

		if (NULL == pScene)
		{
			CORE_TRACE("(GameReceiver::ServerAllProp)no scene");
			return false;
		}

		CLoadArchive arLoad((char*)pdata + sizeof(server_msg_all_prop_t),
			len - sizeof(server_msg_all_prop_t));

		for (size_t i = 0; i < pmsg->nCount; ++i)
		{
			int obj_ident = arLoad.ReadInt();
			int obj_serial = arLoad.ReadInt();
			int prop_num = arLoad.ReadWord();
			char ident[32];

			SafeSprintf(ident, sizeof(ident), "%u-%u", obj_ident, obj_serial);

			GameSceneObj* pObj = pScene->GetSceneObjByIdent(ident);

			if (pObj)
			{
				if (!RecvProperty(pObj, arLoad, prop_num, true))
				{
					CORE_TRACE("(GameReceiver::ServerAllProp)property error");
				}

				if (m_pMsgHandler)
				{
					m_pMsgHandler->OnObjectProperty(ident, prop_num);
				}
				else
				{
					event_send(GetID(), "on_object_property", 
						CVarList() << ident << prop_num);
				}
			}
			else
			{
				CORE_TRACE("(GameReceiver::ServerAllProp)no object");
			}
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnAllProperty(pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_all_prop", 
			CVarList() << pmsg->nCount);
	}

	return true;
}

bool GameReceiver::ServerAddMoreObject(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_add_more_object_t))
	{
		CORE_TRACE("(GameReceiver::ServerAddMoreObject)size error");
		return false;
	}

	server_msg_add_more_object_t* pmsg = (server_msg_add_more_object_t*)pdata;

	if (m_pClient)
	{
		GameScene* pScene = m_pClient->GetCurrentScene();

		if (NULL == pScene)
		{			
			CORE_TRACE("(GameReceiver::ServerAddMoreObject)no scene");
			return false;
		}

		char* p = (char*)pdata + sizeof(server_msg_add_more_object_t);
		char* end = (char*)pdata + len;

		for (size_t i = 0; i < pmsg->nCount; ++i)
		{
			outer_object_t* object_id = (outer_object_t*)p;

			p += sizeof(outer_object_t);

			if (p > end)
			{
				CORE_TRACE("(GameReceiver::ServerAddMoreObject)overflow 1");
				return false;
			}

			outer_posi_t* posi = (outer_posi_t*)p;

			p += sizeof(outer_posi_t);

			if (p > end)
			{
				CORE_TRACE("(GameReceiver::ServerAddMoreObject)overflow 2");
				return false;
			}

			outer_dest_t* dest = (outer_dest_t*)p;

			p += sizeof(outer_dest_t);

			if (p > end)
			{
				CORE_TRACE("(GameReceiver::ServerAddMoreObject)overflow 3");
				return false;
			}

			unsigned short prop_count = *(unsigned short*)p;

			p += sizeof(unsigned short);

			if (p > end)
			{
				CORE_TRACE("(GameReceiver::ServerAddMoreObject)overflow 4");
				return false;
			}

			char ident[32];

			SafeSprintf(ident, sizeof(ident), "%u-%u", object_id->nIdent, 
				object_id->nSerial);

			GameSceneObj* pObj = pScene->AddSceneObj(ident);

			// 属性
			if (prop_count > 0)
			{
				CLoadArchive arLoad(p, end - p);

				if (!RecvProperty(pObj, arLoad, prop_count, false))
				{
					CORE_TRACE("(GameReceiver::ServerAddMoreObject)property error");
				}

				p += arLoad.GetLength();
			}

			// 位置
			pObj->SetLocation(posi->X, posi->Y, posi->Z, posi->Orient);
			pObj->SetDestination(dest->X, dest->Y, dest->Z, dest->Orient, 
				dest->MoveSpeed, dest->RotateSpeed, dest->JumpSpeed, 
				dest->Gravity);
			pObj->SetMode(dest->Mode);
			
			if (m_pMsgHandler)
			{
				m_pMsgHandler->OnAddObject(ident, prop_count);
			}
			else
			{
				event_send(GetID(), "on_add_object", 
					CVarList() << ident << prop_count);
			}
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnAddMoreObject(pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_add_more_object", 
			CVarList() << pmsg->nCount);
	}

	return true;
}

bool GameReceiver::ServerRemoveMoreObject(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_remove_more_object_t))
	{
		CORE_TRACE("(GameReceiver::ServerRemoveMoreObject)size error");
		return false;
	}

	server_msg_remove_more_object_t* pmsg = 
		(server_msg_remove_more_object_t*)pdata;

	size_t need_size = sizeof(server_msg_remove_more_object_t) 
		+ sizeof(outer_object_t) * pmsg->nCount;

	if ((size_t)len < need_size)
	{
		CORE_TRACE("(GameReceiver::ServerRemoveMoreObject)size error 1");
		return false;
	}

	if (m_pClient)
	{
		GameScene* pScene = m_pClient->GetCurrentScene();

		if (NULL == pScene)
		{			
			CORE_TRACE("(GameReceiver::ServerRemoveMoreObject)no scene");
			return false;
		}
		
		outer_object_t* object_ids = (outer_object_t*)
			((char*)pdata + sizeof(server_msg_remove_more_object_t));

		for (size_t i = 0; i < pmsg->nCount; ++i)
		{
			char ident[32];

			SafeSprintf(ident, sizeof(ident), "%u-%u", object_ids[i].nIdent, 
				object_ids[i].nSerial);

			GameSceneObj* pObj = pScene->GetSceneObjByIdent(ident);

			if (NULL == pObj)
			{
				continue;
			}
			
			if (m_pMsgHandler)
			{
				m_pMsgHandler->OnBeforeRemoveObject(ident);
			}
			else
			{
				event_send(GetID(), "on_before_remove_object", 
					CVarList() << ident);
			}
			
			pScene->RemoveSceneObj(ident);
			
			if (m_pMsgHandler)
			{
				m_pMsgHandler->OnRemoveObject(ident);
			}
			else
			{
				event_send(GetID(), "on_remove_object", 
					CVarList() << ident);
			}
		}
	}

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnRemoveMoreObject(pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_remove_more_object", 
			CVarList() << pmsg->nCount);
	}

	return true;
}


bool GameReceiver::ServerChargeValidstring(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_charge_validstring))
	{
		CORE_TRACE("(GameReceiver::ServerChargeValidstring)size error");
		return false;
	}

	server_msg_charge_validstring* pmsg = 
		(server_msg_charge_validstring*)pdata;

    if (pmsg)
    {
		if (m_pMsgHandler)
		{
            m_pMsgHandler->OnServerChargeValidstring(pmsg->strValidateString);
		}
		else
		{
			event_send(GetID(), "on_charge_validstring", 
				CVarList() << pmsg->strValidateString);
		}
	}

	return true;
}


bool GameReceiver::ServerMeasureDelay(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_measure_delay))
	{
		CORE_TRACE("(GameReceiver::ServerMeasureDelay)size error");
		return false;
	}

	server_msg_measure_delay* pmsg = 
		(server_msg_measure_delay*)pdata;

	if (pmsg)
	{
		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnServerMeasureDelay(pmsg->nIndex, pmsg->nServerTime);
		}
		else
		{
			event_send(GetID(), "on_server_measure_delay", 
				CVarList() << pmsg->nIndex << pmsg->nServerTime);
		}
	}

	return true;
}

bool GameReceiver::ServerFrameUpdate(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_frame_t))
	{
		CORE_TRACE("(GameReceiver::ServerFrameUpdate)size error");
		return false;
	}

	server_msg_frame_t* pmsg = (server_msg_frame_t*)pdata;

	if (pmsg)
	{
		if (m_pMsgHandler)
		{
			GameScene* pScene = m_pClient->GetCurrentScene();
			if (pScene)
			{
				size_t nCount = pScene->GetSceneObjCount();
				for (size_t i = 0; i < nCount; ++i)
				{
					PERSISTID scene_obj = pScene->GetSceneObjByIndex(i);

					if (!scene_obj.IsNull())
					{
						char ident[32];

						SafeSprintf(ident, sizeof(ident), "%u-%u", scene_obj.nIdent, 
							scene_obj.nSerial);

						m_pMsgHandler->OnServerFrameUpdate(ident, pmsg->frame_id_);
					}
				}
			}
		}
	}

	return true;
}

bool GameReceiver::ServerFrameMoving(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_dest_frame_t))
	{
		CORE_TRACE("(GameReceiver::ServerFrameMoving)size error");
		return false;
	}

	server_msg_dest_frame_t* pmsg = (server_msg_dest_frame_t*)pdata;

	char ident[32];
	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->info_.object_id_.nIdent, 
		pmsg->info_.object_id_.nSerial);

	if (m_pClient)
	{
		GameSceneObj* pObj = GetSceneObj(ident);
		if (NULL == pObj)
		{
			// 可能是因为视野消息优化发送的信息，不需要报错 
			return false;
		}

		pObj->SetLocation(pmsg->info_.src_pos_x_, 
			pmsg->info_.src_pos_y_, 
			pmsg->info_.src_pos_z_, 
			pmsg->info_.orient_);

		//强制移动到起点
		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnLocation(ident);
		}
		else
		{
			event_send(GetID(), "on_location", CVarList() << ident);
		}


		pObj->SetDestination(pmsg->info_.dest_.X, pmsg->info_.dest_.Y, pmsg->info_.dest_.Z, 
			pmsg->info_.dest_.Orient, pmsg->info_.dest_.MoveSpeed, pmsg->info_.dest_.RotateSpeed, 
			pmsg->info_.dest_.JumpSpeed, pmsg->info_.dest_.Gravity);

		pObj->SetMode(pmsg->info_.dest_.Mode);

		//准备向目的地移动
		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnMoving(ident);
		}
		else
		{
			event_send(GetID(), "on_moving", CVarList() << ident);
		}
	}

	return true;
}

bool GameReceiver::ServerFrameMovingDiff(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_dest_frame_diff_t))
	{
		CORE_TRACE("(GameReceiver::ServerFrameMovingDiff)size error");
		return false;
	}

	server_msg_dest_frame_diff_t* pmsg = 
		(server_msg_dest_frame_diff_t*)pdata;

	size_t need_size = sizeof(server_msg_dest_frame_diff_t) 
		+ sizeof(outer_frame_move_diff_t) * (pmsg->count_ - 1);

	if ((size_t)len < need_size)
	{
		CORE_TRACE("(GameReceiver::ServerFrameMovingDiff)size error 1");
		return false;
	}

	if (pmsg)
	{
		for (unsigned short i = 0; i < pmsg->count_; i++)
		{
			const outer_frame_move_diff_t& move_diff = pmsg->info_[i];

			char ident[32];
			SafeSprintf(ident, sizeof(ident), "%u-%u", move_diff.object_id_.nIdent, 
				move_diff.object_id_.nSerial);

			GameSceneObj* pObj = GetSceneObj(ident);
			if (NULL == pObj)
			{
				continue;
			}

			float fDiffX =  (float)(move_diff.object_move_diff_ >> 12 & 0x7);
			float fDiffY =  (float)(move_diff.object_move_diff_ >> 8 & 0x7);
			float fDiffZ =  (float)(move_diff.object_move_diff_ >> 4 & 0x7);
			float fDiffOrient = (float)(move_diff.object_move_diff_ & 0x7);

			if (move_diff.object_move_diff_ >> 12 & 0x8) fDiffX = -fDiffX;
			if (move_diff.object_move_diff_ >> 8 & 0x8) fDiffY = -fDiffY;
			if (move_diff.object_move_diff_ >> 4 & 0x8) fDiffZ = -fDiffZ;
			if (move_diff.object_move_diff_ & 0x8) fDiffOrient = -fDiffOrient;

			if (m_pMsgHandler)
			{
				m_pMsgHandler->OnServerFrameMovingDiff(ident, fDiffX, fDiffY, fDiffZ, fDiffOrient);
			}
			else
			{
				event_send(GetID(), "on_server_frame_movingdiff", 
					CVarList() << ident << fDiffX << fDiffY << fDiffZ << fDiffOrient);
			}
		}
	}

	return true;
}

bool GameReceiver::ServerMsgToStub(const void* pdata, size_t len)
{

	if (len < sizeof(server_msg_to_stub))
	{
		CORE_TRACE("(GameReceiver::ServerMsgToStub)size error");
		return false;
	}

	server_msg_to_stub* pmsg = (server_msg_to_stub*)pdata;
	if(m_pSoloStub)
		m_pSoloStub->PutMessage(pmsg->nMsgType,pdata,len,REMOTE_MSG);
	else 
		return false;

	return true;
}


bool GameReceiver::StubMsgToClient(const void* pdata, size_t len)
{
	if (len < sizeof(stub_msg_to_client))
	{
		CORE_TRACE("(GameReceiver::StubMsgToClient)size error");
		return false;
	}

	stub_msg_to_client* pmsg = (stub_msg_to_client*)pdata;

	//去掉头部后再处理
	if(m_pSoloStub)
		m_pSoloStub->SendToClient(&pmsg->nSubMsgType,len - sizeof(server_msg_base_t));
	else 
		return false;


	return true;

}