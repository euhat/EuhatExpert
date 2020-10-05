#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define WhSockHandle int
#define WhSockLen socklen_t
#define WH_INVALID_SOCKET (~0)
#define whCloseSocket(_socket) close(_socket)
#define whSleep(_secs) sleep(_secs)
#define WhThreadHandle pthread_t
#define WH_INVALID_THREAD 0
#define WH_THREAD_DEF(_proc, _arg) void *_proc(void *_arg)
#define WH_THREAD_PREPROCESS \
do { \
} while(0)
#define whThreadCreate(_handle, _loop, _param) pthread_create(&_handle, NULL, _loop, _param)
#define whThreadJoin(_handle) pthread_join(_handle, NULL)
#define whThreadTerminate(_handle) pthread_cancel(_handle)
//#define whThreadTerminate(_handle) pthread_kill(_handle,SIGUSR2)
struct WhMutexStruct
{
	pthread_mutexattr_t attr_;
	pthread_mutex_t m_;
};
#define WhMutexParam WhMutexStruct

#define whMutexInit(_handle) \
do { \
	if (pthread_mutexattr_init(&(_handle)->attr_) != 0) { \
		DBG(("init mutex attr failed.\n")); \
		break; \
				} \
	pthread_mutexattr_settype(&(_handle)->attr_, PTHREAD_MUTEX_RECURSIVE_NP); \
	pthread_mutex_init(&(_handle)->m_, &(_handle)->attr_); \
} while (0)

#define whMutexEnter(_handle) pthread_mutex_lock(&(_handle)->mutex_.m_)
#define whMutexLeave(_handle) pthread_mutex_unlock(&(_handle)->mutex_.m_)

#define whMutexFini(_handle) \
do { \
	pthread_mutex_destroy(&(_handle)->m_); \
	pthread_mutexattr_destroy(&(_handle)->attr_); \
} while (0)

#define WhCondParam pthread_cond_t
#define whCondInit(_handle) pthread_cond_init(_handle, NULL)
#define whCondWait(_handle, _mutex) pthread_cond_wait(_handle, &(_mutex)->mutex_.m_)
#define whCondWaitTimedMsec(_handle, _mutex, _msecs) \
do { \
	struct timespec timed; \
	timed.tv_sec = _msecs / 1000; \
	timed.tv_nsec = (_msecs % 1000) * 1000 * 1000; \
	pthread_cond_timedwait(_handle, &(_mutex)->mutex_.m_, &timed); \
} while (0)
#define whCondSignal(_handle) pthread_cond_signal(_handle)
#define whCondFini(_handle) pthread_cond_destroy(_handle)

#define whGetLastError() errno
#define whSockEnvInit()
#define whSockEnvFini()

int whFseek64(FILE *stream, int64_t offset, int origin);
int64_t whFtell64(FILE *stream);

#define whUnlink unlink

#define EUHAT_PATH_BASE "/sdcard/Android/data/com.euhat.euhatexpert/"
#define EUHAT_PATH_DB EUHAT_PATH_BASE "euhat/"
#define EUHAT_PATH_FILES EUHAT_PATH_BASE "files/"
#define EUHAT_PATH_CLIPBOARD EUHAT_PATH_BASE "clipboard/"