#ifndef __LogDefine_h__
#define __LogDefine_h__

// Log�ȼ�
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
    LOG_TRACE = 1,  /**< ������Լ�����־ */
    LOG_INFO,       /**< ϵͳ������־ */
    LOG_WARNING,    /**< ������־ */
    LOG_ERROR,      /**< ������־ */
    LOG_CRITICAL    /**< ���ش���(������������) */
};


// $@ 1 = ������־����
enum TASK_LOG_TYPE
{
    TASK_LOG_MIN        = 10000,    // $ 10000 = ������־������Сֵ
    TASK_LOG_ACCEPTED   = 10001,    // $ 10001 = ��������
    TASK_LOG_COMPLETED  = 10002,    // $ 10002 = �������
    TASK_LOG_CANCEL     = 10003,    // $ 10003 = ��������
    TASK_LOG_FAILED     = 10004,    // $ 10004 = ����ʧ��
    TASK_LOG_MAX        = 10999,    // $ 10999 = ������־�������ֵ
};

// $@ 2 = ��Ϸ�淨��־����
enum GAME_ACTION_LOG_TYPE
{
    GAME_ACTION_LOG_MIN = 11000,            // $ 11000 = ��Ϸ�淨��־������Сֵ
    GAME_ACTION_LOG_START = 11001,          // $ 11001 = �淨��ʼ
    GAME_ACTION_LOG_FINISH = 11002,         // $ 11002 = �淨���
    GAME_ACTION_LOG_UNFINISHED = 11003,     // $ 11003 = �淨δ���
    GAME_ACTION_LOG_MAX = 11999,            // $ 11999 = ��Ϸ�淨��־�������ֵ
};

// $@ 3 = ��Ϸ��Ǯ�춯��־����
enum MONEY_LOG_TYPE
{
    MONEY_LOG_MIN = 12000,                          // $ 12000 = ��Ϸ��Ǯ�춯��־������Сֵ

    MONEY_LOG_GAIN_MIN = 12001,                     // $ 12001 = �����Ϸ��Ǯ��־������Сֵ
    MONEY_LOG_GAIN_TASK = 12002,                    //$ 12002 = ������������ӽ�Ǯ
    MONEY_LOG_GAIN_EXCHANGE = 12003,                //$ 12003 = ��һ��ཻ�����ӽ�Ǯ
    MONEY_LOG_GAIN_PICKUP = 12004,                  //$ 12004 = ������ɱ�ֵ����Ǯ
    MONEY_LOG_GAIN_CLONE = 12005,                   //$ 12005 = ͨ�ظ���������Ǯ
    MONEY_LOG_GAIN_GM = 12006,                      //$ 12006 = ͨ��GM��lua���ӽ�Ǯ
    MONEY_LOG_GAIN_DEPOT = 12007,                   //$ 12007 = �ֿ�ȡǮ
    MONEY_LOG_GAIN_SELL_ITEM = 12008,               //$ 12008 = ��NPC������Ʒ��ȡ��Ǯ
    MONEY_LOG_GAIN_POST = 12009,                    //$ 12009 = ��ȡ�ʼ��еĽ�Ǯ
    MONEY_LOG_GAIN_FIGHT_REWARD = 12010,            //$ 12010 = ��������������
    MONEY_LOG_GAIN_ITEM_GLORY = 12011,              //$ 12011 = �����һ���Ʒʧ�ܷ���������
    MONEY_LOG_GAIN_BS = 12012,                      //$ 12012 = ��Ӫƽ̨�������ӽ�Ǯ
    MONEY_LOG_GAIN_INVEST = 12013,                  //$ 12013 = Ͷ�ʻر������Ǯ
    MONEY_LOG_GAIN_SPORTS = 12014,                  //$ 12014 = ����ʤ����ý�����Ǯ
    MONEY_LOG_GAIN_TALENT = 12015,                  //$ 12015 = ������λ��ý���
    MONEY_LOG_GAIN_TOOLITEM = 12016,                //$ 12016 = ʹ����Ʒ��ý�Ǯ
    MONEY_LOG_GAIN_CLONE_REWARD = 12017,            //$ 12017 = �������佱����Ǯ
    MONEY_LOG_GAIN_SWEEP_CLONE = 12018,             //$ 12018 = ɨ��������ý�Ǯ
    MONEY_LOG_GAIN_FURANCE = 12019,                 //$ 12019 = ����/������ý�Ǯ
    MONEY_LOG_GAIN_BUY_FAIL = 12020,                //$ 12020 = ������Ʒʧ���˻ؽ�Ǯ
    MONEY_LOG_GAIN_PAY = 12021,                     //$ 12021 = ��ֵ��ý�Ǯ
	MONEY_LOG_GAIN_BUY_ITEM = 12022,                //$ 12022 = �����Ǯ����߻�ý�Ǯ
	MONEY_LOG_GAIN_GIFT_ITEM = 12023,               //$ 12023 = ���������õĽ�Ǯ
    MONEY_LOG_GAIN_MAX = 12299,                     // $ 12299 = �����Ϸ��Ǯ��־�������ֵ

    MONEY_LOG_LOSE_MIN = 12300,                     // $ 12300 = ʧȥ��Ϸ��Ǯ��־������Сֵ
    MONEY_LOG_LOSE_GM = 12301,                      //$ 12301 = ͨ��GM���ٽ�Ǯ
    MONEY_LOG_LOSE_DROP=12302,                      //$ 12302 = ��ұ�����ɱ������
    MONEY_LOG_LOSE_COMPOSE=12303,                   //$ 12303 = ��ʯ�ϳ� 
    MONEY_LOG_LOSE_COMPOUND=12304,                  //$ 12304 = ��Ʒ�ϳ�
    MONEY_LOG_LOSE_STRENGTHEN=12305,                //$ 12305 = ��Ʒǿ��
    MONEY_LOG_LOSE_APPEND=12306,                    //$ 12306 = ��Ʒ��ħ
    MONEY_LOG_LOSE_ENHANCE=12307,                   //$ 12307 = ��ʯ��Ƕ
    MONEY_LOG_LOSE_SLOTITEM=12308,                  //$ 12308 = ��Ʒ���
    MONEY_LOG_LOSE_TRANSPORT=12309,                 //$ 12309 = ��ͨ����
    MONEY_LOG_LOSE_UNLOCK_BAG=12310,                //$ 12310 = ��Ʒ��������
    MONEY_LOG_LOSE_DEPOT=12311,                     //$ 12311 = ���д�Ǯ
    MONEY_LOG_LOSE_UNLOCK_ROLE=12312,               //$ 12312 = ��ɫ����   
    MONEY_LOG_LOSE_CLONE_ENTER=12313,               //$ 12313 = ���븱������
    MONEY_LOG_LOSE_PK=12314,                        //$ 12314 = PK��������  
    MONEY_LOG_LOSE_SHOP=12315,                      //$ 12315 = �̵깺����Ʒ
    MONEY_LOG_LOSE_LEARN_SKILL=12316,               //$ 12316 = ѧϰ��������
    MONEY_LOG_LOSE_AUCTION=12317,                   //$ 12317 = �����й�����Ʒ
    MONEY_LOG_LOSE_RRCLAIM=12318,                   //$ 12318 = �ع���Ʒ
    MONEY_LOG_LOSE_EXCHANGE=12319,                  //$ 12319 = ��Ҽ佻��  
    MONEY_LOG_LOSE_CREATE_GUILD=12320,              //$ 12320 = ��������
    MONEY_LOG_LOSE_GUILD_DONATE=12321,              //$ 12321 = ���ṱ��
    MONEY_LOG_LOSE_POST_REMIT = 12322,              //$ 12322 = �����ʼ�
    MONEY_LOG_LOSE_POST_SEND = 12323,               //$ 12323 = �����ʼ�����
	MONEY_LOG_LOSE_MALL_ITEM = 12324,               //$ 12324 = �̳ǹ�����߸���
	MONEY_LOG_LOSE_MALL_EXCHANGE = 12325,           //$ 12325 = �̳Ƕһ���Ҹ���
	MONEY_LOG_LOSE_MALL_VIP_ITEM = 12326,			//$ 12326 = �̳ǹ���vip���߸���
    MONEY_LOG_LOSE_ITEM_GLORY = 12327,              //$ 12327 = �һ���Ʒ��������
    MONEY_LOG_LOSE_BUY_PEACE = 12328,               //$ 12328 = �����ƽģʽ����
    MONEY_LOG_LOSE_BUY_INVEST = 12329,              //$ 12329 = ����Ͷ��Ʒ����
    MONEY_LOG_LOSE_TALENT = 12330,                  //$ 12330 = ������λ����
	MONEY_LOG_LOSE_RESET_CLONE_SAODANG = 12331,		//$ 12331 = ����ɨ��ʱ�����ս��������
    MONEY_LOG_LOSE_SKILL = 12332,		            //$ 12332 = ��������
    MONEY_LOG_LOSE_UPDATE_SKILL = 12333,            //$ 12333 = �������ܻ���
    MONEY_LOG_LOSE_TAVERN = 12334,                  //$ 12334 = �ƹݻ���
    MONEY_LOG_LOSE_BLACKMARKET = 12335,             //$ 12335 = ���л���
    MONEY_LOG_LOSE_TREASURE = 12336,                //$ 12336 = ���ػ���

    MONEY_LOG_LOSE_MAX = 12599,                     // $ 12599 = ʧȥ��Ϸ��Ǯ��־�������ֵ

    MONEY_LOG_MAX = 12999,                          // $ 12999 = ��Ϸ��Ǯ�춯��־�������ֵ
};

// ��Ǯ�춯״̬
enum MONEY_ACT_STATE_TYPE
{
    MONEY_ACT_STATE_FAIL = 0,       // ��Ǯ�춯״̬Ϊʧ��
    MONEY_ACT_STATE_SUCCESS = 1,    // ��Ǯ�춯״̬Ϊ�ɹ�
};


// $@ 4 = ��Ʒ�춯��־����
enum ITEM_LOG_TYPE
{
    ITEM_LOG_MIN = 13000,                       // $ 13000 = ��Ʒ�춯��־������Сֵ

    ITEM_LOG_OUTPUT_MIN = 13001,                // $ 13001 = ��Ʒ������־������Сֵ
    ITEM_LOG_OUTPUT_PICK_UP = 13002,            // $ 13002 = ͨ��ʰȡ�����Ʒ
    ITEM_LOG_OUTPUT_COMPOSE_GEM = 13003,        // $ 13003 = ͨ���ϳ����ɱ�ʯ
    ITEM_LOG_OUTPUT_SPLIT = 13004,              // $ 13004 = ͨ����ֻ����Ʒ
    ITEM_LOG_OUTPUT_MELT = 13005,               // $ 13005 = ͨ�����������Ʒ
    ITEM_LOG_OUTPUT_TASK = 13006,               // $ 13006 = ͨ����������Ʒ
    ITEM_LOG_OUTPUT_COMPOSE_EQUIP = 13007,      // $ 13007 = ͨ���ϳ�����װ��
    ITEM_LOG_OUTPUT_COLLECT = 13008,            // $ 13008 = ͨ���ɼ������Ʒ
	ITEM_LOG_OUTPUT_SHOP = 13009,				// $ 13009 = ͨ���̳ǻ����Ʒ
	ITEM_LOG_OUTPUT_ARENA = 13010,				// $ 13010 = �����������һ��̵�
	ITEM_LOG_OUTPUT_GIFT_PACKAGE = 13011,		// $ 13011 = �����������Ʒ
	ITEM_LOG_OUTPUT_PATCH_COMPOSE = 13012,		// $ 13012 = ��Ƭ�ϳɲ�����Ʒ
	ITEM_LOG_OUTPUT_BUY_VIP_GIFT = 13013,		// $ 13013 = ����vip���
	ITEM_LOG_OUTPUT_FIRST_PAY_GIFT = 13014,		// $ 13014 = �׳����
	ITEM_LOG_OUTPUT_TALENT_REWARD = 13015,		// $ 13015 = �츳ϵͳ������Ʒ
	ITEM_LOG_OUTPUT_SYSTEM_MAIL = 13016,		// $ 13016 = ϵͳ�ʼ�������Ʒ
	ITEM_LOG_OUTPUT_PHY_EVENT = 13017,			// $ 13017 = �����¼�������Ʒ(�Դ��)
	ITEM_LOG_OUTPUT_GM_ADD = 13018,             // $ 13018 = ͨ��GM������ӵ���Ʒ
	ITEM_LOG_OUTPUT_TREASURE = 13019,           // $ 13019 = ͨ�������ز���
	ITEM_LOG_OUTPUT_SPORTS =  13020,            // $ 13020 = ͨ���ᱦ����
	ITEM_LOG_OUTPUT_GIFT = 13021,               // $ 13021 = ͨ����ȡ�������
	ITEM_LOG_OUTPUT_AWARD = 13022,              // $ 13022 = ͨ��������ȡ
	ITEM_LOG_OUTPUT_BLACKMARKET = 13023,        // $ 13023 = ͨ�����в���
    ITEM_LOG_OUTPUT_BORN = 13024,               // $ 13024 = ������ɫʱ������Ʒ
    ITEM_LOG_OUTPUT_TAVERN = 13025,             // $ 13024 = ͨ���ƹݲ���
	ITEM_LOG_OUTPUT_CLONE_AWARD = 13026,        // $ 13025 = ͨ�ظ�����ȡ����Ʒ����
    ITEM_LOG_OUTPUT_MAX = 13299,                // $ 13299 = ��Ʒ������־�������ֵ

    ITEM_LOG_CONSUME_MIN = 13300,               // $ 13300 = ��Ʒ������־������Сֵ
    ITEM_LOG_CONSUME_RECYCLE = 13301,           // $ 13301 = ��Ʒ��ϵͳ����
    ITEM_LOG_CONSUME_COMPOSE_GEM = 13302,       // $ 13302 = �ϳɱ�ʯ������Ʒ
    ITEM_LOG_CONSUME_USE_SKILL = 13303,         // $ 13303 = ʹ�ü���������Ʒ
    ITEM_LOG_CONSUME_APPEND_MATERIAL = 13304,   // $ 13304 = ���ĸ�ħ����
    ITEM_LOG_CONSUME_APPEND_PAPER = 13305,      // $ 13305 = ���ĸ�ħͼֽ
    ITEM_LOG_CONSUME_EVALUATE = 13306,          // $ 13306 = �������ļ�����
    ITEM_LOG_CONSUME_ENHANCE = 13307,           // $ 13307 = ��Ƕ���ı�ʯ
    ITEM_LOG_CONSUME_SPLIT_GEM = 13308,         // $ 13308 = ��ж��ʯ������Ʒ
    ITEM_LOG_CONSUME_SLOT = 13309,              // $ 13309 = ���������Ʒ
    ITEM_LOG_CONSUME_MELT = 13310,              // $ 13310 = ����װ������
    ITEM_LOG_CONSUME_MELT_MATERIAL = 13311,     // $ 13311 = ������������
	ITEM_LOG_CONSUME_DISCARD_FORM_BAG = 13312,	// $ 13312 = �ӱ���������Ʒ
	ITEM_LOG_CONSUME_COMPOSE_FORM_BAG = 13313,	// $ 13313 = �ϳ�װ��������Ʒ
	ITEM_LOG_CONSUME_COMPOSE_EQUIP_INITIAL = 13314,	// $ 13314 = �ϳ�װ��ʱ��ʼװ������
	ITEM_LOG_CONSUME_GIFT_PACKAGE = 13315,		// $ 13315 = ���������
	ITEM_LOG_CONSUME_PATCH_COMPOSE = 13316,		// $ 13316 = ��Ƭ�ϳ�����
	ITEM_LOG_CONSUME_CAPITAL_ITEM = 13317,		// $ 13317 = �ʲ���������
	ITEM_LOG_OUTPUT_TALENT_CONSUME = 13318,		// $ 13015 = �츳ϵͳ������Ʒ
    ITEM_LOG_OUTPUT_TAVERN_CONSUME = 13319,     // $ 13319 = �ƹ�ϵͳ������Ʒ
	ITEM_LOG_CONSUME_ADDPROP_ITEM = 13320,     // $ 13320 = �����Ե�������
    ITEM_LOG_CONSUME_MAX = 13599,               // $ 13599 = ��Ʒ������־�������ֵ

    ITEM_LOG_MAX = 13999,                       // $ 13999 = ��Ʒ�춯��־�������ֵ
};

// ��Ʒ�춯״̬
enum ITEM_ACT_STATE_TYPE
{
    ITEM_ACT_STATE_FAIL = 0,        // ��Ʒ�춯״̬Ϊʧ��
    ITEM_ACT_STATE_SUCCESS = 1,     // ��Ʒ�춯״̬Ϊ�ɹ�
};


// $@ 5 = ������־����
enum DEAL_LOG_TYPE
{
    DEAL_LOG_MIN = 14000,           // $ 14000 = ������־������Сֵ
    DEAL_LOG_EXCHANGE = 14001,      // $ 14001 = ֱ�ӽ���
    DEAL_LOG_MAIL = 14002,          // $ 14002 = ����ʼ�
    DEAL_LOG_MAX = 14999,           // $ 14999 = ������־�������ֵ
};


// $@ 6 = ���������־����
enum ROLE_UPGRADE_LOG_TYPE
{
    ROLE_UPGRADE_LOG_MIN = 15000,       // $ 15000 = ���������־������Сֵ
    ROLE_UPGRADE_LOG_PLAYER = 15001,    // $ 15001 = �������
    ROLE_UPGRADE_LOG_SKILL = 15002,     // $ 15002 = ��������
    ROLE_UPGRADE_LOG_MAX = 15999,       // $ 15999 = ���������־�������ֵ
};

// $@ 7 = �ͷ���־����
enum PUNISH_LOG_TYPE
{
    PUNISH_LOG_MIN = 16000,     // $ 16000 = �ͷ���־������Сֵ
    PUNISH_LOG_MAX = 16999,     // $ 16000 = �ͷ���־�������ֵ
};

// ��������־����
enum PLAYER_LOG_TYPE
{
    PLAYER_LOG_MIN = 17000,					// ��������־������Сֵ

    PLAYER_LOG_CHAT = 17001,				// ����������־
    PLAYER_LOG_CAPITAL = 17002,				// �ʽ�仯
    PLAYER_LOG_EXCHANGE_ITEMS = 17003,		// ��ҽ�����Ʒ
    PLAYER_LOG_EXCHANGE_MONEY = 17004,		// ��ҽ���Ǯ
    PLAYER_LOG_POST = 17005,				// �ʼ����
    PLAYER_LOG_FRIEND = 17006,				// �������
    PLAYER_LOG_SKILL = 17007,				// �������
	PLAYER_LOG_SEND_DEC_POINT = 17008,		// ��ҷ��ͼƷѿ۳���ȯ����Ϣ
	PLAYER_LOG_RECEIVE_DEC_POINT = 17009,	// ��ҽ��ܼƷѿ۳���ȯ����Ϣ

    PLAYER_LOG_MAX = 17999,					// ��������־�������ֵ
};

// �����ʽ���־����, ��������־
//enum CAPITAL_SPECIAL_LOG_TYPE
//{
//	CLS_MAX_CAPITAL                 = 37001,    //$ 37001 = �ʽ𳬳����ֵ û�б����ϵĲ���
//	CLS_DEC_POINT                   = 37002,    //$ 37002 = ��Ϸ�����ĵ���
//	CLS_ADD_POINT                   = 37003,    //$ 37003 = ����齱��õ���
//	CLS_DECPOINT_ARCHIVE            = 37004,    //$ 37004 = �����Լ��浵���ĵ���
//	CLS_DECPOINT_TIME_AUC           = 37005,    //$ 37005 = ����������ʱ֮ɳ���ĵ���
//};

#define  STR_TIME_MAX_LENGTH  64
#define  STR_LOG_COMMON_LENGTH  128
#define  STR_LOG_MAX_LENGTH     256
#define  STR_LOG_COMMENT_LENGTH  128
// ��������־����

#endif  // __LogDefine_h__