#ifndef _HOOK_MSG_H_
#define _HOOK_MSG_H_

#include <common/OpCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COPYDATA_HOST_TO_GUEST_MAGIC	(WPARAM)13425
#define COPYDATA_GUEST_TO_HOST_MAGIC	(WPARAM)94325

#define GUEST_TO_HOST_MSG_COMMON \
	vint code; \
	HWND guest_hwnd

#define HOST_TO_GUEST_MSG_COMMON \
	int code

typedef struct guest_to_host_msg_common {
	GUEST_TO_HOST_MSG_COMMON;
} guest_to_host_msg_common_t;

#define MSG_LEN_MAX 1024
#define DATA_LEN_MAX (MSG_LEN_MAX - sizeof (guest_to_host_msg_common_t))

#define WM_GUEST_RETURN			WM_USER+10

#define DIRECT_INPUT_BUFFER_LEN 256
#define DIRECT_INPUT_PIPE_KEYBOARD	"\\\\.\\pipe\\DirectInputXKeyboard"
#define DIRECT_INPUT_PIPE_MOUSE		"\\\\.\\pipe\\DirectInputXMouse"

#define MSG_TYPE_WATCH_GUEST			1
#define MSG_TYPE_PROCMSG_RET			2
#define MSG_TYPE_DIRECT_INPUT_CREATE		3
#define MSG_TYPE_DIRECT_INPUT_KEYBOARD		4
#define MSG_TYPE_DIRECT_INPUT_MOUSE		5
#define MSG_TYPE_GET_CMDLINE			6

typedef struct guest_to_host_msg_watch_guest {
	GUEST_TO_HOST_MSG_COMMON;
	HWND tohwnd;
	UINT message;
	LPARAM lparam;
	WPARAM wparam;
} guest_to_host_msg_watch_guest_t, guest_to_host_msg_procmsg_ret_t;

typedef struct guest_to_host_msg_direct_input_keyboard {
	GUEST_TO_HOST_MSG_COMMON;
	unsigned char keys[DIRECT_INPUT_BUFFER_LEN];
} guest_to_host_msg_direct_input_keyboard_t;

typedef struct direct_input_mouse_state {
//	long x;
//	long y;
	short x;
	short y;
//	long z;
	char buttons;
} direct_input_mouse_state_t;

typedef struct guest_to_host_msg_direct_input_mouse {
	GUEST_TO_HOST_MSG_COMMON;
	direct_input_mouse_state_t state;
} guest_to_host_msg_direct_input_mouse_t;

typedef struct guest_to_host_msg_string {
	GUEST_TO_HOST_MSG_COMMON;
	char str[DATA_LEN_MAX];
} guest_to_host_msg_string_t;

typedef struct guest_to_host_msg_max {
	GUEST_TO_HOST_MSG_COMMON;
	char data[DATA_LEN_MAX];
} guest_to_host_msg_max_t;

typedef struct host_to_guest_msg_common {
	HOST_TO_GUEST_MSG_COMMON;
} host_to_guest_msg_common_t;

void send_msg_to_host (void *msg, int len);

#include "defmsg.h"

#ifdef __cplusplus
}
#endif

#endif /* _HOOK_MSG_H_ */
