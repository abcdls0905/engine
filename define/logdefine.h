#ifndef __LogDefine_h__
#define __LogDefine_h__

// Log等级
enum LogLevelEnum
{
    LOG_TRACE_LCH = -20,
    LOG_TRACE_LCH_DEBUG_DEBUG = -15,
    LOG_TRACE_LCH_SKILL_CONNECT = - 10,
    LOG_TRACE_LCH_PROP = -9,
    LOG_TRACE_LCH_BUFFER_A = -8,
    LOG_TRACE_LCH_BUFFER_B = -7,
    LOG_TRACE_LCH_SKILL_A = -6,
    LOG_TRACE_LCH_SKILL_B = -5,
    LOG_TRACE_LCH_FIGHT_A = -4,
    LOG_TRACE_LCH_FIGHT_B = -3,

    LOG_FPS_INFO = -1,

    LOG_TRACE_PERFORMANCE,
    LOG_TRACE = 1,  /**< 程序调试级别日志 */
    LOG_INFO,       /**< 系统跟踪日志 */
    LOG_WARNING,    /**< 警告日志 */
    LOG_ERROR,      /**< 错误日志 */
    LOG_CRITICAL    /**< 严重错误(崩溃级别，慎用) */
};


// $@ 1 = 任务日志类型
enum TASK_LOG_TYPE
{
    TASK_LOG_MIN        = 10000,    // $ 10000 = 任务日志类型最小值
    TASK_LOG_ACCEPTED   = 10001,    // $ 10001 = 接受任务
    TASK_LOG_COMPLETED  = 10002,    // $ 10002 = 完成任务
    TASK_LOG_CANCEL     = 10003,    // $ 10003 = 放弃任务
    TASK_LOG_FAILED     = 10004,    // $ 10004 = 任务失败
    TASK_LOG_MAX        = 10999,    // $ 10999 = 任务日志类型最大值
};

// $@ 2 = 游戏玩法日志类型
enum GAME_ACTION_LOG_TYPE
{
    GAME_ACTION_LOG_MIN = 11000,            // $ 11000 = 游戏玩法日志类型最小值
    GAME_ACTION_LOG_START = 11001,          // $ 11001 = 玩法开始
    GAME_ACTION_LOG_FINISH = 11002,         // $ 11002 = 玩法完成
    GAME_ACTION_LOG_UNFINISHED = 11003,     // $ 11003 = 玩法未完成
    GAME_ACTION_LOG_MAX = 11999,            // $ 11999 = 游戏玩法日志类型最大值
};

// $@ 3 = 游戏金钱异动日志类型
enum MONEY_LOG_TYPE
{
    MONEY_LOG_MIN = 12000,                          // $ 12000 = 游戏金钱异动日志类型最小值

    MONEY_LOG_GAIN_MIN = 12001,                     // $ 12001 = 获得游戏金钱日志类型最小值
    MONEY_LOG_GAIN_TASK = 12002,                    //$ 12002 = 完成任务奖励增加金钱
    MONEY_LOG_GAIN_EXCHANGE = 12003,                //$ 12003 = 玩家互相交易增加金钱
    MONEY_LOG_GAIN_PICKUP = 12004,                  //$ 12004 = 副本中杀怪掉落金钱
    MONEY_LOG_GAIN_CLONE = 12005,                   //$ 12005 = 通关副本奖励金钱
    MONEY_LOG_GAIN_GM = 12006,                      //$ 12006 = 通过GM的lua增加金钱
    MONEY_LOG_GAIN_DEPOT = 12007,                   //$ 12007 = 仓库取钱
    MONEY_LOG_GAIN_SELL_ITEM = 12008,               //$ 12008 = 向NPC出售物品获取金钱
    MONEY_LOG_GAIN_POST = 12009,                    //$ 12009 = 提取邮件中的金钱
    MONEY_LOG_GAIN_FIGHT_REWARD = 12010,            //$ 12010 = 竞技场奖励声望
    MONEY_LOG_GAIN_ITEM_GLORY = 12011,              //$ 12011 = 声望兑换物品失败返给的声望
    MONEY_LOG_GAIN_BS = 12012,                      //$ 12012 = 运营平台调用增加金钱
    MONEY_LOG_GAIN_INVEST = 12013,                  //$ 12013 = 投资回报收益金钱
    MONEY_LOG_GAIN_SPORTS = 12014,                  //$ 12014 = 竞技胜利获得奖励金钱
    MONEY_LOG_GAIN_TALENT = 12015,                  //$ 12015 = 激活星位获得奖励
    MONEY_LOG_GAIN_TOOLITEM = 12016,                //$ 12016 = 使用物品获得金钱
    MONEY_LOG_GAIN_CLONE_REWARD = 12017,            //$ 12017 = 副本宝箱奖励金钱
    MONEY_LOG_GAIN_SWEEP_CLONE = 12018,             //$ 12018 = 扫荡副本获得金钱
    MONEY_LOG_GAIN_FURANCE = 12019,                 //$ 12019 = 熔炼/重铸获得金钱
    MONEY_LOG_GAIN_BUY_FAIL = 12020,                //$ 12020 = 购买物品失败退回金钱
    MONEY_LOG_GAIN_PAY = 12021,                     //$ 12021 = 充值获得金钱
	MONEY_LOG_GAIN_BUY_ITEM = 12022,                //$ 12022 = 购买金钱类道具获得金钱
	MONEY_LOG_GAIN_GIFT_ITEM = 12023,               //$ 12023 = 精彩礼包获得的金钱
    MONEY_LOG_GAIN_MAX = 12299,                     // $ 12299 = 获得游戏金钱日志类型最大值

    MONEY_LOG_LOSE_MIN = 12300,                     // $ 12300 = 失去游戏金钱日志类型最小值
    MONEY_LOG_LOSE_GM = 12301,                      //$ 12301 = 通过GM减少金钱
    MONEY_LOG_LOSE_DROP=12302,                      //$ 12302 = 玩家被怪物杀死掉落
    MONEY_LOG_LOSE_COMPOSE=12303,                   //$ 12303 = 宝石合成 
    MONEY_LOG_LOSE_COMPOUND=12304,                  //$ 12304 = 物品合成
    MONEY_LOG_LOSE_STRENGTHEN=12305,                //$ 12305 = 物品强化
    MONEY_LOG_LOSE_APPEND=12306,                    //$ 12306 = 物品附魔
    MONEY_LOG_LOSE_ENHANCE=12307,                   //$ 12307 = 宝石镶嵌
    MONEY_LOG_LOSE_SLOTITEM=12308,                  //$ 12308 = 物品打孔
    MONEY_LOG_LOSE_TRANSPORT=12309,                 //$ 12309 = 交通工具
    MONEY_LOG_LOSE_UNLOCK_BAG=12310,                //$ 12310 = 物品包裹开锁
    MONEY_LOG_LOSE_DEPOT=12311,                     //$ 12311 = 银行存钱
    MONEY_LOG_LOSE_UNLOCK_ROLE=12312,               //$ 12312 = 角色解锁   
    MONEY_LOG_LOSE_CLONE_ENTER=12313,               //$ 12313 = 进入副本消耗
    MONEY_LOG_LOSE_PK=12314,                        //$ 12314 = PK死亡掉落  
    MONEY_LOG_LOSE_SHOP=12315,                      //$ 12315 = 商店购买物品
    MONEY_LOG_LOSE_LEARN_SKILL=12316,               //$ 12316 = 学习技能消耗
    MONEY_LOG_LOSE_AUCTION=12317,                   //$ 12317 = 拍卖行购买物品
    MONEY_LOG_LOSE_RRCLAIM=12318,                   //$ 12318 = 回购商品
    MONEY_LOG_LOSE_EXCHANGE=12319,                  //$ 12319 = 玩家间交易  
    MONEY_LOG_LOSE_CREATE_GUILD=12320,              //$ 12320 = 创建公会
    MONEY_LOG_LOSE_GUILD_DONATE=12321,              //$ 12321 = 公会贡献
    MONEY_LOG_LOSE_POST_REMIT = 12322,              //$ 12322 = 付费邮件
    MONEY_LOG_LOSE_POST_SEND = 12323,               //$ 12323 = 发送邮件付费
	MONEY_LOG_LOSE_MALL_ITEM = 12324,               //$ 12324 = 商城购买道具付费
	MONEY_LOG_LOSE_MALL_EXCHANGE = 12325,           //$ 12325 = 商城兑换金币付费
	MONEY_LOG_LOSE_MALL_VIP_ITEM = 12326,			//$ 12326 = 商城购买vip道具付费
    MONEY_LOG_LOSE_ITEM_GLORY = 12327,              //$ 12327 = 兑换物品消耗声望
    MONEY_LOG_LOSE_BUY_PEACE = 12328,               //$ 12328 = 购买和平模式消耗
    MONEY_LOG_LOSE_BUY_INVEST = 12329,              //$ 12329 = 购买投资品消耗
    MONEY_LOG_LOSE_TALENT = 12330,                  //$ 12330 = 激活星位消耗
	MONEY_LOG_LOSE_RESET_CLONE_SAODANG = 12331,		//$ 12331 = 重置扫荡时间或挑战次数付费
    MONEY_LOG_LOSE_SKILL = 12332,		            //$ 12332 = 技能消耗
    MONEY_LOG_LOSE_UPDATE_SKILL = 12333,            //$ 12333 = 升级技能花费
    MONEY_LOG_LOSE_TAVERN = 12334,                  //$ 12334 = 酒馆花费
    MONEY_LOG_LOSE_BLACKMARKET = 12335,             //$ 12335 = 黑市花费
    MONEY_LOG_LOSE_TREASURE = 12336,                //$ 12336 = 宝藏花费

    MONEY_LOG_LOSE_MAX = 12599,                     // $ 12599 = 失去游戏金钱日志类型最大值

    MONEY_LOG_MAX = 12999,                          // $ 12999 = 游戏金钱异动日志类型最大值
};

// 金钱异动状态
enum MONEY_ACT_STATE_TYPE
{
    MONEY_ACT_STATE_FAIL = 0,       // 金钱异动状态为失败
    MONEY_ACT_STATE_SUCCESS = 1,    // 金钱异动状态为成功
};


// $@ 4 = 物品异动日志类型
enum ITEM_LOG_TYPE
{
    ITEM_LOG_MIN = 13000,                       // $ 13000 = 物品异动日志类型最小值

    ITEM_LOG_OUTPUT_MIN = 13001,                // $ 13001 = 物品产出日志类型最小值
    ITEM_LOG_OUTPUT_PICK_UP = 13002,            // $ 13002 = 通过拾取获得物品
    ITEM_LOG_OUTPUT_COMPOSE_GEM = 13003,        // $ 13003 = 通过合成生成宝石
    ITEM_LOG_OUTPUT_SPLIT = 13004,              // $ 13004 = 通过拆分获得物品
    ITEM_LOG_OUTPUT_MELT = 13005,               // $ 13005 = 通过熔炼获得物品
    ITEM_LOG_OUTPUT_TASK = 13006,               // $ 13006 = 通过任务获得物品
    ITEM_LOG_OUTPUT_COMPOSE_EQUIP = 13007,      // $ 13007 = 通过合成生成装备
    ITEM_LOG_OUTPUT_COLLECT = 13008,            // $ 13008 = 通过采集获得物品
	ITEM_LOG_OUTPUT_SHOP = 13009,				// $ 13009 = 通过商城获得物品
	ITEM_LOG_OUTPUT_ARENA = 13010,				// $ 13010 = 竞技场声望兑换商店
	ITEM_LOG_OUTPUT_GIFT_PACKAGE = 13011,		// $ 13011 = 打开礼包产出物品
	ITEM_LOG_OUTPUT_PATCH_COMPOSE = 13012,		// $ 13012 = 碎片合成产出物品
	ITEM_LOG_OUTPUT_BUY_VIP_GIFT = 13013,		// $ 13013 = 购买vip礼包
	ITEM_LOG_OUTPUT_FIRST_PAY_GIFT = 13014,		// $ 13014 = 首充礼包
	ITEM_LOG_OUTPUT_TALENT_REWARD = 13015,		// $ 13015 = 天赋系统奖励物品
	ITEM_LOG_OUTPUT_SYSTEM_MAIL = 13016,		// $ 13016 = 系统邮件给予物品
	ITEM_LOG_OUTPUT_PHY_EVENT = 13017,			// $ 13017 = 体力事件奖励物品(吃大餐)
	ITEM_LOG_OUTPUT_GM_ADD = 13018,             // $ 13018 = 通过GM命令添加的物品
	ITEM_LOG_OUTPUT_TREASURE = 13019,           // $ 13019 = 通过开宝藏产出
	ITEM_LOG_OUTPUT_SPORTS =  13020,            // $ 13020 = 通过夺宝产出
	ITEM_LOG_OUTPUT_GIFT = 13021,               // $ 13021 = 通过领取礼包产出
	ITEM_LOG_OUTPUT_AWARD = 13022,              // $ 13022 = 通过奖励获取
	ITEM_LOG_OUTPUT_BLACKMARKET = 13023,        // $ 13023 = 通过黑市产出
    ITEM_LOG_OUTPUT_BORN = 13024,               // $ 13024 = 创建角色时发的物品
    ITEM_LOG_OUTPUT_TAVERN = 13025,             // $ 13024 = 通过酒馆产出
	ITEM_LOG_OUTPUT_CLONE_AWARD = 13026,        // $ 13025 = 通关副本获取的物品奖励
    ITEM_LOG_OUTPUT_MAX = 13299,                // $ 13299 = 物品产出日志类型最大值

    ITEM_LOG_CONSUME_MIN = 13300,               // $ 13300 = 物品消耗日志类型最小值
    ITEM_LOG_CONSUME_RECYCLE = 13301,           // $ 13301 = 物品被系统回收
    ITEM_LOG_CONSUME_COMPOSE_GEM = 13302,       // $ 13302 = 合成宝石消耗物品
    ITEM_LOG_CONSUME_USE_SKILL = 13303,         // $ 13303 = 使用技能消耗物品
    ITEM_LOG_CONSUME_APPEND_MATERIAL = 13304,   // $ 13304 = 消耗附魔材料
    ITEM_LOG_CONSUME_APPEND_PAPER = 13305,      // $ 13305 = 消耗附魔图纸
    ITEM_LOG_CONSUME_EVALUATE = 13306,          // $ 13306 = 鉴定消耗鉴定书
    ITEM_LOG_CONSUME_ENHANCE = 13307,           // $ 13307 = 镶嵌消耗宝石
    ITEM_LOG_CONSUME_SPLIT_GEM = 13308,         // $ 13308 = 拆卸宝石消耗物品
    ITEM_LOG_CONSUME_SLOT = 13309,              // $ 13309 = 打孔消耗物品
    ITEM_LOG_CONSUME_MELT = 13310,              // $ 13310 = 熔炼装备消耗
    ITEM_LOG_CONSUME_MELT_MATERIAL = 13311,     // $ 13311 = 熔炼材料消耗
	ITEM_LOG_CONSUME_DISCARD_FORM_BAG = 13312,	// $ 13312 = 从背包丢弃物品
	ITEM_LOG_CONSUME_COMPOSE_FORM_BAG = 13313,	// $ 13313 = 合成装备消耗物品
	ITEM_LOG_CONSUME_COMPOSE_EQUIP_INITIAL = 13314,	// $ 13314 = 合成装备时初始装备消耗
	ITEM_LOG_CONSUME_GIFT_PACKAGE = 13315,		// $ 13315 = 打开礼包消耗
	ITEM_LOG_CONSUME_PATCH_COMPOSE = 13316,		// $ 13316 = 碎片合成消耗
	ITEM_LOG_CONSUME_CAPITAL_ITEM = 13317,		// $ 13317 = 资产道具消耗
	ITEM_LOG_OUTPUT_TALENT_CONSUME = 13318,		// $ 13015 = 天赋系统消耗物品
    ITEM_LOG_OUTPUT_TAVERN_CONSUME = 13319,     // $ 13319 = 酒馆系统消耗物品
	ITEM_LOG_CONSUME_ADDPROP_ITEM = 13320,     // $ 13320 = 加属性道具消耗
    ITEM_LOG_CONSUME_MAX = 13599,               // $ 13599 = 物品消耗日志类型最大值

    ITEM_LOG_MAX = 13999,                       // $ 13999 = 物品异动日志类型最大值
};

// 物品异动状态
enum ITEM_ACT_STATE_TYPE
{
    ITEM_ACT_STATE_FAIL = 0,        // 物品异动状态为失败
    ITEM_ACT_STATE_SUCCESS = 1,     // 物品异动状态为成功
};


// $@ 5 = 交易日志类型
enum DEAL_LOG_TYPE
{
    DEAL_LOG_MIN = 14000,           // $ 14000 = 交易日志类型最小值
    DEAL_LOG_EXCHANGE = 14001,      // $ 14001 = 直接交易
    DEAL_LOG_MAIL = 14002,          // $ 14002 = 玩家邮件
    DEAL_LOG_MAX = 14999,           // $ 14999 = 交易日志类型最大值
};


// $@ 6 = 玩家升级日志类型
enum ROLE_UPGRADE_LOG_TYPE
{
    ROLE_UPGRADE_LOG_MIN = 15000,       // $ 15000 = 玩家升级日志类型最小值
    ROLE_UPGRADE_LOG_PLAYER = 15001,    // $ 15001 = 玩家升级
    ROLE_UPGRADE_LOG_SKILL = 15002,     // $ 15002 = 技能升级
    ROLE_UPGRADE_LOG_MAX = 15999,       // $ 15999 = 玩家升级日志类型最大值
};

// $@ 7 = 惩罚日志类型
enum PUNISH_LOG_TYPE
{
    PUNISH_LOG_MIN = 16000,     // $ 16000 = 惩罚日志类型最小值
    PUNISH_LOG_MAX = 16999,     // $ 16000 = 惩罚日志类型最大值
};

// 玩家相关日志类型
enum PLAYER_LOG_TYPE
{
    PLAYER_LOG_MIN = 17000,					// 玩家相关日志类型最小值

    PLAYER_LOG_CHAT = 17001,				// 聊天脏字日志
    PLAYER_LOG_CAPITAL = 17002,				// 资金变化
    PLAYER_LOG_EXCHANGE_ITEMS = 17003,		// 玩家交易物品
    PLAYER_LOG_EXCHANGE_MONEY = 17004,		// 玩家交易钱
    PLAYER_LOG_POST = 17005,				// 邮件相关
    PLAYER_LOG_FRIEND = 17006,				// 好友相关
    PLAYER_LOG_SKILL = 17007,				// 技能相关
	PLAYER_LOG_SEND_DEC_POINT = 17008,		// 玩家发送计费扣除点券的消息
	PLAYER_LOG_RECEIVE_DEC_POINT = 17009,	// 玩家接受计费扣除点券的消息

    PLAYER_LOG_MAX = 17999,					// 玩家相关日志类型最大值
};

// 特殊资金日志类型, 仅仅是日志
//enum CAPITAL_SPECIAL_LOG_TYPE
//{
//	CLS_MAX_CAPITAL                 = 37001,    //$ 37001 = 资金超出最大值 没有被加上的部分
//	CLS_DEC_POINT                   = 37002,    //$ 37002 = 游戏中消耗点数
//	CLS_ADD_POINT                   = 37003,    //$ 37003 = 三测抽奖获得点数
//	CLS_DECPOINT_ARCHIVE            = 37004,    //$ 37004 = 购买自己存档消耗点数
//	CLS_DECPOINT_TIME_AUC           = 37005,    //$ 37005 = 购买拍卖行时之沙消耗点数
//};

#define  STR_TIME_MAX_LENGTH  64
#define  STR_LOG_COMMON_LENGTH  128
#define  STR_LOG_MAX_LENGTH     256
#define  STR_LOG_COMMENT_LENGTH  128
// 玩家相关日志描述

#endif  // __LogDefine_h__