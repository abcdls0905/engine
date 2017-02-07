
#ifndef _OBJ_STATE_DEFINE_H
#define _OBJ_STATE_DEFINE_H


// ״̬���������Ͷ���
enum PlayerInput
{
    PLAYER_INPUT_NULL = 0,     //��Ч����

	PLAYER_INPUT_ENTRY,        // ����״̬
	PLAYER_INPUT_LEAVE,        // �˳�״̬
	PLAYER_INPUT_TIMER,        // ��ʱ��ˢ��
	PLAYER_INPUT_LOCATION,     // ������ǿ�ƶ�λ
    PLAYER_INPUT_BEGIN_MOVE,   // ��ʼ�ƶ�(ҡ��Ѱ·)
	PLAYER_INPUT_BEGIN_MOVE_TO,// ��ʼ�ƶ���Ŀ���λ��
	PLAYER_INPUT_END_MOVE,     // �����ƶ�
	PLAYER_INPUT_BE_MOVING,    // ���������ƶ����ƶ�
	PLAYER_INPUT_LOGIC,        // �������߼�����������

	PLAYER_INPUT_MAX,
};

// PLAYER_INPUT_LOGIC��������
enum LogicIndex
{
    LOGIC_UNKOWN = 0,
    LOGIC_DIE  = 9,
    LOGIC_RELIVE  = 10,
    LOGIC_LOCK  = 11,
    LOGIC_UNLOCK  = 12,
    LOGIC_END_TRACE  = 32,
    LOGIC_START_MOVE = 33,
    LOGIC_MAX,
};

// ״̬���ֶ�Ӧ��״̬����ֵ�Ķ���
enum ObjStateIndex
{
    STATE_INDEX_NULL  =  0,     //��Ч״̬

	STATE_STATIC_INDEX,         //����Ҿ�ֹ
    STATE_MOTION_INDEX,         //�����ң���ƶ�
	STATE_PATH_FINDING_INDEX,   //�����Ѱ·�ƶ�
	STATE_BE_STOP_INDEX,        //������Ҿ�ֹ
	STATE_BE_MOTION_INDEX,      //��������ƶ�
	STATE_LOCKED_INDEX,         //����
	STATE_DEAD_INDEX,           //����

	STATE_INDEX_MAX,
};

#endif 
