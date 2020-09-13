#pragma once

#include "../local/WorkTrace/WorkSpan.h"
#include <common/JyList.h>
#include <common/JyMutex.h>
#include <common/JyWorkThread.h>

//#define EUHAT_EDITION_WORK_TRACE
#define EUHAT_EDITION_EXPERT

#define EUHAT_CFG_FOLDER "cfg"
#define EUHAT_INI_FILENAME EUHAT_CFG_FOLDER "\\euhatExpert.ini"
#define EUHAT_INI_FILENAME_WORK_SPANS EUHAT_CFG_FOLDER "\\workSpans.ini"
#define EUHAT_LOCALIZATION_FOLDER "localization"

enum
{
	EUHAT_UI_LANGUAGE_EN,
	EUHAT_UI_LANGUAGE_CN_SIMPLIFIED,
	EUHAT_UI_LANGUAGE_CN_TRADITIONAL,
	EUHAT_UI_LANGUAGE_COUNT
};

class EuhatDemonThread : public JyWorkThread
{
protected:
	virtual int workProc();

public:
	void stop();
	HWND hwnd_;
};

class CExpertDlg;
class EuhatUiLocalization;
class MfcBasePage;
class EuhatBitmap;
class DbOpIni;
class DbOpSqlite;

class EuhatBase
{
	int load();
	int modDlg(EuhatUiLocalization *uiLoc, MfcBasePage *page, const char *tag);

	string uiLocalizationXmlPath_;

public:
	EuhatBase();
	~EuhatBase();

	int save();
	int refreshUiLocalization();
	int refreshUiLocalization(MfcBasePage *dlg, const char *tag);

	unique_ptr<DbOpIni> ini_;
	unique_ptr<DbOpSqlite> db_;

	unique_ptr<EuhatDemonThread> demonThread_;

	CExpertDlg *mainWnd_;
	EuhatBitmap *bmpBk_;

	HWND workTraceListeningHwnd_;
	WhMutex mutexWtPayload_;

	string tagApp_;
	string tagCfgDlg_;
	string tagCfgWorkTraceDlg_;

	int workTraceKeepDays_;

	int isAutoRunWhenReboot_;
	int curUiLanguage_;

	JyList<WorkSpan> workSpans_;
};