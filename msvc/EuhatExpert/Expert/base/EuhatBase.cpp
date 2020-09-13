#include "stdafx.h"
#include "EuhatBase.h"
#include <direct.h>
#include "../Expert.h"
#include "../ExpertDlg.h"
#include "../cfg/CfgDlg.h"
#include "../KqInformDlg.h"
#include "EuhatMacHdd.h"
#include <dbop/DbOpIni.h>
#include <EuhatUiLocalization.h>
#include <OpWin.h>
#include <EuhatPostDefMfc.h>

int EuhatDemonThread::workProc()
{
	mySystem("taskkill /F /IM EuhatDamon.exe");
	while (workThreadIsRunning_)
	{
		char cmd[256];
		sprintf(cmd, "EuhatDamon.exe eu %lld", (int64_t)hwnd_);
		mySystem(cmd);
	}
	return 1;
}

void EuhatDemonThread::stop()
{
	workThreadIsRunning_ = 0;
//	mySystem("taskkill /F /IM EuhatDamon.exe");
}

EuhatBase::EuhatBase()
{
	mainWnd_ = NULL;
	bmpBk_ = NULL;

	_mkdir(EUHAT_CFG_FOLDER);

	do {
		db_.reset(new DbOpSqlite());
		db_->openFileDbEncrypted(DEFAULT_SQLITE_DB_FILENAME);

		ini_.reset(new DbOpIni(db_.get()));

		int configCount = db_->selectCount("select count(*) from Config;");

		if (configCount < 0 || isMacHddChanged(this))
		{
			db_.reset();
			_unlink(DEFAULT_SQLITE_DB_FILENAME);
		}
		else
			break;
	} while (1);

	workTraceListeningHwnd_ = NULL;
	workTraceKeepDays_ = 7;

	load();
}

EuhatBase::~EuhatBase()
{

}

int EuhatBase::load()
{
	workSpans_.filePath_ = EUHAT_INI_FILENAME_WORK_SPANS;
	if (!workSpans_.load())
	{
		WorkSpan ws;
		ws.beginSecs_ = 30 * 60;
		ws.startTime_ = "08:30:00";
		ws.endTime_ = "17:30:00";
		ws.untilSecs_ = 2 * 60 * 60;
		workSpans_.list_.push_back(ws);
	}

	isAutoRunWhenReboot_ = atoi(readIniString(EUHAT_INI_FILENAME, "General", "autoStart").c_str());
	curUiLanguage_ = readIniInt(EUHAT_INI_FILENAME, "General", "uiLanguage", -1);

	return 1;
}

int EuhatBase::save()
{
	writeIniInt(EUHAT_INI_FILENAME, "General", "autoStart", isAutoRunWhenReboot_);
	writeIniInt(EUHAT_INI_FILENAME, "General", "uiLanguage", curUiLanguage_);

	workSpans_.save();

	return 1;
}

int EuhatBase::modDlg(EuhatUiLocalization *uiLoc, MfcBasePage *page, const char *tag)
{
	uiLoc->modDlg(page->m_hWnd, page->locText_, tag);
	page->refreshUiLocalization();
	return 1;
}

int EuhatBase::refreshUiLocalization()
{
	if (curUiLanguage_ < 0 || curUiLanguage_ >= EUHAT_UI_LANGUAGE_COUNT)
	{
		LCID lcidNew = GetThreadLocale();
		int langType = PRIMARYLANGID(LANGIDFROMLCID(lcidNew));
		switch (langType)
		{
		case LANG_ENGLISH: curUiLanguage_ = EUHAT_UI_LANGUAGE_EN; break;
		case LANG_CHINESE_SIMPLIFIED: curUiLanguage_ = EUHAT_UI_LANGUAGE_CN_SIMPLIFIED; break;
		default: curUiLanguage_ = EUHAT_UI_LANGUAGE_EN; break;
		}
	}

	string xmlPath = EUHAT_LOCALIZATION_FOLDER;
	xmlPath += "/";
	switch (curUiLanguage_)
	{
	case EUHAT_UI_LANGUAGE_CN_SIMPLIFIED:
		xmlPath += "chineseSimplified.xml";
		break;
	default:
		xmlPath += "english.xml";
		break;
	}

	uiLocalizationXmlPath_ = xmlPath;

	EuhatUiLocalization uiLoc;
	if (!uiLoc.init(xmlPath.c_str()))
		return 0;

	tagApp_ = "expert";
	tagCfgDlg_ = "CfgDlg";
	string tag = tagApp_ + "/" + tagCfgDlg_;
	modDlg(&uiLoc, mainWnd_->cfgDlg_.get(), tag.c_str());

	string cfgCommonDlgTag = "CfgCommonDlg";
	tag = tagApp_ + "/" + tagCfgDlg_ + "/" + cfgCommonDlgTag;
	modDlg(&uiLoc, mainWnd_->cfgDlg_->pages_[0].get(), tag.c_str());

	tagCfgWorkTraceDlg_ = "CfgWorkTraceDlg";
	tag = tagApp_ + "/" + tagCfgDlg_ + "/" + tagCfgWorkTraceDlg_;
	modDlg(&uiLoc, mainWnd_->cfgDlg_->pages_[1].get(), tag.c_str());

	string mainDlgTag = "MainDlg";
	tag = tagApp_ + "/" + mainDlgTag;
	modDlg(&uiLoc, mainWnd_, tag.c_str());

	string kqInformDlgTag = "KqInformDlg";
	tag = tagApp_ + "/" + kqInformDlgTag;
	modDlg(&uiLoc, mainWnd_->kqInformDlg_.get(), tag.c_str());

	return 1;
}

int EuhatBase::refreshUiLocalization(MfcBasePage *dlg, const char *tag)
{
	EuhatUiLocalization uiLoc;
	if (!uiLoc.init(uiLocalizationXmlPath_.c_str()))
		return 0;
	modDlg(&uiLoc, dlg, tag);

	return 1;
}