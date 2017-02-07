//--------------------------------------------------------------------
// 文件名:		game_rec_common.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月19日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "game_receiver.h"
#include "custom_handler.h"
#include "../public/outer_msg.h"
#include "../visual/i_game_client.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "../visual/i_event_manager.h"
#include "../visual/i_solo_stub.h"

// GameReceiver
// 一般消息接收

bool GameReceiver::ServerSetVerify(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_set_verify_t))
	{
		CORE_TRACE("(GameReceiver::ServerSetVerify)size error");
		return false;
	}

	server_msg_set_verify_t* pmsg = (server_msg_set_verify_t*)pdata;

	if (1 == pmsg->nHeight)
	{
		// 直接包含位置字符串
		char code[17];

		if (len < (sizeof(server_msg_set_verify_t) - sizeof(int) + 16))
		{
			CORE_TRACE("(GameReceiver::ServerSetVerify)size error");
			return false;
		}

		memcpy(code, pmsg->pData, 16);
		code[16] = 0;

		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnSetVerify(pmsg->nWidth, pmsg->nHeight, code, 
				NULL);
		}
		else
		{
			event_send(GetID(), "on_set_verify", 
				CVarList() << pmsg->nWidth << pmsg->nHeight << code);
		}
	}
	else
	{
		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnSetVerify(pmsg->nWidth, pmsg->nHeight, "", 
				pmsg->pData);
		}
		else
		{
			event_send(GetID(), "on_set_verify", 
				CVarList() << pmsg->nWidth << pmsg->nHeight);
		}
	}
	
	return true;
}

bool GameReceiver::ServerSetEncode(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_set_encode_t))
	{
		CORE_TRACE("(GameReceiver::ServerSetEncode)size error");
		return false;
	}

	server_msg_set_encode_t* pmsg = (server_msg_set_encode_t*)pdata;

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnSetEncode();
	}
	else
	{
		event_send(GetID(), "on_set_encode", CVarList());
	}
	
	return true;
}

bool GameReceiver::ServerErrorCode(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_error_code_t))
	{
		CORE_TRACE("(GameReceiver::ServerError)size error");
		return false;
	}

	server_msg_error_code_t* pmsg = (server_msg_error_code_t*)pdata;

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnErrorCode(pmsg->nResult);
	}
	else
	{
		event_send(GetID(), "on_error_code", 
			CVarList() << pmsg->nResult);
	}
	
	return true;
}

bool GameReceiver::ServerLoginSucceed(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_login_succeed_t))
	{
		CORE_TRACE("(GameReceiver::ServerLoginSucceed)size error");
		return false;
	}

	server_msg_login_succeed_t* pmsg = (server_msg_login_succeed_t*)pdata;

	ClearRoles();

	if (pmsg->nRoles > 0)
	{
		CLoadArchive arLoad((char*)pdata + sizeof(server_msg_login_succeed_t),
			len - sizeof(server_msg_login_succeed_t));

		try
		{
			for (int i = 0; i < pmsg->nRoles; ++i)
			{
				unsigned short role_args = arLoad.ReadWord();
				role_data_t* data = CORE_NEW(role_data_t);

				for (size_t k = 0; k < role_args; ++k)
				{
					unsigned char type = arLoad.ReadByte();

					switch (type)
					{
					case VTYPE_INT:
						data->Parameters.AddInt(arLoad.ReadInt());
						break;
					case VTYPE_INT64:
						data->Parameters.AddInt64(arLoad.ReadInt64());
						break;
					case VTYPE_FLOAT:
						data->Parameters.AddFloat(arLoad.ReadFloat());
						break;
					case VTYPE_DOUBLE:
						data->Parameters.AddDouble(arLoad.ReadDouble());
						break;
					case VTYPE_STRING:
						data->Parameters.AddString(arLoad.ReadString());
						break;
					case VTYPE_WIDESTR:
						data->Parameters.AddWideStr(arLoad.ReadWideStr());
						break;
					case VTYPE_OBJECT:
						data->Parameters.AddObject(arLoad.ReadObject());
						break;
					default:
						CORE_TRACE("(GameReceiver::ServerLoginSucceed)"
							"role parameter type error");
						CORE_DELETE(data);
						return false;
					}
				}

				if (role_args >= 4)
				{
					if (data->Parameters.GetType(0) == VTYPE_INT)
					{
						data->nRoleIndex = data->Parameters.IntVal(0);
					}
					
					if (data->Parameters.GetType(1) == VTYPE_INT)
					{
						data->nSysFlags = data->Parameters.IntVal(1);
					}

					if (data->Parameters.GetType(2) == VTYPE_WIDESTR)
					{
						data->wsName = data->Parameters.WideStrVal(2);
					}

					if (data->Parameters.GetType(3) == VTYPE_WIDESTR)
					{
						data->wsPara = data->Parameters.WideStrVal(3);
					}
				}

				if (role_args >= 6)
				{
					if (data->Parameters.GetType(4) == VTYPE_INT)
					{
						data->nDeleted = data->Parameters.IntVal(4);
					}
					
					if (data->Parameters.GetType(5) == VTYPE_DOUBLE)
					{
						data->dDeleteTime = data->Parameters.DoubleVal(5);
					}
				}
				else
				{
					data->nDeleted = 0;
					data->dDeleteTime = 0.0;
				}

				m_Roles.push_back(data);
			}
		}
		catch (CExceptMsg& e)
		{
			CORE_TRACE("(GameReceiver::ServerLoginSucceed)get message error");
			CORE_TRACE(e.GetMsg());
			return false;
		}
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnLoginSucceed(pmsg->nIsFree, pmsg->nPoints, 
			pmsg->nYear, pmsg->nMonth, pmsg->nDay, pmsg->nHour, pmsg->nMinute,
			pmsg->nSecond, pmsg->nRoles);
	}
	else
	{
		event_send(GetID(), "on_login_succeed", 
			CVarList() << pmsg->nIsFree << pmsg->nPoints << pmsg->nYear
			<< pmsg->nMonth << pmsg->nDay << pmsg->nHour << pmsg->nMinute
			<< pmsg->nSecond << pmsg->nRoles);
	}

	return true;
}

bool GameReceiver::ServerWorldInfo(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_world_info_t))
	{
		CORE_TRACE("(GameReceiver::ServerWorldInfo)size error");
		return false;
	}

	server_msg_world_info_t* pmsg = (server_msg_world_info_t*)pdata;

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnWorldInfo(pmsg->nInfoType, (wchar_t*)pmsg->wsInfo);
	}
	else
	{
		event_send(GetID(), "on_world_info", 
			CVarList() << pmsg->nInfoType << (const wchar_t*)pmsg->wsInfo);
	}

	return true;
}

bool GameReceiver::ServerTerminate(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_terminate_t))
	{
		CORE_TRACE("(GameReceiver::ServerTerminate)size error");
		return false;
	}

	server_msg_terminate_t* pmsg = (server_msg_terminate_t*)pdata;

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnTerminate(pmsg->nResult);
	}
	else
	{
		event_send(GetID(), "on_terminate", 
			CVarList() << pmsg->nResult);
	}
	
	return true;
}

bool GameReceiver::ServerPropertyTable(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_property_table_t))
	{
		CORE_TRACE("(GameReceiver::ServerPropertyTable)size error");
		return false;
	}

	server_msg_property_table_t* pmsg = (server_msg_property_table_t*)pdata;
	size_t count = pmsg->nCount;

	ClearPropertyTable();
	
	char* p = (char*)pdata + sizeof(server_msg_property_table_t);
	
	for (size_t i = 0; i < count; ++i)
	{
		prop_data_t* data = CORE_NEW(prop_data_t);
		const size_t SIZE1 = strlen(p) + 1;

		data->strName = p;
		p += SIZE1;
		data->nType = *(unsigned char*)p;
		p += sizeof(unsigned char);
		data->nCount = 0;

		m_PropertyTable.push_back(data);
	}	

	if ((p - (char*)pdata) != len)
	{
		CORE_TRACE("(GameReceiver::ServerPropertyTable)length error");
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnPropertyTable(pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_property_table", 
			CVarList() << pmsg->nCount);
	}
	
	return true;
}

bool GameReceiver::ServerRecordTable(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_record_table_t))
	{
		CORE_TRACE("(GameReceiver::ServerRecordTable)size error");
		return false;
	}
	
	server_msg_record_table_t* pmsg = (server_msg_record_table_t*)pdata;
	size_t count = pmsg->nCount;
	
	ClearRecordTable();
	
	char* p = (char*)pdata + sizeof(server_msg_record_table_t);

	for (size_t i = 0; i < count; ++i)
	{
		rec_data_t* data = CORE_NEW(rec_data_t);
		const size_t SIZE1 = strlen(p) + 1;

		data->strName = p;
		p += SIZE1;

		size_t cols = *(unsigned short*)p;

		p += sizeof(unsigned short);
		data->nCols = cols;
		
		for (size_t k = 0; k < cols; ++k)
		{
			int type = *(unsigned char*)p;

			p += sizeof(unsigned char);
			data->ColTypes.push_back(type);
		}

		m_RecordTable.push_back(data);
	}

	if ((p - (char*)pdata) != len)
	{
		CORE_TRACE("(GameReceiver::ServerRecordTable)length error");
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnRecordTable(pmsg->nCount);
	}
	else
	{
		event_send(GetID(), "on_record_table", 
			CVarList() << pmsg->nCount);
	}
	
	return true;
}

bool GameReceiver::ServerSpeech(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_speech_t))
	{
		CORE_TRACE("(GameReceiver::ServerSpeech)size error");
		return false;
	}

	server_msg_speech_t* pmsg = (server_msg_speech_t*)pdata;
	char ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnSpeech(ident, (wchar_t*)pmsg->wsContent);
	}
	else
	{
		event_send(GetID(), "on_speech", 
			CVarList() << ident << (wchar_t*)pmsg->wsContent);
	}
	
	return true;
}

bool GameReceiver::ServerSystemInfo(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_system_info_t))
	{
		CORE_TRACE("(GameReceiver::ServerSystemInfo)size error");
		return false;
	}

	server_msg_system_info_t* pmsg = (server_msg_system_info_t*)pdata;

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnSystemInfo(pmsg->nInfoType, (wchar_t*)pmsg->wsInfo);
	}
	else
	{
		event_send(GetID(), "on_system_info", 
			CVarList() << pmsg->nInfoType << (wchar_t*)pmsg->wsInfo);
	}
	
	return true;
}

bool GameReceiver::ServerMenu(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_menu_t))
	{
		CORE_TRACE("(GameReceiver::ServerMenu)size error");
		return false;
	}

	server_msg_menu_t* pmsg = (server_msg_menu_t*)pdata;
	char ident[32];

	SafeSprintf(ident, sizeof(ident), "%u-%u", pmsg->ObjectId.nIdent, 
		pmsg->ObjectId.nSerial);

	unsigned int count = pmsg->nCount;
	
	ClearMenus();
	
	CLoadArchive arLoad(pdata, len);
	
	try
	{
		arLoad.Seek(sizeof(server_msg_menu_t));
		
		for (unsigned int i = 0; i < count; ++i)
		{
			menu_data_t* data = CORE_NEW(menu_data_t);

			data->nType = arLoad.ReadByte();
			data->nMark = arLoad.ReadWord();
			data->wsInfo = arLoad.ReadWideStr();

			m_Menus.push_back(data);
		}
	}
	catch (CExceptMsg& e)
	{
		CORE_TRACE("(GameReceiver::ServerMenu)get message error");
		CORE_TRACE(e.GetMsg());
		return false;
	}
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnMenu(ident, count);
	}
	else
	{
		event_send(GetID(), "on_menu", CVarList() << ident << count);
	}
	
	return true;
}

bool GameReceiver::ServerClearMenu(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_clear_menu_t))
	{
		CORE_TRACE("(GameReceiver::ServerClearMenu)size error");
		return false;
	}

	server_msg_clear_menu_t* pmsg = (server_msg_clear_menu_t*)pdata;

	ClearMenus();

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnClearMenu();
	}
	else
	{
		event_send(GetID(), "on_clear_menu", CVarList());
	}
	
	return true;
}

bool GameReceiver::ServerCustom(int src,const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_custom_t))
	{
		CORE_TRACE("(GameReceiver::ServerCustom)size error");
		return false;
	}

	if(m_pSoloStub && m_pSoloStub->IsRunning() && src == 1)
	{
		CORE_TRACE("(GameReceiver::ServerCustom)Main player in local clone scene,not dispose");
		return false;
	}

	server_msg_custom_t* pmsg = (server_msg_custom_t*)pdata;

	unsigned int arg_num = pmsg->nArgNum;

	if (m_nMaxCustomArguments > 0)
	{
		if (int(arg_num) > m_nMaxCustomArguments)
		{
			CORE_TRACE("(GameReceiver::ServerCustom)argument exceed limit");
			arg_num = m_nMaxCustomArguments;
		}
	}

	CVarList args;
	
	args << arg_num;
	
	CLoadArchive arLoad((char*)pdata + sizeof(server_msg_custom_t), 
		len - sizeof(server_msg_custom_t));

	try
	{
		for (size_t i = 0; i < arg_num; ++i)
		{
			unsigned char type = arLoad.ReadByte();

			switch (type)
			{
			case VTYPE_INT:
				args << arLoad.ReadInt();
				break;
			case VTYPE_INT64:
				args << arLoad.ReadInt64();
				break;
			case VTYPE_FLOAT:
				args << arLoad.ReadFloat();
				break;
			case VTYPE_DOUBLE:
				args << arLoad.ReadDouble();
				break;
			case VTYPE_STRING:
				args << arLoad.ReadString();
				break;
			case VTYPE_WIDESTR:
				args << arLoad.ReadWideStr();
				break;
			case VTYPE_OBJECT:
				args << arLoad.ReadObject();
				break;
			default:
				throw CExceptMsg("error arg type");
				break;
			}
		}
	}
	catch (CExceptMsg& e)
	{
		CORE_TRACE("(GameReceiver::ServerCustom)get message error");
		CORE_TRACE(e.GetMsg());
		return false;
	}

	if (m_pCustomHandler)
	{
		m_pCustomHandler->Process(args);
	}
	else
	{
		if (m_pMsgHandler)
		{
			m_pMsgHandler->OnCustom(args);
		}
		else
		{
			event_send(GetID(), "on_custom", args);
		}
	}
	
	return true;
}

bool GameReceiver::ServerIdle(const void* pdata, size_t len)
{
	if (len != sizeof(server_msg_idle_t))
	{
		CORE_TRACE("(GameReceiver::ServerIdle)size error");
		return false;
	}

	server_msg_idle_t* pmsg = (server_msg_idle_t*)pdata;

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnIdle();
	}
	else
	{
		event_send(GetID(), "on_idle", CVarList());
	}
	
	return true;
}

bool GameReceiver::ServerWarning(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_warning_t))
	{
		CORE_TRACE("(GameReceiver::ServerWarning)size error");
		return false;
	}

	server_msg_warning_t* pmsg = (server_msg_warning_t*)pdata;

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnWarning(pmsg->nType, (wchar_t*)pmsg->wsInfo);
	}
	else
	{
		event_send(GetID(), "on_warning", 
			CVarList() << pmsg->nType << (wchar_t*)pmsg->wsInfo);
	}
	
	return true;
}

bool GameReceiver::ServerFromGmcc(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_from_gmcc_t))
	{
		CORE_TRACE("(GameReceiver::ServerFromGmcc)size error");
		return false;
	}

	server_msg_from_gmcc_t* pmsg = (server_msg_from_gmcc_t*)pdata;

	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnFromGmcc(pmsg->nId, (wchar_t*)pmsg->wsName, 
			(wchar_t*)pmsg->wsInfo);
	}
	else
	{
		event_send(GetID(), "on_from_gmcc", 
			CVarList() << pmsg->nId << (wchar_t*)pmsg->wsName 
			<< (wchar_t*)pmsg->wsInfo);
	}
	
	return true;
}

bool GameReceiver::ServerQueue(const void* pdata, size_t len)
{
	if (len < sizeof(server_msg_queue_t))
	{
		CORE_TRACE("(GameReceiver::ServerQueue)size error");
		return false;
	}
	
	server_msg_queue_t* pmsg = (server_msg_queue_t*)pdata;
	
	if (m_pMsgHandler)
	{
		m_pMsgHandler->OnQueue(pmsg->nQueue, pmsg->nPosition, 
			pmsg->nPriorCount);
	}
	else
	{
		event_send(GetID(), "on_queue", 
			CVarList() << pmsg->nQueue << pmsg->nPosition << pmsg->nPriorCount);
	}
	
	return true;
}

