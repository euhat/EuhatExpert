#pragma once

#include <common/OpCommon.h>
#include <common/JyMutex.h>
#include <common/JyDataStream.h>
#include "FmTask.h"

class FmTaskGetSubList;
class JyMsgLoop;

class FmScheduler
{
public:
	class User
	{
	public:
		virtual void fmscOnGetSubList(JyMsgLoop *loop, JyDataReadStream &ds) = 0;
		virtual void fmscOnRefresh() = 0;
		virtual void fmscNeedReconnect() = 0;
		virtual void fmscOnGetSysInfo(JyMsgLoop *loop, JyDataReadStream &ds) = 0;

		virtual void fmscNotify() = 0;
	};
	class UserWrapper
	{
	public:
		class FmscUserParam
		{
		public:
			int cmd_;
			void *p_;
			int64_t i1_;
			int64_t i2_;
		};
		UserWrapper(User *user);
		~UserWrapper();
		void fmscOnGetSubList(JyMsgLoop *loop, JyDataReadStream &ds);
		void fmscOnRefresh();
		void fmscNeedReconnect();
		void fmscOnGetSysInfo(JyMsgLoop *loop, JyDataReadStream &ds);

		void notify();

		User *user_;

	private:
		JyMutexList<FmscUserParam *> listMsg_;
	};
	FmScheduler(User *user);
	~FmScheduler();

	void addRefreshTask(shared_ptr<FmTaskGetSubList> task);
	void add(shared_ptr<FmTask> task);
	void cleanError();
	void cleanAll();
	void remove(int id);
	shared_ptr<FmTask> getFirst(int isRemoteTask = 1);
	shared_ptr<FmTask> get(int id);
	shared_ptr<FmTask> getByType(FmTask::Type type);
	void notifyUi();
	void notifyEngine();
	void onTimer();

	WhMutex mutex_;
	list<shared_ptr<FmTask>> tasks_;
	shared_ptr<FmTaskGetSubList> refreshSubListTask_;
	unique_ptr<UserWrapper> user_;
	JyMsgLoop *remote_;
	JyMsgLoop *local_;
	int idxBase_;
	int needNotifyUi_;
};