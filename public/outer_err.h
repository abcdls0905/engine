//--------------------------------------------------------------------
// 文件名:		outer_err.h
// 内  容:		通用服务器引擎，客户端和服务器通讯的错误代码
// 说  明:		
// 创建日期:	2008年9月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_OUTER_ERR_H
#define _SERVER_OUTER_ERR_H

// 错误码信息格式 [5位整型数]
// 第1位是子系统代码
// 第2位是子模块代码
// 第3位是类型(0-存储过程、1-程序错误、2-网络通信)
// 后两位为顺序号编码
// 1000为不可预知的错误

#define OUTER_RESULT_SUCCEED		1		// 结果成功
#define OUTER_RESULT_FAILED			0		// 结果失败

// 1000为不可预知的错误
#define OUTER_ERR_UNEXPECT			1000	// 非预期错误

// 游戏服务器相关错误码描述[2xxxx]

// 网络接口相关错误[20xxx]

#define MGS_ERR_VERSION				20101	// 网络通讯协议版本错误
#define MGS_ERR_REPEAT_LOGIN		20102	// 同一个帐号不能重复登陆
#define MGS_ERR_LAND_SCENE			20103	// 要进入的场景未开放或已关闭
#define MGS_ERR_NO_CONNECT			20104	// 连接数满
#define MGS_ERR_BLOCK_MOMENT		20105	// 在一段时间内禁止登陆
#define MGS_ERR_TIME_OUT			20106	// 登录超时
#define MGS_ERR_QUEUE_FULL			20107	// 登录队列满
#define MGS_ERR_STORAGE_FAILED		20108	// 服务器与角色数据库的连接故障
#define MGS_ERR_CHAREGE_FAILED		20109	// 服务器与计费服务器的连接故障
#define MGS_ERR_SWITCH_SCENE		20110	// 要切换的场景不存在
#define MGS_ERR_ACCOUNT_LOGGED		20111	// 此帐号已经登陆此游戏服务器
#define MGS_ERR_ACCOUNT_INVALID		20112	// 帐号密码中包含非法字符
#define MGS_ERR_CHARGE_BREAK		20113	// 因费用不足已经被系统踢出
#define MGS_ERR_PLAYER_FULL			20114	// 承载人数已经达到服务器上限
#define MGS_ERR_NO_ANY_SCENE		20115	// 没有可使用的场景
#define MGS_ERR_KICK_OUT			20116	// 被其他玩家踢下线
#define MGS_ERR_IN_PROCESSING		20117	// 正在进行处理中
#define MGS_ERR_CANT_PROCESS		20118	// 无法处理请求
#define MGS_ERR_MESSAGE_ILLEGAL		20119	// 涉嫌使用作弊程序向服务器发送非法消息
#define MGS_ERR_ACCOUNT_IN_GAME		20120	// 此帐号已经进入游戏
#define MGS_ERR_CANT_REPLACE		20121	// 使用密保卡登录的不能被其他不同IP顶替
#define MGS_ERR_SUPERVISOR_KICK_OUT	20122	// 被管理员踢下线
#define MGS_ERR_SUPERVISOR_LOGGED	20123	// 登录类型为管理员，不能被踢下线
#define MGS_ERR_DENY_ENTRY_SCENE	20124	// 当前服务器不允许进入场景

// 存储接口相关错误[21xxx]

#define MGS_ERR_ROLE_DUPLICATE		21101	// 人物重名
#define MGS_ERR_ROLE_PROTECT		21102	// 人物在保护期内，不可删除
#define MGS_ERR_ROLE_SAVE_FAIL		21103	// 人物存盘失败
#define MGS_ERR_ROLE_DATA			21104	// 人物数据错误
#define MGS_ERR_ROLE_TOO_MANY		21105	// 超过总的人物数量限制
#define MGS_ERR_ROLE_INDEX			21106	// 人物位置错误
#define MGS_ERR_ROLE_NOT_FOUND		21107	// 人物没找到
#define MGS_ERR_NAME_REPEAT			21108	// 名字重复
#define MGS_ERR_NAME_INVALID		21109	// 名字包含非法字符
#define MGS_ERR_NAME_EXISTS			21110	// 此名称已存在
#define MGS_ERR_STORAGE_BUSY		21111	// 数据库忙，无法处理请求
#define MGS_ERR_CHARGE_BUSY			21112	// 计费系统忙，无法处理请求
#define MGS_ERR_LETTER_TOO_MANY		21113	// 信件数量超过上限
#define MGS_ERR_NO_ROLE_UID			21114	// 创建人物时编号资源不足
#define MGS_ERR_SWITCH_NO_SERVER	21115	// 人物转服时服务器不存在
#define MGS_ERR_SWITCH_CAN_NOT		21116	// 此服务器不能执行人物转服
#define MGS_ERR_ROLE_TOO_BIG		21117	// 人物存盘数据超过允许的上限
#define MGS_ERR_STATUS_CHANGED		21118	// 转服时目标服务器的状态发生改变
#define MGS_ERR_SET_LAND_SCENE		21119	// 新建人物时未设置登陆场景
#define MGS_ERR_NEW_ROLE_FAILED		21120	// 新建人物时保存数据失败
#define MGS_ERR_ROLE_BLOCKED		21121	// 人物数据在一段时间内被封闭
#define MGS_ERR_LOGON_FAILED		21122	// 计费登陆失败
#define MGS_ERR_LOAD_ROLE_FAILED	21123	// 加载人物数据失败
#define MGS_ERR_GET_ROLES_FAILED	21124	// 获取角色列表失败
#define MGS_ERR_CREATE_ROLE_FAILED	21125	// 创建人物数据失败
#define MGS_ERR_DELETE_ROLE_FAILED	21126	// 删除人物数据失败
#define MGS_ERR_RECEIVER_NOT_FOUND	21127	// 收信人不存在
#define MGS_ERR_ROLE_IN_USE			21128	// 角色正在使用中
#define MGS_ERR_VALIDATE_FAILED		21129	// 验证帐号失败
#define MGS_ERR_CONTINUE_FAILED		21130	// 顶替登录失败
#define MGS_ERR_ROLE_LOCKED			21131	// 角色被暂时锁定
#define MGS_ERR_DENY_CREATE_ROLE	21132	// 禁止创建新人物

// 计费应用相关错误码描述[5xxxx]

// 服务器注册、注销 50xxx
#define CAS_ERR_SERVER_ID			50001	// 游戏服务器ID不可识别
#define CAS_ERR_SERVER_FORBID		50002	// 游戏服务器状态为已禁用
#define CAS_ERR_SERVER_IP			50003	// 游戏服务器注册IP不正确
#define CAS_ERR_GAME_ID				50004	// 游戏类型ID不正确

// 玩家帐号登录、扣点、退出 51xxx
#define CAS_ERR_NO_ACCOUNT			51001	// 帐号不存在
#define CAS_ERR_ACCOUNT_PSWD		51002	// 帐号或密码错误
#define CAS_ERR_ACCOUNT_BLOCK		51003	// 此帐号已被冻结
#define CAS_ERR_ACCOUNT_ONLY		51004	// 实名帐号只能登录绿色服
#define CAS_ERR_ACCOUNT_DENIED		51005	// 普通帐号不能登录绿色服
#define	CAS_ERR_ILLEGAL_GM			51006	// 非法GM帐号登录
#define CAS_ERR_UNACTIVE			51007	// 此分区帐户尚未激活
#define	CAS_ERR_EXPIRED				51008	// 分区游戏帐户余额不足
#define CAS_ERR_SAVE_LOG			51009	// 创建登录日志失败
#define CAS_ERR_GET_ITEM			51010	// 获取物品失败，物品不存在或是之前已取完
#define CAS_ERR_ACCOUNT_LOGGED		51011	// 此帐号已在分区内其他服务器登录
#define CAS_ERR_ACCOUNT_LOCKED		51012	// 帐号为锁定状态，请通过手机解锁后再试
#define CAS_ERR_ACCOUNT_FORBID		51013	// 您的帐号不可以登录此游戏服务器(帐号与运营商不匹配),请到会员中心查看自己能登录哪些游戏服务器.

// 点卡寄售、购买、道具购买 52xxx
#define CAS_ERR_CARD_ACCOUNT		52000	// 未知帐号请求
#define CAS_ERR_CARD_PSWD			52001	// 卡号或密码不正确
#define CAS_ERR_CARD_INVALID		52002	// 无效点卡，已被使用
#define CAS_ERR_CARD_BLOCK			52003	// 点卡目前已在寄售中
#define CAS_ERR_CARD_DISCARD		52004	// 无效点卡，已被作废
#define CAS_ERR_NO_POINTS			52005	// 中心帐户余额不足
#define CAS_ERR_CARD_FILL			52006	// 购买点卡充值时出错
#define CAS_ERR_CARD_TYPE			52007	// 卡类型不可识别，不允许寄售
#define CAS_ERR_CARD_SELLED			52008	// 请求失败,点卡之前已经售出
#define CAS_ERR_CARD_CANCEL			52009	// 请求失败,点卡可能已撤消寄售
#define CAS_ERR_CARD_SELL_CANCEL	52010	// 点卡可能之前已售出或已撤消
#define CAS_ERR_CARD_CANT_CANCEL	52011	// 点卡寄售两小时内不可撤消
#define CAS_ERR_ITEM_NOT_FOUND		52012	// 虚拟道具编码不存在
#define CAS_ERR_ITEM_AMOUNT_PRICE	52013	// 购买数量或单价有误
#define CAS_ERR_ITEM_PRICE_CHANGE	52014	// 物品点数单价发生变化,请刷新

#endif // _SERVER_OUTER_ERR_H

