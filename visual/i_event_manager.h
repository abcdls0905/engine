#pragma once

#include "fastdelegate.h"

typedef fastdelegate::FastDelegate2<const char*, const IVarList&, void> EventDelegate;

//����һ�¼���һ������
extern void event_connect(const PERSISTID& sender, const char* eventname, EventDelegate recv_delegate);

//�����¼�
extern int event_send(const PERSISTID& sender, const char* eventname, const IVarList& arg);

//���sender����������
extern void event_disconnect(const PERSISTID& sender);

//���ָ���¼��Ĵ���
extern void event_disconnect(const PERSISTID& sender, const char* eventname, EventDelegate recv_callback);

//����/���� sender��������Ϣ����
void event_enableconnect(const PERSISTID& sender, bool enable);

//����/���� sender��ĳ����Ϣ����
void event_enableconnect(const PERSISTID& sender, const char* eventname, bool enable);


//�����κ��¼���һ������
extern void event_connect_globals(EventDelegate recv_callback);

//����κ��¼���һ������
extern void disconnect_globals(EventDelegate recv_callback);