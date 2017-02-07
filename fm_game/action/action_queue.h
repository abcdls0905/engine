
#pragma once

#include <vector>
#include "../objbinder/i_bind_entity.h"
#include "../public/i_entity.h"
#include "../visual/i_vis_base.h"
#include <string>

// �������
enum ACTION_CLEAR_TYPE
{
	// �����
	eClearNone,
	// ���ȫ����
	eClearState,
	// ���������
	eClearHalf,
	// ���һ���Զ���
	eClearOnce,
	// ���һ���Զ��������Ծܾ�����
	eClearOnceIgnore,
	// ���ȫ��
	eClearAll,
};

// �����Ƿ�ɱ����
enum ACTION_INTERRUPT_TYPE
{
	// �ɱ����:��Ϊ������ʱ,�����һ����ҲΪ������,���ϵ�ǰ������,����һ��������
	//      ��Ϊȫ����ʱ,�����һ����Ϊ�ǰ�����,���ϵ�ǰ������,����һ��ȫ����
	eInterrupt,

	// ���������仯ʱ�����(��ֹ��ر���ǰ��ص�)
	eChangeInterrupt,

    // ֻ��eClearAll�Ķ������
    eNoInterrupt,

	// �������,�Ҿܾ�������������,����eClearAll���ȼ�
	eRefuseAction,
};

//��ǰ���������Ƿ�ɼ�����������
enum ACTION_REFUSE_TYPE
{
	eRefuseNone,
	eRefuseAll,
};

//�����еĶ���״̬
enum ActionState
{
	ACTION_STATE_WAIT,
	ACTION_STATE_PLAY,
};

enum PLAYER_ACTION_TYPE
{
	ePlayAll,
	ePlayNormal,
	ePlayHalfAction,
};

//������Ϣ
struct ActionInfo 
{
	char ActionName[256];               //������
	char FullActionName[256];	        //ȫ������
	unsigned char loop;					//�Ƿ�ѭ��
	unsigned char state;				//������ǰ״̬
	ACTION_CLEAR_TYPE clear;	        //�Ƿ����֮ǰ�������ں�
	ACTION_INTERRUPT_TYPE bInterrupt;   //�Ƿ�ɴ�ϣ�������һ���Զ�����
};

const int QUEUE_COUNT_MAX = 50; //�������з�ֵ

class CActionQueue : public IBindEntity
{
public:
  CActionQueue();
  ~CActionQueue();
  virtual void SetRole(const PERSISTID & id){ m_visID = id; }
  virtual bool Init(const IVarList& args);
  virtual bool Shut();
private:
  std::vector<ActionInfo*>	m_vecActionQueue;      // ��������
  std::vector<ActionInfo*>	m_vecFreeActionQueue;      // ���ж�������
  std::string         m_strLoopActionName;           // ѭ����������
  bool                m_bPlayLoopAction;	           // �Ƿ񲥷�ѭ������
  ACTION_REFUSE_TYPE  m_eRefuseAction;	           // �Ƿ�ܾ�����
  PERSISTID           m_visID;
};