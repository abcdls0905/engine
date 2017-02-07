//----------------------------------------------------------
// �ļ���:		CustomPropDefine
// ��  ��:		�ͻ����Զ������Զ���
// ˵  ��:		�Զ�������̫���ˣ���������ͳһ���岢��ϸ˵������鿴
//              ********Ϊ�˱������ֿͻ����Զ�������ȫ����Сд******
// ������:		chenhh
// ��������:    2013/06/18
// ��Ȩ����:    ������ţ�������޹�˾
//----------------------------------------------------------

#ifndef CUSTOM_PROP_DEFINE_H_
#define CUSTOM_PROP_DEFINE_H_


#define  CUSTOM_PROP_STRING_STATE         "state"       //ģ��״̬
#define  CUSTOM_PROP_INT_STATE_INDEX      "state_index"  //ģ��״̬����

#define  CUSTOM_PROP_OBJECT_TERRAIN      "terrain"   //�����ĵ���
#define  CUSTOM_PROP_OBJECT_SCENE        "scene"   //���������ĳ���
#define  CUSTOM_PROP_HIDE_NPC_BAR        "hide_npc_bar" //�Ƿ�����NPCͷ��Ѫ��

#define  CUSTOM_PROP_STRING_CLIENT_IDENT   "client_ident"   //����Ŀͻ��������ʾ
#define  CUSTOM_PROP_NPC_SCRIPT                          "npc_script"  //�ͻ��˶��� ����npcscript
/*#define  CUSTOM_PROP_OBJECT_SERVE_RDEAD		"ServerDead" // */
#define  CUSTOM_PROP_OBJECT_CREATE_FINISH   "createfinish" // �Ƿ񴴽���� 

//�����Ƿ��Ѿ����õ�����
#define  CUSTOM_PROP_BOOL_MAIN_ROLE_ADD_TO_TERRAIN  "main_role_addto_terrain"

#define  CUSTOM_PROP_LAST_OBJECT  "lastobject"
//------------------------------< Action >----------------------------------------
#define  ACTION_ACTION_SET					"action_set"
#define  ACTION_OLD							"action_old"
#define  ACTION_DIEDOWN						"diedown"

//------------------------------< Skill >----------------------------------------
#define CUSTOM_PROP_SKILL_NO_CONSUME					"skill_no_consume"  //����������
#define CUSTOM_PROP_SKILL_ATTACK_SPEED_RATIO			"attack_speed_ratio"  //���ܹ����ٶȱ���
#define CUSTOM_PROP_SKILL_TARGET_COUNT					"target_count"		//Ŀ����Ŀ
#define CUSTOM_PROP_SKILL_CAN_PAUSE						"can_pause"		//ͣ��
#define CUSTOM_PROP_EFFECT_PLAY_HURT_EFFECT				"play_hurt_effect"	//�����������������Ч
#define CUSTOM_PROP_EFFECT_PLAY_HURTER					"play_hurter"		//�����ܻ���������
#define CUSTOM_PROP_SKILL_HURT_PLAYER					"hurt_player"		// ���˵Ĺ�����
#define CUSTOM_PROP_SKILL_DAMAGE_TYPE					"damage_type"		// �˺�����
#define CUSTOM_PROP_CANTCOMBATACTION					"cant_combat_action"	// �Ƿ񲻲����ܻ�����
#define CUSTOM_PROP_SKILL_DAMAGE_TYPE					"damage_type"		// �˺�����
//------------------------------< State >----------------------------------------
#define STATE_MOVE_ANGLE                    "move_angle"
#define STATE_MOVE_DIST                     "move_distance"
#define STATE_FACE_ANGLE                    "face_angle"
#define STATE_LOCK_MOVE                     "lock_move"
#define STATE_LOCK_FACE                     "lock_face"
#define STATE_LOCK_SKILL                    "lock_skill"
#define STATE_CANT_USENORMAL        "notusenormal"
#define STATE_JOYSTICK_ANGLE                "joystick_angle"



#endif