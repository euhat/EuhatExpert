#pragma once

#include "JyWorkQueue.h"

enum JyMsgType
{
	JY_MSG_TYPE_INIT,
	JY_MSG_TYPE_FINI,
	JY_MSG_TYPE_SOCK_READ,
	JY_MSG_TYPE_SOCK_LISTEN,
	JY_MSG_TYPE_SOCK_ACCEPT,
	JY_MSG_TYPE_SOCK_CONNECT_BEGIN,
	JY_MSG_TYPE_SOCK_CONNECT_END,
	JY_MSG_TYPE_SOCK_EXCHANGE_ASYM_SECURITY,
	JY_MSG_TYPE_SOCK_EXCHANGE_SYM_SECURITY,
	JY_MSG_TYPE_SOCK_DISCONNECT,
	JY_MSG_TYPE_SOCK_TOTAL_SEND,
	JY_MSG_TYPE_SOCK_TOTAL_RECV,
	JY_MSG_TYPE_USER_START,
	JY_MSG_TYPE_USER_LOOP,
	JY_MSG_TYPE_TIMER,
	JY_MSG_TYPE_USER = 100
};

struct JyMsg
{
	int cmd_;
	void *p_;
	int int_;
};

class JyMsgTimer;

class JyMsgLoop : public JyWorkQueue<JyMsg>
{
	unique_ptr<JyMsgTimer> timer_;

protected:
	virtual int onWork(JyMsg &msg);
	int quit();

public:
	JyMsgLoop();
	virtual ~JyMsgLoop();

	int start();
	int stop();

	void postMsg(int cmd, void *p = NULL, int int_ = 0);

	int createTimer(int id, long msecs);
	int killTimer(int id);

	int delayWork(int type, long msecs);
	int cancelWork(int type);
};