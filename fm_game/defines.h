#pragma once

enum NORMAL_ATTACK_STAGE
{
    NORMAL_ATTACK_STAGE_NONE = 0,
    NORMAL_ATTACK_STAGE_BEGIN,		// ��ʼ��ͨ������ʹ���ߣ�Ŀ������
    NORMAL_ATTACK_STAGE_EFFECT,		// ���Ч��������
    NORMAL_ATTACK_STAGE_BREAK,		// ��ϣ�ԭ��
    NORMAL_ATTACK_STAGE_FINISH,		// ������ԭ��
    NORMAL_ATTACK_STAGE_MAX,		
};