//--------------------------------------------------------------------
// �ļ���:		OuterMsg.h
// ��  ��:		ͨ�÷��������棬�ͻ��˺ͷ�����ͨѶ����Ϣ����
// ˵  ��:		
// ��������:	2008��9��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SERVER_OUTERMSG_H
#define _SERVER_OUTERMSG_H

#include "len_type.h"

// ���鳤����Ϊ1,��ʾ�ɱ䳤

#pragma pack(push, 1)

// ͨѶЭ��İ汾��
const int OUTER_PROTOCOL_VERSION = 0x300;
// ������ֳ���
const int OUTER_OBJNAME_LENGTH = 32;
// һ������֤������
const size_t VALIDATE_LEN_LENGTH = 1024;

// ������������
enum OUTER_DATA_TYPE_ENUM
{
	OUTER_TYPE_UNKNOWN,	// δ֪
	OUTER_TYPE_BYTE,	// һ�ֽ�
	OUTER_TYPE_WORD,	// ���ֽ�
	OUTER_TYPE_DWORD,	// ���ֽ�
	OUTER_TYPE_QWORD,	// ���ֽ�
	OUTER_TYPE_FLOAT,	// �������ֽ�
	OUTER_TYPE_DOUBLE,	// ������ֽ�
	OUTER_TYPE_STRING,	// �ַ�����ǰ�ĸ��ֽ�Ϊ����
	OUTER_TYPE_WIDESTR,	// UNICODE���ַ�����ǰ�ĸ��ֽ�Ϊ����
	OUTER_TYPE_OBJECT,	// �����
	OUTER_TYPE_MAX,
};

// �����ʶ��
struct outer_object_t
{
	union
	{
		struct
		{
			uint64_t nData64;
		};

		struct
		{
			int nIdent;
			int nSerial;
		};
	};
};

// λ������
struct outer_posi_t
{
	len_t X;
	len_t Y;
	len_t Z;
	len_t Orient;
};

// �ƶ�Ŀ������
struct outer_dest_t
{
	len_t X;
	len_t Y;
	len_t Z;
	len_t Orient;
	len_t MoveSpeed;
	len_t RotateSpeed;
	len_t JumpSpeed;
	len_t Gravity;
	int Mode;
};

// ������ƶ�Ŀ��
struct outer_object_dest_t
{
	outer_object_t ObjectId;	// �����ʶ
	outer_dest_t dest;			// �ƶ�Ŀ��
};

// ������ƶ�Ŀ��
struct outer_object_dest_ex_t
{
	outer_object_t ObjectId;	// �����ʶ
	outer_dest_t dest;			// �ƶ�Ŀ��
	unsigned int nDelayTime;	// Ŀ���ӳ�
};

// ������ƶ�Ŀ��
struct outer_object_dest_frame_t
{
	outer_object_t	object_id_;	// �����ʶ
	len_t			src_pos_x_;	// ԭpos_x
	len_t			src_pos_y_;	// ԭpos_y
	len_t			src_pos_z_;	// ԭpos_z
	len_t			orient_;	// ԭorient
	len_t			accelerate_speed_;	// ���Լ��ٶ�
	outer_dest_t	dest_;	// �ƶ�Ŀ��
};
// client --> server messages
// �ͻ��˷�������������Ϣ

enum CLIENT_MESSAGE_ENUM
{
	CLIENT_GET_VERIFY,
	CLIENT_RET_ENCODE,
	CLIENT_LOGIN,
	CLIENT_WORLD_INFO,
	CLIENT_CHOOSE_ROLE,
	CLIENT_CREATE_ROLE,
	CLIENT_DELETE_ROLE,
	CLIENT_SELECT,
	CLIENT_SPEECH,
	CLIENT_READY,
	CLIENT_CUSTOM,
	CLIENT_REQUEST_MOVE,
	CLIENT_REVIVE_ROLE,
	CLIENT_MEASURE_DELAY_ACK,	// �ͻ����ӳٲ�������

	// ��ȫ��Ϣ����
	CLIENT_SECURITY_LOGIN	= 20,
	CLIENT_REQ_OUTLINE_STAT = 21,
	CLIENT_REQ_DETAIL_STAT  = 22,
	CLIENT_REQ_REMOTE_CTRL  = 23,

	CLIENT_STUB_MSG_TO_SERVER,	// ���ط���Ϣ��member��

	CLIENT_MESSAGE_MAX,
};

// �ͻ�����Ϣ����
struct client_msg_base_t
{
	unsigned char nMsgType;		// ��Ϣ���
};
struct client_stub_msg_base_t: public client_msg_base_t
{
	unsigned char nSubMsgType;	// ��Ϣ���

};

struct client_stub_msg_to_server: public client_stub_msg_base_t
{

};


// ������֤��
struct client_msg_get_verify_t: public client_msg_base_t
{
};

// ���ܸı�ȷ��
struct client_msg_return_encode_t: public client_msg_base_t
{
	char strInfo[32];
	int nAddress;
	unsigned char nVerify[16];
};

// ��¼����
enum LOGIN_TYPE_ENUM
{
	LOGIN_TYPE_NORMAL		= 1,  // Ĭ�ϵ�¼����(�˺š�����ģʽ)
	LOGIN_TYPE_VAILDSRING	= 2,  // ����¼
	LOGIN_TYPE_SHIELD		= 4,  // ţ�ܵ�¼
	LOGIN_TYPE_MOBILESHIELD = 5,  // �ֻ�ţ�ܵ�¼
	LOGIN_TYPE_RECONNECT	= 100,// ��������¼
	LOGIN_TYPE_MAX,
};

// ��¼
struct client_msg_login_t: public client_msg_base_t
{
	int nVersion;			// �汾��
	char strVerify[16];		// ��֤��
	//string account		// �ʺ�
	//string password		// ����
	//string loginstring	// ��¼��
	//int	login_type		// ��¼����
};

// �����Ϸ������Ϣ���������ﴴ��������Ŀ�ģ�
struct client_msg_world_info_t: public client_msg_base_t
{
	int nInfoType;		// ��Ϣ����
	//unsigned short wsInfo[1];	// ������Ϣ
};

// ѡ���ɫ
struct client_msg_choose_role_t: public client_msg_base_t
{
	unsigned short wsName[OUTER_OBJNAME_LENGTH + 1];	// ����
	unsigned char nVerify[16];							// У����
	char strInfo[1];									// ������Ϣ
};

// ������ɫ
struct client_msg_create_role_t: public client_msg_base_t
{
	int nIndex;
	unsigned short wsName[OUTER_OBJNAME_LENGTH + 1];	// ����
	unsigned short nArgNum;								// ��������
	//unsigned char	arg_type;							// ��������
	// ��������
};

// ɾ����ɫ
struct client_msg_delete_role_t: public client_msg_base_t
{
	unsigned short wsName[OUTER_OBJNAME_LENGTH + 1];	// ����
};

// �ָ���ɫ
struct client_msg_revive_role_t: public client_msg_base_t
{
	unsigned short wsName[OUTER_OBJNAME_LENGTH + 1];	// ����
};

// ������ѡ�����
struct client_msg_select_t: public client_msg_base_t
{
	unsigned char nVerify[16];	// У����
	int nSerial;				// ��Ϣ���к�	
	outer_object_t ObjectId;	// �����ʶ
	int nFunctionId;			// ���ܺ�Ϊ0��ʾִ��ȱʡ����
};

// ��ͨ���� 
struct client_msg_speech_t: public client_msg_base_t
{
	unsigned short wsContent[1];	// ��������
};

// �ͻ���ͨ�ö�����Ϣ
struct client_msg_custom_t: public client_msg_base_t
{
	unsigned char nVerify[16];	// У����
	int nSerial;				// ��Ϣ���к�	
	unsigned short nArgNum;		// ��������
	//unsigned char	arg_type;	// ��������
	// ��������
};

// �ͻ��˾���
struct client_msg_ready_t: public client_msg_base_t
{
};

// �ƶ�����
struct client_msg_request_move_t: public client_msg_base_t
{
	unsigned char nMode;	// �ƶ�ģʽ
	unsigned short nArgNum;	// �����������
	//len_t args[1]			// �����������
	//char info[1]			// ��Ϣ��ĩβ���ܰ�����0�����ĸ����ַ�����Ϣ
};

// ��ȫ��¼
struct client_msg_security_login_t: public client_msg_base_t
{
	char strVerify[128];	// ��֤��
};

// ����Զ�̿��Ʒ�����
struct client_msg_request_ctrl_t: public client_msg_base_t
{
	unsigned short nType;	// ��������
	char strInfo[1];		// ������Ϣ
};

struct client_msg_measure_delay_ack_t: public client_msg_base_t
{
	unsigned int nIndex;
	unsigned int nServerTime;
};

// server --> client messages
// �����������ͻ��˵���Ϣ

enum SERVER_MESSAGE_ENUM
{
	SERVER_SERVER_INFO,
	SERVER_SET_VERIFY,
	SERVER_SET_ENCODE,
	SERVER_ERROR_CODE,
	SERVER_LOGIN_SUCCEED,
	SERVER_WORLD_INFO,
	SERVER_IDLE,
	SERVER_QUEUE,
	SERVER_TERMINATE,
	SERVER_PROPERTY_TABLE,
	SERVER_RECORD_TABLE,
	SERVER_ENTRY_SCENE,
	SERVER_EXIT_SCENE,
	SERVER_ADD_OBJECT,
	SERVER_REMOVE_OBJECT,
	SERVER_SCENE_PROPERTY,
	SERVER_OBJECT_PROPERTY,
	SERVER_RECORD_ADDROW,
	SERVER_RECORD_DELROW,
	SERVER_RECORD_GRID,
	SERVER_RECORD_CLEAR,
	SERVER_CREATE_VIEW,
	SERVER_DELETE_VIEW,
	SERVER_VIEW_PROPERTY,
	SERVER_VIEW_ADD,
	SERVER_VIEW_REMOVE,
	SERVER_SPEECH,
	SERVER_SYSTEM_INFO,
	SERVER_MENU,
	SERVER_CLEAR_MENU,
	SERVER_CUSTOM,
	SERVER_LOCATION,
	SERVER_MOVING,
	SERVER_ALL_DEST,
	SERVER_WARNING,
	SERVER_FROM_GMCC,
	SERVER_LINK_TO,
	SERVER_UNLINK,
	SERVER_LINK_MOVE,
	SERVER_CP_CUSTOM,			// ѹ�����Զ�����Ϣ
	SERVER_CP_ADD_OBJECT,		// ѹ������ӿɼ�������Ϣ
	SERVER_CP_RECORD_ADDROW,	// ѹ���ı���������Ϣ
	SERVER_CP_VIEW_ADD,			// ѹ����������Ӷ�����Ϣ
	SERVER_VIEW_CHANGE,			// �����ı�
	SERVER_CP_ALL_DEST,			// ѹ���Ķ����ƶ���Ϣ
	SERVER_ALL_PROP,			// �����������Ըı���Ϣ
	SERVER_CP_ALL_PROP,			// ѹ���Ķ����������Ըı���Ϣ
	SERVER_ADD_MORE_OBJECT,		// ���Ӷ������
	SERVER_CP_ADD_MORE_OBJECT,	// ѹ�������Ӷ������
	SERVER_REMOVE_MORE_OBJECT,	// �Ƴ��������
	SERVER_CHARGE_VALIDSTRING,  // ���Ʒѷ��������ص���֤�������ͻ���

	// ��ȫ��Ϣ����
	SERVER_SECURITY_LOGIN	= 51,
	SERVER_REP_OUTLINE_STAT = 52,
	SERVER_REP_DETAIL_STAT	= 53,
	SERVER_REP_REMOTE_CTRL	= 54,
	SERVER_MOVING_EX,			// ����ʱ���ƶ���Ϣ
	SERVER_ALL_DEST_EX,			// ����ʱ���ƶ���Ϣ����
	SERVER_CP_ALL_DEST_EX,		// ѹ������ʱ�Ķ����ƶ���Ϣ
	SERVER_ENTRY_SCENE_EX,		// ֧��֡ͬ���Ľ��볡����Ϣ
	SERVER_MEASURE_DELAY,		// �������ӳٶ���
	SERVER_FRAME_UPDATE,        // �߼�֡����
	SERVER_FRAME_MOVING,        // �����߼�֡���ƶ� -- �ƶ�key
	SERVER_FRAME_MOVING_DIFF,   // �����߼�֡���ƶ� -- �ƶ���ֵ

	SERVER_MSG_TO_STUB,			// �������Ϣ���ͻ��˱��ط�
	STUB_MSG_TO_CLIENT,			// ���ط���Ϣ��client
	SERVER_MESSAGE_MAX,
};

// ��������Ϣ����
struct server_msg_base_t
{
	unsigned char nMsgType;		// ��Ϣ���
};
struct server_stub_msg_base_t: public server_msg_base_t
{
	unsigned char nSubMsgType;	// ����Ϣ���
	unsigned short nArgNum;		// ��������
	//unsigned char arg_type;	// ��������
	// ��������

};

// ���͵����ط�����Ϣ
struct server_msg_to_stub: public server_stub_msg_base_t
{

};

struct stub_msg_to_client: public server_stub_msg_base_t
{

};


// ��֤��
struct server_msg_set_verify_t: public server_msg_base_t
{
	int nWidth;		// ͼƬ���	
	int nHeight;	// ͼƬ�߶�
	int pData[1];	// ����
};

// �ı���ܷ���
struct server_msg_set_encode_t: public server_msg_base_t
{
	char strInfo[32];
	int nDynamicKey;
	int nEncodeId;
	int nAddress;
	unsigned char nCondition[16];
	unsigned char pData[1];
};

// ���ش�����Ϣ
struct server_msg_error_code_t: public server_msg_base_t
{
	int nResult;		// ������
};

// ��¼��Ϣ
struct server_msg_login_succeed_t: public server_msg_base_t
{
	int nIsFree;		// �Ƿ����
	int nPoints;		// ʣ�����
	int nYear;			// ���½�ֹ����
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;
	int nSecond;
	int nDynamicKey;	// ��̬�ܳ�
	int nRoles;			// ��ɫ��
	//unsigned short arg_num;	// ��һ����ɫ�Ĳ�������
	// ����ȡ�Զ�����Ϣ���Ƶķ���ȡ��ɫ��Ϣ
	//unsigned char type;
	//int sys_flags;			// ��ɫ��ϵͳ��־
	//unsigned char type;
	//wstring name;				// ��ɫ����
	//unsigned char type;
	//wstring para;				// ��ɫ����
	//unsigned char type;
	//int deleted;				// ��ɫ�Ƿ�ɾ��
	//unsigned char type;
	//double delete_time;		// ��ɫɾ��ʱ��
	//string cross_addr;		// ������������б��ַ(idc1:ip1:port1,idc2:ip2:port2),���ڿ��ʱΪ��
	// ...						// ����������������
	//unsigned short arg_num;	// �¸���ɫ�Ĳ�������
	// ...
};

// ��Ϸ������Ϣ
struct server_msg_world_info_t: public server_msg_base_t
{
	unsigned short nInfoType;	// ��Ϣ����
	unsigned short wsInfo[1];	// ��Ϣ����
};

// ����
struct server_msg_idle_t: public server_msg_base_t
{
};

// �Ŷӣ�������������������������ʱ��ҵ����ӽ����Ŷ�״̬��
struct server_msg_queue_t: public server_msg_base_t
{
	int nQueue;		// ���б��
	int nPosition;	// �ڶ����е�λ�ã�Ϊ0��ʾ�����Ŷӣ�
	int nPriorCount; // ��ɫͨ������
};

// ǿ����ֹ
struct server_msg_terminate_t: public server_msg_base_t
{
	int nResult;	// ԭ��
};

// ���Ա�
struct server_msg_property_table_t: public server_msg_base_t
{
	unsigned short nCount;	// ��������
	//string name;			// �������ƣ���0�������ַ�����ǰ�治��������
	//unsigned char type	// ��������
};

// ���б�����Ϣ
struct server_msg_record_table_t: public server_msg_base_t
{
	unsigned short nCount;		// �������
	//string name;				// ������ƣ���0�������ַ�����ǰ�治��������
	//unsigned short cols;		// ���������256
	//unsigned char col_type[];	// ������
	// ... ���������Ϣ
};

// ���볡��
struct server_msg_entry_scene_t: public server_msg_base_t
{
	int nDynamicKey;			// ��̬�ܳ�
	outer_object_t ObjectId;	// ������ҵĶ����ʶ
	unsigned short nCount;		// ����������������
	// ��������
	//unsigned short property_index
	//property_data
	// �������������ԣ��������Ա���н���
};

// ���볡��
struct server_msg_entry_scene_ex_t: public server_msg_base_t
{
	int nDynamicKey;			// ��̬�ܳ�
	int scene_motion_mode_;		// �ƶ���ʽ
	int frame_time_delta_;		// ÿ֡������߼�ʱ��
	len_t move_line_diff_ratio_;// �ƶ����α��ʡ�ÿ����λ��ֵ�����ʵ�ʾ��롣
	len_t move_rotate_diff_ratio_;// �ƶ��Ƕȱ��ʡ�ÿ����λ��ֵ�����ʵ�ʽǶȡ�

	outer_object_t ObjectId;	// ������ҵĶ����ʶ
	unsigned short nCount;		// ����������������
	// ��������
	//unsigned short property_index
	//property_data
	// �������������ԣ��������Ա���н���
};
// �˳�����
struct server_msg_exit_scene_t: public server_msg_base_t
{
};

// ������Ϸ����
struct server_msg_add_object_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// �����ʶ
	outer_posi_t posi;			// ����λ��
	outer_dest_t dest;			// �ƶ�Ŀ��
	unsigned short nCount;		// ������������
	// ��������
	//unsigned short property_index
	//property_data
	// �������������ԣ��������Ա���н���
};

// ɾ����Ϸ����
struct server_msg_remove_object_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// �����ʶ
};

// ��������
struct server_msg_scene_property_t: public server_msg_base_t
{
	unsigned short nCount;		// ������������
	// ��������
	//unsigned short property_index
	//property_data
	// �������������ԣ��������Ա���н���
};

// ��������
struct server_msg_object_property_t: public server_msg_base_t
{
	unsigned char nIsViewObj;	// �Ƿ������ж���
	outer_object_t ObjectId;	// �����ʶ
	unsigned short nCount;		// ������������
	// ��������
	//unsigned short property_index
	//property_data
	// �������������ԣ��������Ա���н���
};

// �����������
struct server_msg_record_add_row_t: public server_msg_base_t
{
	unsigned char nIsViewObj;	// �Ƿ������ж���
	outer_object_t ObjectId;	// �����ʶ
	unsigned short nIndex;		// �����
	unsigned short nRow;		// ������к�
	unsigned short nRows;		// ����
	// ����
};

// ������ɾ����
struct server_msg_record_remove_row_t: public server_msg_base_t
{
	unsigned char nIsViewObj;	// �Ƿ������ж���
	outer_object_t ObjectId;	// �����ʶ
	unsigned short nIndex;		// �����
	unsigned short nRow;		// ɾ�����к�
};

// �����ݸı�
struct server_msg_record_grid_t: public server_msg_base_t
{
	unsigned char nIsViewObj;	// �Ƿ������ж���
	outer_object_t ObjectId;	// �����ʶ
	unsigned short nIndex;		// �����
	unsigned short nCount;		// ��������
	//unsigned short row
	//unsigned char col		
	// ����
};

// ���������
struct server_msg_record_clear_t: public server_msg_base_t
{
	unsigned char nIsViewObj;	// �Ƿ������ж���
	outer_object_t ObjectId;	// �����ʶ
	unsigned short nIndex;		// �����
};

// ����Ӵ�
struct server_msg_create_view_t: public server_msg_base_t
{
	unsigned short nViewId;		// �Ӵ����, 0-����������
	unsigned short nCapacity;	// ����
	unsigned short nCount;		// ������������
	// ��������
	//unsigned short property_index
	//property_data
	// �������������ԣ��������Ա���н���
};

// ɾ���Ӵ�
struct server_msg_delete_view_t: public server_msg_base_t
{
	unsigned short nViewId;		// �Ӵ����
};

// �Ӵ�����
struct server_msg_view_property_t: public server_msg_base_t
{
	unsigned short nViewId;		// �Ӵ����
	unsigned short nCount;		// ������������
	// ��������
	//unsigned short property_index
	//property_data
	// �������������ԣ��������Ա���н���
};

// �Ӵ�����Ӷ���
struct server_msg_view_add_t: public server_msg_base_t
{
	unsigned short nViewId;		// �Ӵ����
	unsigned short nObjectId;	// �Ӵ������
	unsigned short nCount;		// ������������
	// ��������
	//unsigned short property_index
	//property_data
	// �������������ԣ��������Ա���н���
};

// �Ӵ���ɾ������
struct server_msg_view_remove_t: public server_msg_base_t
{
	unsigned short nViewId;		// �Ӵ����
	unsigned short nObjectId;	// �Ӵ������
};

// �Ӵ��ж���ı�λ��
struct server_msg_view_change_t: public server_msg_base_t
{
	unsigned short nViewId;			// �Ӵ����
	unsigned short nOldObjectId;	// ���Ӵ������
	unsigned short nNewObjectId;	// ���Ӵ������
};

// ��ͨ����
struct server_msg_speech_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// �����	
	unsigned short wsContent[1];// ��������
};

// ϵͳ��Ϣ
struct server_msg_system_info_t: public server_msg_base_t
{
	unsigned short nInfoType;	// ��Ϣ����
	unsigned short wsInfo[1];	// ��Ϣ����
};

// NPC�Ի�ѡ��
struct server_msg_menu_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// �����ʶ
	unsigned short nCount;		// �Ի���Ϣ����
	//unsigned char type;		// ���ͣ��磺0�����֣�1������2ѡ�
	//unsigned short mark;		// ���
	//unsigned short size;		// ���ֳ���
	//unsigned short info[1];	// ������Ϣ
};

// ����˵�
struct server_msg_clear_menu_t: public server_msg_base_t
{
};

// �ű�������Ϣ
struct server_msg_custom_t: public server_msg_base_t
{
	unsigned short nArgNum;		// ��������
	//unsigned char arg_type;	// ��������
	// ��������
};

// ����λ�ú�Ŀ��
struct server_msg_location_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// �����ʶ
	outer_posi_t posi;			// λ��
};

// �����˶�Ŀ��
struct server_msg_moving_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// �����ʶ
	outer_dest_t dest;			// �ƶ�Ŀ��
};

// �����˶�Ŀ��(����Ϣ��ʱ)
struct server_msg_moving_ex_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// �����ʶ
	outer_dest_t dest;			// �ƶ�Ŀ��
	unsigned int nDelayTime;	// Ŀ���ӳ�
};
// ��������Ŀ��ͬ��
struct server_msg_all_dest_t: public server_msg_base_t
{
	unsigned short nCount;			// ��������
	outer_object_dest_t	info[1];	// �����ƶ�Ŀ��
};
// ��������Ŀ��ͬ��(����Ϣ��ʱ)
struct server_msg_all_dest_ex_t: public server_msg_base_t
{
	unsigned short nCount;			// ��������
	outer_object_dest_ex_t	info[1];	// �����ƶ�Ŀ��
};


// �������߼�֡ -- �ͻ����յ��߼�֡��,���л���֡���߼�����
struct server_msg_frame_t: public server_msg_base_t
{
	unsigned int frame_id_;			// �߼�֡
};

// �����˶�Ŀ��--�����߼�֡
struct server_msg_dest_frame_t: public server_msg_base_t
{
	outer_object_dest_frame_t info_;	// �ƶ�Ŀ��
};

// �����ƶ���ֵ����
struct outer_frame_move_diff_t
{
	//unsigned char object_visual_index_; // �����������
	outer_object_t object_id_;				// ����ID����֤���к�������������
	unsigned short object_move_diff_;		// �����ƶ���ֵ���� x 4bit, y 4bit, z 4bit, orient 4bit.
};

// �����˶�Ŀ��--�����߼�֡
struct server_msg_dest_frame_diff_t: public server_msg_base_t
{
	unsigned short count_;				// ��������
	outer_frame_move_diff_t	info_[1];	// �����ƶ���ֵ����
};

// ϵͳ�澯
struct server_msg_warning_t: public server_msg_base_t
{
	unsigned short nType;		// �澯����
	unsigned short wsInfo[1];	// �澯����
};

// GM��Ӧ
struct server_msg_from_gmcc_t: public server_msg_base_t
{
	int nId;											// GM���
	unsigned short wsName[OUTER_OBJNAME_LENGTH + 1];	// GM����
	unsigned short wsInfo[1];							// ��Ϣ����
};

// ���ӵ���������
struct server_msg_link_to_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// �����ʶ
	outer_object_t LinkId;		// �����ӵĶ���
	outer_posi_t LinkPos;		// ����λ��
};

// ȡ������
struct server_msg_unlink_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// �����ʶ
};

// �ı�����λ��
struct server_msg_link_move_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// �����ʶ
	outer_object_t LinkId;		// �����ӵĶ���
	outer_posi_t LinkPos;		// ����λ��
};

// �����������Ըı�
struct server_msg_all_prop_t: public server_msg_base_t
{
	unsigned short nCount;		// ��������
	// object id
	// prop info
	// ...
};

// ���Ӷ������
struct server_msg_add_more_object_t: public server_msg_base_t
{
	unsigned short nCount;		// ��������
	// object id
	// posi
	// dest
	// prop count
	// prop info
	// ...
};

// �Ƴ��������
struct server_msg_remove_more_object_t: public server_msg_base_t
{
	unsigned short nCount;		// ��������
	// object id
	// ...
};

// ���ͻ��˷��ͼƷ���֤��
struct server_msg_charge_validstring: public server_msg_base_t
{
	// �Ʒ�ϵͳ���ش�
	char	strValidateString[VALIDATE_LEN_LENGTH+1];	// �����´ε�½����֤��
};

// ��ȫ��¼
struct server_msg_security_login_t: public server_msg_base_t
{
	unsigned short nCode;	// �����룬1Ϊ�ɹ�������ʧ��
};

// Զ�̿��Ʒ�������Ӧ
struct server_msg_reponse_ctrl_t: public server_msg_base_t
{
	unsigned short nType;	// ��������
	unsigned short nCode;	// �����룬1Ϊ�ɹ�������ʧ��
	char strInfo[1];		// ������Ϣ
};

// �������ӳٶ���
struct server_msg_measure_delay: public server_msg_base_t
{
	unsigned int nIndex;
	unsigned int nServerTime;
};

#pragma pack(pop)

#endif // _SERVER_OUTERMSG_H
