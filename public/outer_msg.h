//--------------------------------------------------------------------
// 文件名:		OuterMsg.h
// 内  容:		通用服务器引擎，客户端和服务器通讯的消息定义
// 说  明:		
// 创建日期:	2008年9月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_OUTERMSG_H
#define _SERVER_OUTERMSG_H

#include "len_type.h"

// 数组长度如为1,表示可变长

#pragma pack(push, 1)

// 通讯协议的版本号
const int OUTER_PROTOCOL_VERSION = 0x300;
// 最大名字长度
const int OUTER_OBJNAME_LENGTH = 32;
// 一次性验证串长度
const size_t VALIDATE_LEN_LENGTH = 1024;

// 属性数据类型
enum OUTER_DATA_TYPE_ENUM
{
	OUTER_TYPE_UNKNOWN,	// 未知
	OUTER_TYPE_BYTE,	// 一字节
	OUTER_TYPE_WORD,	// 二字节
	OUTER_TYPE_DWORD,	// 四字节
	OUTER_TYPE_QWORD,	// 八字节
	OUTER_TYPE_FLOAT,	// 浮点四字节
	OUTER_TYPE_DOUBLE,	// 浮点八字节
	OUTER_TYPE_STRING,	// 字符串，前四个字节为长度
	OUTER_TYPE_WIDESTR,	// UNICODE宽字符串，前四个字节为长度
	OUTER_TYPE_OBJECT,	// 对象号
	OUTER_TYPE_MAX,
};

// 对象标识符
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

// 位置数据
struct outer_posi_t
{
	len_t X;
	len_t Y;
	len_t Z;
	len_t Orient;
};

// 移动目标数据
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

// 对象的移动目标
struct outer_object_dest_t
{
	outer_object_t ObjectId;	// 对象标识
	outer_dest_t dest;			// 移动目标
};

// 对象的移动目标
struct outer_object_dest_ex_t
{
	outer_object_t ObjectId;	// 对象标识
	outer_dest_t dest;			// 移动目标
	unsigned int nDelayTime;	// 目标延迟
};

// 对象的移动目标
struct outer_object_dest_frame_t
{
	outer_object_t	object_id_;	// 对象标识
	len_t			src_pos_x_;	// 原pos_x
	len_t			src_pos_y_;	// 原pos_y
	len_t			src_pos_z_;	// 原pos_z
	len_t			orient_;	// 原orient
	len_t			accelerate_speed_;	// 线性加速度
	outer_dest_t	dest_;	// 移动目标
};
// client --> server messages
// 客户端发给服务器的消息

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
	CLIENT_MEASURE_DELAY_ACK,	// 客户端延迟测量返回

	// 安全消息定义
	CLIENT_SECURITY_LOGIN	= 20,
	CLIENT_REQ_OUTLINE_STAT = 21,
	CLIENT_REQ_DETAIL_STAT  = 22,
	CLIENT_REQ_REMOTE_CTRL  = 23,

	CLIENT_STUB_MSG_TO_SERVER,	// 本地服消息到member服

	CLIENT_MESSAGE_MAX,
};

// 客户端消息基类
struct client_msg_base_t
{
	unsigned char nMsgType;		// 消息类别
};
struct client_stub_msg_base_t: public client_msg_base_t
{
	unsigned char nSubMsgType;	// 消息类别

};

struct client_stub_msg_to_server: public client_stub_msg_base_t
{

};


// 请求验证码
struct client_msg_get_verify_t: public client_msg_base_t
{
};

// 加密改变确认
struct client_msg_return_encode_t: public client_msg_base_t
{
	char strInfo[32];
	int nAddress;
	unsigned char nVerify[16];
};

// 登录类型
enum LOGIN_TYPE_ENUM
{
	LOGIN_TYPE_NORMAL		= 1,  // 默认登录类型(账号、密码模式)
	LOGIN_TYPE_VAILDSRING	= 2,  // 串登录
	LOGIN_TYPE_SHIELD		= 4,  // 牛盾登录
	LOGIN_TYPE_MOBILESHIELD = 5,  // 手机牛盾登录
	LOGIN_TYPE_RECONNECT	= 100,// 重连串登录
	LOGIN_TYPE_MAX,
};

// 登录
struct client_msg_login_t: public client_msg_base_t
{
	int nVersion;			// 版本号
	char strVerify[16];		// 验证码
	//string account		// 帐号
	//string password		// 密码
	//string loginstring	// 登录串
	//int	login_type		// 登录类型
};

// 获得游戏世界信息（用于人物创建或其他目的）
struct client_msg_world_info_t: public client_msg_base_t
{
	int nInfoType;		// 信息类型
	//unsigned short wsInfo[1];	// 附加信息
};

// 选择角色
struct client_msg_choose_role_t: public client_msg_base_t
{
	unsigned short wsName[OUTER_OBJNAME_LENGTH + 1];	// 名称
	unsigned char nVerify[16];							// 校验码
	char strInfo[1];									// 附加信息
};

// 创建角色
struct client_msg_create_role_t: public client_msg_base_t
{
	int nIndex;
	unsigned short wsName[OUTER_OBJNAME_LENGTH + 1];	// 名称
	unsigned short nArgNum;								// 参数数量
	//unsigned char	arg_type;							// 参数类型
	// 参数数据
};

// 删除角色
struct client_msg_delete_role_t: public client_msg_base_t
{
	unsigned short wsName[OUTER_OBJNAME_LENGTH + 1];	// 名称
};

// 恢复角色
struct client_msg_revive_role_t: public client_msg_base_t
{
	unsigned short wsName[OUTER_OBJNAME_LENGTH + 1];	// 名称
};

// 场景中选择对象
struct client_msg_select_t: public client_msg_base_t
{
	unsigned char nVerify[16];	// 校验码
	int nSerial;				// 消息序列号	
	outer_object_t ObjectId;	// 对象标识
	int nFunctionId;			// 功能号为0表示执行缺省功能
};

// 普通聊天 
struct client_msg_speech_t: public client_msg_base_t
{
	unsigned short wsContent[1];	// 话语内容
};

// 客户端通用定制消息
struct client_msg_custom_t: public client_msg_base_t
{
	unsigned char nVerify[16];	// 校验码
	int nSerial;				// 消息序列号	
	unsigned short nArgNum;		// 参数数量
	//unsigned char	arg_type;	// 参数类型
	// 参数数据
};

// 客户端就绪
struct client_msg_ready_t: public client_msg_base_t
{
};

// 移动请求
struct client_msg_request_move_t: public client_msg_base_t
{
	unsigned char nMode;	// 移动模式
	unsigned short nArgNum;	// 浮点参数数量
	//len_t args[1]			// 浮点参数数组
	//char info[1]			// 消息的末尾可能包含以0结束的附加字符串信息
};

// 安全登录
struct client_msg_security_login_t: public client_msg_base_t
{
	char strVerify[128];	// 验证串
};

// 请求远程控制服务器
struct client_msg_request_ctrl_t: public client_msg_base_t
{
	unsigned short nType;	// 操作类型
	char strInfo[1];		// 附加信息
};

struct client_msg_measure_delay_ack_t: public client_msg_base_t
{
	unsigned int nIndex;
	unsigned int nServerTime;
};

// server --> client messages
// 服务器发到客户端的消息

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
	SERVER_CP_CUSTOM,			// 压缩的自定义消息
	SERVER_CP_ADD_OBJECT,		// 压缩的添加可见对象消息
	SERVER_CP_RECORD_ADDROW,	// 压缩的表格添加行消息
	SERVER_CP_VIEW_ADD,			// 压缩的容器添加对象消息
	SERVER_VIEW_CHANGE,			// 容器改变
	SERVER_CP_ALL_DEST,			// 压缩的对象移动消息
	SERVER_ALL_PROP,			// 多个对象的属性改变信息
	SERVER_CP_ALL_PROP,			// 压缩的多个对象的属性改变信息
	SERVER_ADD_MORE_OBJECT,		// 增加多个对象
	SERVER_CP_ADD_MORE_OBJECT,	// 压缩的增加多个对象
	SERVER_REMOVE_MORE_OBJECT,	// 移除多个对象
	SERVER_CHARGE_VALIDSTRING,  // 将计费服务器返回的验证串发给客户端

	// 安全消息定义
	SERVER_SECURITY_LOGIN	= 51,
	SERVER_REP_OUTLINE_STAT = 52,
	SERVER_REP_DETAIL_STAT	= 53,
	SERVER_REP_REMOTE_CTRL	= 54,
	SERVER_MOVING_EX,			// 带延时的移动消息
	SERVER_ALL_DEST_EX,			// 带延时的移动消息集合
	SERVER_CP_ALL_DEST_EX,		// 压缩带延时的对象移动消息
	SERVER_ENTRY_SCENE_EX,		// 支持帧同步的进入场景消息
	SERVER_MEASURE_DELAY,		// 服务器延迟度量
	SERVER_FRAME_UPDATE,        // 逻辑帧更新
	SERVER_FRAME_MOVING,        // 基于逻辑帧的移动 -- 移动key
	SERVER_FRAME_MOVING_DIFF,   // 基于逻辑帧的移动 -- 移动差值

	SERVER_MSG_TO_STUB,			// 服务端消息到客户端本地服
	STUB_MSG_TO_CLIENT,			// 本地服消息到client
	SERVER_MESSAGE_MAX,
};

// 服务器消息基类
struct server_msg_base_t
{
	unsigned char nMsgType;		// 消息类别
};
struct server_stub_msg_base_t: public server_msg_base_t
{
	unsigned char nSubMsgType;	// 子消息类别
	unsigned short nArgNum;		// 参数数量
	//unsigned char arg_type;	// 参数类型
	// 参数数据

};

// 发送到本地服的消息
struct server_msg_to_stub: public server_stub_msg_base_t
{

};

struct stub_msg_to_client: public server_stub_msg_base_t
{

};


// 验证码
struct server_msg_set_verify_t: public server_msg_base_t
{
	int nWidth;		// 图片宽度	
	int nHeight;	// 图片高度
	int pData[1];	// 像素
};

// 改变加密方法
struct server_msg_set_encode_t: public server_msg_base_t
{
	char strInfo[32];
	int nDynamicKey;
	int nEncodeId;
	int nAddress;
	unsigned char nCondition[16];
	unsigned char pData[1];
};

// 返回错误信息
struct server_msg_error_code_t: public server_msg_base_t
{
	int nResult;		// 错误码
};

// 登录信息
struct server_msg_login_succeed_t: public server_msg_base_t
{
	int nIsFree;		// 是否免费
	int nPoints;		// 剩余点数
	int nYear;			// 包月截止日期
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;
	int nSecond;
	int nDynamicKey;	// 动态密匙
	int nRoles;			// 角色数
	//unsigned short arg_num;	// 第一个角色的参数数量
	// 按照取自定义消息相似的方法取角色信息
	//unsigned char type;
	//int sys_flags;			// 角色的系统标志
	//unsigned char type;
	//wstring name;				// 角色名称
	//unsigned char type;
	//wstring para;				// 角色数据
	//unsigned char type;
	//int deleted;				// 角色是否被删除
	//unsigned char type;
	//double delete_time;		// 角色删除时间
	//string cross_addr;		// 跨服服务器的列表地址(idc1:ip1:port1,idc2:ip2:port2),不在跨服时为空
	// ...						// 可能有其他的数据
	//unsigned short arg_num;	// 下个角色的参数数量
	// ...
};

// 游戏世界信息
struct server_msg_world_info_t: public server_msg_base_t
{
	unsigned short nInfoType;	// 信息类型
	unsigned short wsInfo[1];	// 信息内容
};

// 空闲
struct server_msg_idle_t: public server_msg_base_t
{
};

// 排队（当服务器连接数量超过上限时玩家的连接进入排队状态）
struct server_msg_queue_t: public server_msg_base_t
{
	int nQueue;		// 队列编号
	int nPosition;	// 在队列中的位置（为0表示结束排队）
	int nPriorCount; // 绿色通道人数
};

// 强行终止
struct server_msg_terminate_t: public server_msg_base_t
{
	int nResult;	// 原因
};

// 属性表
struct server_msg_property_table_t: public server_msg_base_t
{
	unsigned short nCount;	// 属性数量
	//string name;			// 属性名称，以0结束的字符串，前面不包含长度
	//unsigned char type	// 属性类型
};

// 所有表格的信息
struct server_msg_record_table_t: public server_msg_base_t
{
	unsigned short nCount;		// 表格数量
	//string name;				// 表格名称，以0结束的字符串，前面不包含长度
	//unsigned short cols;		// 列数，最大256
	//unsigned char col_type[];	// 列类型
	// ... 其他表的信息
};

// 进入场景
struct server_msg_entry_scene_t: public server_msg_base_t
{
	int nDynamicKey;			// 动态密匙
	outer_object_t ObjectId;	// 主角玩家的对象标识
	unsigned short nCount;		// 后续场景属性数量
	// 属性数据
	//unsigned short property_index
	//property_data
	// 后续其他的属性，根据属性表进行解码
};

// 进入场景
struct server_msg_entry_scene_ex_t: public server_msg_base_t
{
	int nDynamicKey;			// 动态密匙
	int scene_motion_mode_;		// 移动方式
	int frame_time_delta_;		// 每帧代表的逻辑时间
	len_t move_line_diff_ratio_;// 移动线形比率。每个单位差值代表的实际距离。
	len_t move_rotate_diff_ratio_;// 移动角度比率。每个单位差值代表的实际角度。

	outer_object_t ObjectId;	// 主角玩家的对象标识
	unsigned short nCount;		// 后续场景属性数量
	// 属性数据
	//unsigned short property_index
	//property_data
	// 后续其他的属性，根据属性表进行解码
};
// 退出场景
struct server_msg_exit_scene_t: public server_msg_base_t
{
};

// 加入游戏对象
struct server_msg_add_object_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// 对象标识
	outer_posi_t posi;			// 对象位置
	outer_dest_t dest;			// 移动目标
	unsigned short nCount;		// 后续属性数量
	// 属性数据
	//unsigned short property_index
	//property_data
	// 后续其他的属性，根据属性表进行解码
};

// 删除游戏对象
struct server_msg_remove_object_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// 对象标识
};

// 场景属性
struct server_msg_scene_property_t: public server_msg_base_t
{
	unsigned short nCount;		// 后续属性数量
	// 属性数据
	//unsigned short property_index
	//property_data
	// 后续其他的属性，根据属性表进行解码
};

// 对象属性
struct server_msg_object_property_t: public server_msg_base_t
{
	unsigned char nIsViewObj;	// 是否容器中对象
	outer_object_t ObjectId;	// 对象标识
	unsigned short nCount;		// 后续属性数量
	// 属性数据
	//unsigned short property_index
	//property_data
	// 后续其他的属性，根据属性表进行解码
};

// 表数据添加行
struct server_msg_record_add_row_t: public server_msg_base_t
{
	unsigned char nIsViewObj;	// 是否容器中对象
	outer_object_t ObjectId;	// 对象标识
	unsigned short nIndex;		// 表序号
	unsigned short nRow;		// 插入的行号
	unsigned short nRows;		// 行数
	// 数据
};

// 表数据删除行
struct server_msg_record_remove_row_t: public server_msg_base_t
{
	unsigned char nIsViewObj;	// 是否容器中对象
	outer_object_t ObjectId;	// 对象标识
	unsigned short nIndex;		// 表序号
	unsigned short nRow;		// 删除的行号
};

// 表数据改变
struct server_msg_record_grid_t: public server_msg_base_t
{
	unsigned char nIsViewObj;	// 是否容器中对象
	outer_object_t ObjectId;	// 对象标识
	unsigned short nIndex;		// 表序号
	unsigned short nCount;		// 数据数量
	//unsigned short row
	//unsigned char col		
	// 数据
};

// 表数据清空
struct server_msg_record_clear_t: public server_msg_base_t
{
	unsigned char nIsViewObj;	// 是否容器中对象
	outer_object_t ObjectId;	// 对象标识
	unsigned short nIndex;		// 表序号
};

// 添加视窗
struct server_msg_create_view_t: public server_msg_base_t
{
	unsigned short nViewId;		// 视窗编号, 0-保留给场景
	unsigned short nCapacity;	// 容量
	unsigned short nCount;		// 后续属性数量
	// 属性数据
	//unsigned short property_index
	//property_data
	// 后续其他的属性，根据属性表进行解码
};

// 删除视窗
struct server_msg_delete_view_t: public server_msg_base_t
{
	unsigned short nViewId;		// 视窗编号
};

// 视窗属性
struct server_msg_view_property_t: public server_msg_base_t
{
	unsigned short nViewId;		// 视窗编号
	unsigned short nCount;		// 后续属性数量
	// 属性数据
	//unsigned short property_index
	//property_data
	// 后续其他的属性，根据属性表进行解码
};

// 视窗中添加对象
struct server_msg_view_add_t: public server_msg_base_t
{
	unsigned short nViewId;		// 视窗编号
	unsigned short nObjectId;	// 视窗对象号
	unsigned short nCount;		// 后续属性数量
	// 属性数据
	//unsigned short property_index
	//property_data
	// 后续其他的属性，根据属性表进行解码
};

// 视窗中删除对象
struct server_msg_view_remove_t: public server_msg_base_t
{
	unsigned short nViewId;		// 视窗编号
	unsigned short nObjectId;	// 视窗对象号
};

// 视窗中对象改变位置
struct server_msg_view_change_t: public server_msg_base_t
{
	unsigned short nViewId;			// 视窗编号
	unsigned short nOldObjectId;	// 旧视窗对象号
	unsigned short nNewObjectId;	// 新视窗对象号
};

// 普通聊天
struct server_msg_speech_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// 对象号	
	unsigned short wsContent[1];// 话语内容
};

// 系统信息
struct server_msg_system_info_t: public server_msg_base_t
{
	unsigned short nInfoType;	// 信息类型
	unsigned short wsInfo[1];	// 信息内容
};

// NPC对话选单
struct server_msg_menu_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// 对象标识
	unsigned short nCount;		// 对话信息数量
	//unsigned char type;		// 类型（如：0纯文字，1过场，2选项）
	//unsigned short mark;		// 标记
	//unsigned short size;		// 文字长度
	//unsigned short info[1];	// 文字信息
};

// 清除菜单
struct server_msg_clear_menu_t: public server_msg_base_t
{
};

// 脚本定制消息
struct server_msg_custom_t: public server_msg_base_t
{
	unsigned short nArgNum;		// 参数数量
	//unsigned char arg_type;	// 参数类型
	// 参数数据
};

// 对象位置和目标
struct server_msg_location_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// 对象标识
	outer_posi_t posi;			// 位置
};

// 对象运动目标
struct server_msg_moving_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// 对象标识
	outer_dest_t dest;			// 移动目标
};

// 对象运动目标(带消息延时)
struct server_msg_moving_ex_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// 对象标识
	outer_dest_t dest;			// 移动目标
	unsigned int nDelayTime;	// 目标延迟
};
// 多个对象的目标同步
struct server_msg_all_dest_t: public server_msg_base_t
{
	unsigned short nCount;			// 对象数量
	outer_object_dest_t	info[1];	// 对象移动目标
};
// 多个对象的目标同步(带消息延时)
struct server_msg_all_dest_ex_t: public server_msg_base_t
{
	unsigned short nCount;			// 对象数量
	outer_object_dest_ex_t	info[1];	// 对象移动目标
};


// 服务器逻辑帧 -- 客户端收到逻辑帧后,进行基于帧的逻辑计算
struct server_msg_frame_t: public server_msg_base_t
{
	unsigned int frame_id_;			// 逻辑帧
};

// 对象运动目标--基于逻辑帧
struct server_msg_dest_frame_t: public server_msg_base_t
{
	outer_object_dest_frame_t info_;	// 移动目标
};

// 对象移动差值数据
struct outer_frame_move_diff_t
{
	//unsigned char object_visual_index_; // 物体可视索引
	outer_object_t object_id_;				// 物体ID，验证可行后才添加索引控制
	unsigned short object_move_diff_;		// 物体移动差值数据 x 4bit, y 4bit, z 4bit, orient 4bit.
};

// 对象运动目标--基于逻辑帧
struct server_msg_dest_frame_diff_t: public server_msg_base_t
{
	unsigned short count_;				// 对象数量
	outer_frame_move_diff_t	info_[1];	// 对象移动差值数据
};

// 系统告警
struct server_msg_warning_t: public server_msg_base_t
{
	unsigned short nType;		// 告警类型
	unsigned short wsInfo[1];	// 告警内容
};

// GM回应
struct server_msg_from_gmcc_t: public server_msg_base_t
{
	int nId;											// GM编号
	unsigned short wsName[OUTER_OBJNAME_LENGTH + 1];	// GM名称
	unsigned short wsInfo[1];							// 信息内容
};

// 连接到其他对象
struct server_msg_link_to_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// 对象标识
	outer_object_t LinkId;		// 被连接的对象
	outer_posi_t LinkPos;		// 连接位置
};

// 取消连接
struct server_msg_unlink_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// 对象标识
};

// 改变连接位置
struct server_msg_link_move_t: public server_msg_base_t
{
	outer_object_t ObjectId;	// 对象标识
	outer_object_t LinkId;		// 被连接的对象
	outer_posi_t LinkPos;		// 连接位置
};

// 多个对象的属性改变
struct server_msg_all_prop_t: public server_msg_base_t
{
	unsigned short nCount;		// 对象数量
	// object id
	// prop info
	// ...
};

// 增加多个对象
struct server_msg_add_more_object_t: public server_msg_base_t
{
	unsigned short nCount;		// 对象数量
	// object id
	// posi
	// dest
	// prop count
	// prop info
	// ...
};

// 移除多个对象
struct server_msg_remove_more_object_t: public server_msg_base_t
{
	unsigned short nCount;		// 对象数量
	// object id
	// ...
};

// 给客户端发送计费验证串
struct server_msg_charge_validstring: public server_msg_base_t
{
	// 计费系统返回串
	char	strValidateString[VALIDATE_LEN_LENGTH+1];	// 用于下次登陆的验证串
};

// 安全登录
struct server_msg_security_login_t: public server_msg_base_t
{
	unsigned short nCode;	// 返回码，1为成功，其他失败
};

// 远程控制服务器响应
struct server_msg_reponse_ctrl_t: public server_msg_base_t
{
	unsigned short nType;	// 操作类型
	unsigned short nCode;	// 返回码，1为成功，其他失败
	char strInfo[1];		// 附加信息
};

// 服务器延迟度量
struct server_msg_measure_delay: public server_msg_base_t
{
	unsigned int nIndex;
	unsigned int nServerTime;
};

#pragma pack(pop)

#endif // _SERVER_OUTERMSG_H
