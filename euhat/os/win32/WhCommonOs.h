#pragma once

#include <winsock2.h>
#include <process.h>
#define WhSockHandle SOCKET
#define WhSockLen int
#define WH_INVALID_SOCKET INVALID_SOCKET
#define whCloseSocket(_socket) closesocket(_socket)
#define whSleep(_secs) Sleep((_secs) * 1000)
#define WhThreadHandle HANDLE
#define WH_INVALID_THREAD (HANDLE)0
#define WH_THREAD_DEF(_proc, _arg) unsigned __stdcall _proc(void *_arg)
#define WH_THREAD_PREPROCESS
#define whThreadCreate(_handle, _loop, _param) _handle = (WhThreadHandle)_beginthreadex(NULL, 0, _loop, _param, 0, NULL)
#define whThreadJoin(_handle) \
do { \
	WaitForSingleObject(_handle, INFINITE); \
	CloseHandle(_handle); \
} while (0)
#define whThreadTerminate(_handle) TerminateThread(_handle, 0)
#define WhMutexParam CRITICAL_SECTION
#define whMutexInit(_handle) InitializeCriticalSection(_handle)
#define whMutexEnter(_handle) EnterCriticalSection(&(_handle)->mutex_)
#define whMutexLeave(_handle) LeaveCriticalSection(&(_handle)->mutex_)
#define whMutexFini(_handle) DeleteCriticalSection(_handle)
#define WhCondParam HANDLE
#define whCondInit(_handle) *(_handle) = CreateSemaphore(NULL, 0, 5, NULL)
#define whCondWait(_handle, _mutex) \
	whMutexLeave(_mutex); \
	WaitForSingleObject(*(_handle), INFINITE); \
	whMutexEnter(_mutex)
#define whCondWaitTimedMsec(_handle, _mutex, _msecs) \
	whMutexLeave(_mutex); \
	WaitForSingleObject(*(_handle), _msecs); \
	whMutexEnter(_mutex)
#define whCondSignal(_handle) ReleaseSemaphore(*(_handle), 1, NULL)
#define whCondFini(_handle) CloseHandle(*(_handle))
#define whGetLastError() WSAGetLastError()
#define whSockEnvInit() \
	WSADATA wsaData; \
	WSAStartup(MAKEWORD(2, 2), &wsaData)
#define whSockEnvFini() \
	WSACleanup()

#define whFseek64 _fseeki64
#define whFtell64 _ftelli64
#define whStrToInt64 _atoi64

int gettimeofday(struct timeval *tp, void *tzp);

#pragma comment(lib,"WS2_32.lib")
