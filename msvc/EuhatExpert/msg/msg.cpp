#include <windows.h>
#include "msg.h"

#pragma warning(disable:4996)

#pragma data_seg(".MsgConnect")
HHOOK g_hhook_callwndproc = NULL;
HHOOK g_hhook_callwndprocret = NULL;
HHOOK g_hhook_getmessage = NULL;
HHOOK g_hhook_msgfilter = NULL;
HHOOK g_hhook_shell = NULL;
HHOOK g_hhook_sysmsgfilter = NULL;
HINSTANCE g_hinst = NULL; 
HWND g_host_hwnd = NULL;
#pragma data_seg()

#pragma comment(linker,"/section:.MsgConnect,rws")

BOOL APIENTRY DllMain (HANDLE hmod, DWORD ul_reason_for_call, LPVOID lpReserved)
{
#ifndef X64
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
#endif
	return TRUE;
}

/*
long WINAPI guest_proc (HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
	case WM_COPYDATA:
		break;
	default:
		break;
	}
	return DefWindowProc (hwnd, message, wparam, lparam);
}
*/

void send_msg_to_host (void *msg, int len)
{
	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = len;
	cds.lpData = msg;
	if (NULL != g_host_hwnd)
		SendMessage (g_host_hwnd, WM_COPYDATA, COPYDATA_GUEST_TO_HOST_MAGIC, (LPARAM)&cds);
/*	if (NULL == g_guest_hwnd) {
		g_guest_hwnd = create_win (guest_hwnd);
		if (NULL != g_guest_hwnd) {
			msg->tohwnd = g_guest_hwnd;
			SendMessage (g_host_hwnd, WM_COPYDATA, COPYDATA_MSG_WPARAM_GUEST, (LPARAM)&cds);
		}
	}
*/
}

static int process_host_to_guest_msg (host_to_guest_msg_common_t *from_host, int from_host_len)
{
	guest_to_host_msg_string_t to_host_string;
	switch (from_host->code) {
	case MSG_TYPE_GET_CMDLINE:
		if (from_host_len != sizeof (*from_host))
			return 0;
		to_host_string.code = from_host->code;
		strcpy (to_host_string.str, "hi, cmdline");
		send_msg_to_host (&to_host_string, sizeof (guest_to_host_msg_string_t));
		break;
	default:
		return 0;
	}
	return 1;
}

static int parse_host_to_guest_msg (HWND guest_hwnd, vint msg)
{
	switch (msg) {
	case MSG_TYPE_GET_CMDLINE: {
/*		guest_to_host_msg_string_t to_host_string;

		to_host_string.code = msg;
		to_host_string.guest_hwnd = guest_hwnd;
		strncpy(to_host_string.str, WIN_CMDLINE(), DATA_LEN_MAX - 1);
		send_msg_to_host(&to_host_string, (int)(sizeof(guest_to_host_msg_common_t) + strlen(to_host_string.str) + 1));
*/
		break;
	}
	default:
		return 0;
	}
	return 1;
}

/*
int g_wm_paint_point_x = 0;
int g_wm_paint_point_y = 0;
*/

/*
 * The CWPSTRUCT structure defines the message parameters passed to a WH_CALLWNDPROC hook procedure, CallWndProc. 
 *
 * typedef struct {
 *     LPARAM lParam;
 *     WPARAM wParam;
 *     UINT message;
 *     HWND hwnd;
 * } CWPSTRUCT, *PCWPSTRUCT;
 *
 * lParam
 *     Specifies additional information about the message. The exact meaning depends on the message value. 
 * wParam
 *     Specifies additional information about the message. The exact meaning depends on the message value. 
 * message
 *     Specifies the message. 
 * hwnd
 *     Handle to the window to receive the message. 
 *
 */

/*
 * Windows 95/98/Me, Windows NT 3.51: The system calls this function whenever the thread 
 * calls the SendMessage function. The WH_CALLWNDPROC hook is called in the context of 
 * the thread that calls SendMessage, not the thread that receives the message. 
 *
 * Windows NT 4.0 and later: The system calls this function before calling the window 
 * procedure to process a message sent to the thread.
 *
 * nCode
 *         [in] Specifies whether the hook procedure must process the message. 
 *         If nCode is HC_ACTION, the hook procedure must process the message. 
 *         If nCode is less than zero, the hook procedure must pass the message 
 *         to the CallNextHookEx function without further processing and must 
 *         return the value returned by CallNextHookEx. 
 * wParam
 *         [in] Specifies whether the message was sent by the current thread. 
 *         If the message was sent by the current thread, it is nonzero; otherwise, it is zero. 
 * lParam
 *         [in] Pointer to a CWPSTRUCT structure that contains details about the message. 
 * Return Value
 *         If nCode is less than zero, the hook procedure must return the value 
 *         returned by CallNextHookEx. 
 *
 *         If nCode is greater than or equal to zero, it is highly recommended 
 *         that you call CallNextHookEx and return the value it returns; otherwise, 
 *         other applications that have installed WH_CALLWNDPROC hooks will not receive 
 *         hook notifications and may behave incorrectly as a result. If the hook 
 *         procedure does not call CallNextHookEx, the return value should be zero. 
 * Remarks
 *         The CallWndProc hook procedure can examine the message, but it 
 *         cannot modify it. After the hook procedure returns control to the system, 
 *         the message is passed to the window procedure. 
 */
LRESULT CALLBACK hook_call_wnd_proc (int ncode, WPARAM wparam, LPARAM lparam)
{
	CWPSTRUCT * p = (CWPSTRUCT *) lparam;
//	guest_to_host_msg_watch_guest_t to_host;
	
	/*
	char strCaption[1024];
	HDC hdc;
	PAINTSTRUCT ps;
	POINT pt;
	RECT rect;	
	*/

	if (GetWindowThreadProcessId (p->hwnd, NULL) == GetWindowThreadProcessId (g_host_hwnd, NULL))
		return CallNextHookEx (g_hhook_callwndproc, ncode, wparam, lparam);

	if (HC_ACTION == ncode && p != NULL) {
		switch(p->message) {
		case WM_PAINT: 
			/*
			GetWindowText (p->hwnd, strCaption, 1020);
			if (strstr (strCaption, "¹ØÓÚ") != NULL) {
				hdc = BeginPaint (p->hwnd, &ps);
				GetCursorPos (&pt);
				ScreenToClient (p->hwnd, &pt);
				SetPixel (hdc, g_wm_paint_point_x, g_wm_paint_point_y, 0);
				LineTo (hdc, pt.x, pt.y);
				g_wm_paint_point_x = pt.x;
				g_wm_paint_point_y = pt.y;
				EndPaint(p->hwnd, &ps);	
			}
			*/
			break;
		case WM_MOUSEMOVE:
			/*
			MessageBox (NULL, "hi,move", NULL, 0);
			GetClientRect (p->hwnd, &rect);
			rect.left = 0;
			rect.top = 0;
			rect.right = 200;
			rect.bottom = 200;
			InvalidateRect (p->hwnd, &rect, 0);
			*/
			break;
			/*
#define CASE_MSG(id) case id:
		CASE_MSG_LIST
#undef CASE_MSG
			to_host.code = MSG_TYPE_WATCH_GUEST;
			to_host.lparam = p->lParam;
			to_host.wparam = p->wParam;
			to_host.message = p->message;
			to_host.tohwnd = p->hwnd;
			send_msg_to_host (&to_host, sizeof (to_host));
			*/
			break;
		default:
			break;
		}
	}
	return CallNextHookEx (g_hhook_callwndproc, ncode, wparam, lparam);
}

/*
 * The CWPRETSTRUCT structure defines the message parameters passed to a WH_CALLWNDPROCRET hook procedure, CallWndRetProc. 
 *
 * typedef struct {
 *     LRESULT lResult;
 *     LPARAM lParam;
 *     WPARAM wParam;
 *     UINT message;
 *     HWND hwnd;
 * } CWPRETSTRUCT, *PCWPRETSTRUCT;
 *
 * lResult
 *     Specifies the return value of the window procedure that processed the message specified by the message value. 
 * lParam
 *     Specifies additional information about the message. The exact meaning depends on the message value. 
 * wParam
 *     Specifies additional information about the message. The exact meaning depends on the message value. 
 * message
 *     Specifies the message. 
 * hwnd
 *     Handle to the window that processed the message specified by the message value. 
 *
 */

/*
 * The system calls this function after the SendMessage function is called. The hook procedure can examine 
 * the message; it cannot modify it. 
 *
 * nCode
 *         [in] Specifies whether the hook procedure must process the message. 
 *         If nCode is HC_ACTION, the hook procedure must process the message. 
 *         If nCode is less than zero, the hook procedure must pass the message to 
 *         the CallNextHookEx function without further processing and should return 
 *         the value returned by CallNextHookEx. 
 * wParam
 *         [in] Specifies whether the message is sent by the current process. 
 *         If the message is sent by the current process, it is nonzero; otherwise, it is NULL. 
 * lParam
 *         [in] Pointer to a CWPRETSTRUCT structure that contains details about the message. 
 * Return Value
 *         If nCode is less than zero, the hook procedure must return the value 
 *         returned by CallNextHookEx. 
 *
 *         If nCode is greater than or equal to zero, it is highly recommended that 
 *         you call CallNextHookEx and return the value it returns; otherwise, 
 *         other applications that have installed WH_CALLWNDPROCRET hooks will 
 *         not receive hook notifications and may behave incorrectly as a result. 
 *         If the hook procedure does not call CallNextHookEx, the return value should be zero. 
 */
LRESULT CALLBACK hook_call_wnd_proc_ret (int ncode, WPARAM wparam, LPARAM lparam)
{
	CWPRETSTRUCT * p = (CWPRETSTRUCT *) lparam;
/*	guest_to_host_msg_procmsg_ret_t to_host;
*/

	if (GetWindowThreadProcessId (p->hwnd, NULL) == GetWindowThreadProcessId (g_host_hwnd, NULL))
		return CallNextHookEx (g_hhook_callwndprocret, ncode, wparam, lparam);

	if (HC_ACTION == ncode && p != NULL) {
		switch(p->message) {
/*
#define CASE_MSG(id) case id:
		CASE_MSG_LIST
#undef CASE_MSG
*/
		case WM_SHOWWINDOW:
			break;
/*		case WM_PAINT:
			to_host.code = MSG_TYPE_PROCMSG_RET;
			to_host.lparam = p->lParam;
			to_host.wparam = p->wParam;
			to_host.message = p->message;
			to_host.tohwnd = p->hwnd;
			send_msg_to_host (&to_host, sizeof (to_host));
			break;
*/		default:
			break;
		}
	}
	return CallNextHookEx (g_hhook_callwndprocret, ncode, wparam, lparam);	
}

/*
 * The MSG structure contains message information from a thread's message queue. 
 *
 * typedef struct {
 *     HWND hwnd;
 *     UINT message;
 *     WPARAM wParam;
 *     LPARAM lParam;
 *     DWORD time;
 *     POINT pt;
 * } MSG, *PMSG;
 *
 * hwnd
 *     Handle to the window whose window procedure receives the message. 
 * message
 *     Specifies the message identifier. Applications can only use the low word; the high word is reserved by the system. 
 * wParam
 *     Specifies additional information about the message. The exact meaning depends on the value of the message member. 
 * lParam
 *     Specifies additional information about the message. The exact meaning depends on the value of the message member. 
 * time
 *     Specifies the time at which the message was posted. 
 * pt
 *     Specifies the cursor position, in screen coordinates, when the message was posted. 
 */

/*
 * The system calls this function whenever the GetMessage or PeekMessage function has retrieved a message from 
 * an application message queue. Before returning the retrieved message to the caller, 
 * the system passes the message to the hook procedure. 
 *
 * code
 *         [in] Specifies whether the hook procedure must process the message. 
 *         If code is HC_ACTION, the hook procedure must process the message. 
 *         If code is less than zero, the hook procedure must pass the message 
 *         to the CallNextHookEx function without further processing and 
 *         should return the value returned by CallNextHookEx. 
 * wParam
 *         [in] Specifies whether the message has been removed from the queue. 
 *         This parameter can be one of the following values. 
 * 	PM_NOREMOVE
 *         Specifies that the message has not been removed from the queue. 
 *         (An application called the PeekMessage function, specifying the PM_NOREMOVE flag.)
 * 	PM_REMOVE
 *         Specifies that the message has been removed from the queue. 
 *         (An application called GetMessage, or it called the PeekMessage function, 
 *         specifying the PM_REMOVE flag.)
 * lParam
 *         [in] Pointer to an MSG structure that contains details about the message. 
 * Return Value
 *         If code is less than zero, the hook procedure must return the value 
 *         returned by CallNextHookEx. 
 *
 *         If code is greater than or equal to zero, it is highly recommended 
 *         that you call CallNextHookEx and return the value it returns; 
 *         otherwise, other applications that have installed WH_GETMESSAGE hooks 
 *         will not receive hook notifications and may behave incorrectly as a result. 
 *         If the hook procedure does not call CallNextHookEx, the return value should be zero. 
 * Remarks
 *         The GetMsgProc hook procedure can examine or modify the message. 
 *         After the hook procedure returns control to the system, 
 *         the GetMessage or PeekMessage function returns the message, 
 *         along with any modifications, to the application that originally called it. 
 */
LRESULT CALLBACK hook_get_message (int ncode, WPARAM wparam, LPARAM lparam)
{
	PMSG p = (PMSG)lparam;
//	host_to_guest_msg_common_t *from_host;
//	COPYDATASTRUCT *cds;

//	if (GetWindowThreadProcessId (p->hwnd, NULL) == GetWindowThreadProcessId (g_host_hwnd, NULL))
//		return CallNextHookEx (g_hhook_getmessage, ncode, wparam, lparam);

	if (HC_ACTION == ncode && p != NULL) {
//	if (ncode >= 0 && p != NULL) {
		switch(p->message) {
		case WM_USER:
//			Msg ("wm_user in guest, wparam:%x, lparam:%x.", p->wParam, p->lParam);
			if (COPYDATA_HOST_TO_GUEST_MAGIC != p->wParam)
				break;
//			Msg ("wm_user in guest, is from replayer.");
			if (parse_host_to_guest_msg (p->hwnd, p->lParam)) {
//				Msg ("wm_user in guest, succeed!");
				p->message = 0;
				p->wParam = 0;
				p->lParam = 0;
//				return 0;
				return CallNextHookEx (g_hhook_getmessage, ncode, wparam, lparam);	
			}
			break;
		case WM_COPYDATA:
			break;
			/*
//			Msg ("copydata in guest");
			if (COPYDATA_HOST_TO_GUEST_MAGIC != p->wParam)
				break;
//			Msg ("get in guest");
			cds = (COPYDATASTRUCT *)p->lParam;
			from_host = (host_to_guest_msg_common_t *)cds->lpData;
			if (process_host_to_guest_msg (from_host, cds->cbData))
				return 0;
			break;
			*/
		default:
			break;
		}
	}
	return CallNextHookEx (g_hhook_getmessage, ncode, wparam, lparam);	
}

/*
 * The system calls this function after an input event occurs in a dialog box, message box, menu, 
 * or scroll bar, but before the message generated by the input event is processed. 
 * The hook procedure can monitor messages for a dialog box, message box, menu, 
 * or scroll bar created by a particular application or all applications. 
 *
 * code
 *         [in] Specifies the type of input event that generated the message. 
 *         If code is less than zero, the hook procedure must pass the message 
 *         to the CallNextHookEx function without further processing and 
 *         return the value returned by CallNextHookEx. 
 *         This parameter can be one of the following values. 
 * 	MSGF_DDEMGR
 *         The input event occurred while the Dynamic Data Exchange Management 
 *         Library (DDEML) was waiting for a synchronous transaction to finish. 
 *         For more information about DDEML, see Dynamic Data Exchange Management Library.
 * 	MSGF_DIALOGBOX
 *         The input event occurred in a message box or dialog box.
 * 	MSGF_MENU
 *         The input event occurred in a menu.
 * 	MSGF_SCROLLBAR
 *         The input event occurred in a scroll bar.
 * wParam
 *         This parameter is not used. 
 * lParam
 *         [in] Pointer to an MSG structure. 
 * Return Value
 *         If code is less than zero, the hook procedure must return the value returned by CallNextHookEx. 
 *
 *         If code is greater than or equal to zero, and the hook procedure 
 *         did not process the message, it is highly recommended that you call CallNextHookEx 
 *         and return the value it returns; otherwise, other applications that have installed 
 *         WH_MSGFILTER hooks will not receive hook notifications and may behave incorrectly 
 *         as a result. If the hook procedure processed the message, it may return a nonzero 
 *         value to prevent the system from passing the message to the rest of the hook 
 *         chain or the target window procedure. 
 * Remarks
 *         If an application that uses the DDEML and performs synchronous transactions 
 *         must process messages before they are dispatched, it must use the WH_MSGFILTER hook. 
 */
LRESULT CALLBACK hook_msg_filter (int ncode, WPARAM wparam, LPARAM lparam)
{
	return CallNextHookEx (g_hhook_msgfilter, ncode, wparam, lparam);	
}

/*
 * The function receives notifications of Shell events from the system. 
 *
 * nCode
 *         [in] Specifies the hook code. If nCode is less than zero, the hook procedure 
 *         must pass the message to the CallNextHookEx function without further processing 
 *         and should return the value returned by CallNextHookEx. 
 *         This parameter can be one of the following values. 
 * 	HSHELL_ACCESSIBILITYSTATE
 *         Windows 2000/XP: The accessibility state has changed. 
 * 	HSHELL_ACTIVATESHELLWINDOW
 *         The shell should activate its main window.
 * 	HSHELL_APPCOMMAND
 *         Windows 2000/XP: The user completed an input event (for example, pressed 
 *         an application command button on the mouse or an application command key 
 *         on the keyboard), and the application did not handle the WM_APPCOMMAND message 
 *         generated by that input. 
 *         If the Shell procedure handles the WM_COMMAND message, it should not call CallNextHookEx. 
 *         See the Return Value section for more information.
 * 	HSHELL_GETMINRECT
 *         A window is being minimized or maximized. The system needs the coordinates of the minimized rectangle for the window. 
 * 	HSHELL_LANGUAGE
 *         Keyboard language was changed or a new keyboard layout was loaded.
 * 	HSHELL_REDRAW
 *         The title of a window in the task bar has been redrawn. 
 * 	HSHELL_TASKMAN
 *         The user has selected the task list. A shell application that provides a task 
 *         list should return TRUE to prevent Microsoft Windows from starting its task list.
 * 	HSHELL_WINDOWACTIVATED
 *         The activation has changed to a different top-level, unowned window. 
 * 	HSHELL_WINDOWCREATED
 *         A top-level, unowned window has been created. The window exists when the system calls this hook.
 * 	HSHELL_WINDOWDESTROYED
 *         A top-level, unowned window is about to be destroyed. The window still exists when the system calls this hook.
 * 	HSHELL_WINDOWREPLACED
 *         Windows XP: A top-level window is being replaced. The window exists when the system calls this hook. 
 * wParam
 *         [in] The value depends on the value of the nCode parameter:
 * 	HSHELL_ACCESSIBILITYSTATE 
 * 	   Windows 2000/XP: Indicates which accessibility feature has changed state. 
 * 	   This value is one of the following: ACCESS_FILTERKEYS, ACCESS_MOUSEKEYS, or ACCESS_STICKYKEYS.  
 * 	HSHELL_APPCOMMAND 
 * 	   Windows 2000/XP: Where the WM_APPCOMMAND message was originally sent; 
 * 	   for example, the handle to a window. For more information, see cmd parameter in WM_APPCOMMAND. 
 * 	HSHELL_GETMINRECT 
 * 	   Handle to the minimized or maximized window. 
 *      HSHELL_LANGUAGE 
 *         Handle to the window. 
 *      HSHELL_REDRAW 
 *         Handle to the redrawn window. 
 *      HSHELL_WINDOWACTIVATED 
 *         Handle to the activated window. 
 *      HSHELL_WINDOWCREATED 
 *         Handle to the created window. 
 *      HSHELL_WINDOWDESTROYED 
 *         Handle to the destroyed window. 
 *      HSHELL_WINDOWREPLACED 
 *         Windows XP: Handle to the window being replaced. 
 * lParam
 *         [in] The value depends on the value of the nCode parameter:
 *      HSHELL_APPCOMMAND 
 *         Windows 2000/XP:
 *       GET_APPCOMMAND_LPARAM(lParam) is the application command 
 *         corresponding to the input event. 
 *       GET_DEVICE_LPARAM(lParam) indicates what generated the input event; 
 *         for example, the mouse or keyboard. For more information, 
 *         see the uDevice parameter description under WM_APPCOMMAND. 
 *       GET_FLAGS_LPARAM(lParam) depends on the value of cmd in WM_APPCOMMAND. 
 *         For example, it might indicate which virtual keys were held down when 
 *         the WM_APPCOMMAND message was originally sent. For more information, 
 *         see the dwCmdFlags description parameter under WM_APPCOMMAND.
 *          
 *      HSHELL_GETMINRECT 
 *         Pointer to a RECT structure.  
 *      HSHELL_LANGUAGE 
 *         Handle to a keyboard layout.  
 *      HSHELL_REDRAW 
 *         The value is TRUE if the window is flashing, or FALSE otherwise.  
 *      HSHELL_WINDOWACTIVATED 
 *         The value is TRUE if the window is in full-screen mode, or FALSE otherwise.  
 *      HSHELL_WINDOWREPLACED 
 *         Windows XP: Handle to the new window. 
 * Return Value
 *         The return value should be zero unless the value of nCode is HSHELL_APPCOMMAND 
 *         and the shell procedure handles the WM_COMMAND message. In this case, the return should be nonzero.
 */
LRESULT CALLBACK hook_shell (int ncode, WPARAM wparam, LPARAM lparam)
{
	return CallNextHookEx (g_hhook_shell, ncode, wparam, lparam);	
}

/*
 * The system calls this function after an input event occurs in a dialog box, message box, menu, or scroll bar, 
 * but before the message generated by the input event is processed. The function can monitor messages for 
 * any dialog box, message box, menu, or scroll bar in the system. 
 *
 * nCode
 *         [in] Specifies the type of input event that generated the message. 
 *         If nCode is less than zero, the hook procedure must pass the message to the CallNextHookEx function 
 *         without further processing and should return the value returned by CallNextHookEx. 
 *         This parameter can be one of the following values. 
 * 	MSGF_DIALOGBOX
 *         The input event occurred in a message box or dialog box.
 *      MSGF_MENU
 *         The input event occurred in a menu.
 *      MSGF_SCROLLBAR
 *         The input event occurred in a scroll bar.
 * wParam
 *         This parameter is not used. 
 * lParam
 *         [in] Pointer to an MSG message structure. 
 * Return Value
 *
 *         If nCode is less than zero, the hook procedure must return the value returned by CallNextHookEx. 
 *
 *         If nCode is greater than or equal to zero, and the hook procedure did not process the message, 
 *         it is highly recommended that you call CallNextHookEx and return the value it returns; 
 *         otherwise, other applications that have installed WH_SYSMSGFILTER hooks will not receive 
 *         hook notifications and may behave incorrectly as a result. If the hook procedure processed 
 *         the message, it may return a nonzero value to prevent the system from passing 
 *         the message to the target window procedure. 
 */
LRESULT CALLBACK hook_sys_msg_filter (int ncode, WPARAM wparam, LPARAM lparam)
{
	return CallNextHookEx (g_hhook_sysmsgfilter, ncode, wparam, lparam);	
}

BOOL WINAPI Start(HINSTANCE hinst, HWND hwnd)
{
	DWORD threadid;

#ifdef TEST_SELF_DINPUT
	hinst = 0;
	threadid = GetWindowThreadProcessId (hwnd, NULL);
#else
	threadid = 0;
#endif

	g_hinst = hinst;
	g_host_hwnd = hwnd;

	/* Installs a hook procedure that monitors messages 
	 * before the system sends them to the destination window procedure.
	 */
	g_hhook_callwndproc = SetWindowsHookEx (WH_CALLWNDPROC, hook_call_wnd_proc, g_hinst, threadid);
	/* Installs a hook procedure that monitors messages posted to a message queue. 
	 */
	g_hhook_getmessage = SetWindowsHookEx (WH_GETMESSAGE, hook_get_message, g_hinst, threadid);
	/* Installs a hook procedure that monitors messages 
	 * after they have been processed by the destination window procedure.
	 */
	g_hhook_callwndprocret = SetWindowsHookEx (WH_CALLWNDPROCRET, hook_call_wnd_proc_ret, g_hinst, threadid);
#if 0
	/* Installs a hook procedure that monitors messages generated as a result 
	 * of an input event in a dialog box, message box, menu, or scroll bar. 
	 */
	g_hhook_msgfilter = SetWindowsHookEx (WH_MSGFILTER, hook_msg_filter, g_hinst, threadid);
	/*
	 * Installs a hook procedure that receives notifications useful to shell applications.
	 */
	g_hhook_shell = SetWindowsHookEx (WH_SHELL, hook_shell, g_hinst, threadid);
	/*
	 * Installs a hook procedure that monitors messages generated as a result of an input event
	 * in a dialog box, message box, menu, or scroll bar. The hook procedure monitors these 
	 * messages for all applications in the same desktop as the calling thread.
	 */
	g_hhook_sysmsgfilter = SetWindowsHookEx (WH_SYSMSGFILTER, hook_sys_msg_filter, g_hinst, threadid);
#endif
	return TRUE;
}

__declspec(dllexport) void WINAPI Stop()
{
	UnhookWindowsHookEx (g_hhook_callwndproc);
	UnhookWindowsHookEx (g_hhook_getmessage);
	UnhookWindowsHookEx (g_hhook_callwndprocret);
#if 0
	UnhookWindowsHookEx (g_hhook_msgfilter);
	UnhookWindowsHookEx (g_hhook_shell);
	UnhookWindowsHookEx (g_hhook_sysmsgfilter);
#endif
} 

