#pragma once

#include <common/OpCommon.h>
#include "../common/FileManType.h"

class JyMsgLoop;
class JyDataReadStream;

class FmTask
{
public:
	enum Type
	{
		TypeConnect,
		TypeGetSubList,
		TypeGetPeerSubList,
		TypeDownload,
		TypeUpload,
		TypeCopy,
		TypeCopyPeer,
		TypeDel,
		TypeDelPeer,
		TypeNew,
		TypeNewPeer
	};
	virtual int getPriority() = 0;

	Type type_;
	int id_;
	FileManResultType result_;

};

class FmTaskConnect : public FmTask
{
public:
	FmTaskConnect() { type_ = TypeConnect; }
	int getPriority() { return 1; }

	string ip_;
	int port_;
	int retryTimes_;
};

class FmTaskGetSubList : public FmTask
{
public:
	enum SubsequentAction
	{
		ActionInformUi,
		ActionDownUp,
		ActionCopy,
		ActionDel
	};
	FmTaskGetSubList(Type type) { assert(type == TypeGetSubList || type == TypeGetPeerSubList); type_ = type; }
	int getPriority()
	{ 
		if (action_ == ActionInformUi) return 10;
		return 70;
	}

	unique_ptr<char[]> path_;
	SubsequentAction action_;
	unique_ptr<char[]> peerPath_;
	int isMove_;
};

class FmTaskDownload : public FmTask
{
public:
	FmTaskDownload() { type_ = TypeDownload; eatGuess_ = 0; }
	int getPriority() { return totalFileSize_ > FILE_MAN_RECV_FILE_SEGMENT_MAX_LEN ? 60 : 20; }

	unique_ptr<char[]> localPath_;
	unique_ptr<char[]> peerPath_;
	int64_t offset_;
	int eatGuess_;
	int64_t totalFileSize_;
	time_t lastWriteTime_;
	int isMove_;
};

class FmTaskUpload : public FmTaskDownload
{
public:
	FmTaskUpload() { type_ = TypeUpload; eatGuess_ = 0; }
};

class FmTaskCopy : public FmTask
{
public:
	FmTaskCopy(Type type) { assert(type == TypeCopy || type == TypeCopyPeer); type_ = type; }
	int getPriority() { return 60; }
	unique_ptr<char[]> fromPath_;
	unique_ptr<char[]> toPath_;
	int64_t offset_;
	int64_t totalFileSize_;
	int isMove_;
};

class FmTaskDel : public FmTask
{
public:
	FmTaskDel(Type type) { assert(type == TypeDel || type == TypeDelPeer); type_ = type; }
	int getPriority() { return priority_; }
	unique_ptr<char[]> path_;
	int isFolder_;
	int priority_;
};

class FmTaskNew : public FmTask
{
public:
	FmTaskNew(Type type) { assert(type == TypeNew || type == TypeNewPeer); type_ = type; }
	int getPriority() { return 10; }
	unique_ptr<char[]> path_;
	int isFolder_;
	int needInformUi_;
};