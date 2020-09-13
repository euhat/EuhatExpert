#pragma once

#include "../../MfcBasePage.h"
#include <app/FileMan/common/FmScheduler.h>

class FmFileDlg;
class FileManCfgDlg;
class FileManDlg;
class FileManClient;
class FmLocal;

class FileManSubDlg : public MfcBasePage, public FmScheduler::User
{
	void correctPos();
	void correctPos(CRect &rc, int margin, int idd, MfcBasePage *dlg);

	friend class FileManDlg;

public:
	FileManSubDlg(EuhatBase *euhatBase);
	virtual ~FileManSubDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_MAN_SUB_DIALOG };
#endif

	void browserDir(JyMsgLoop *loop, const char *dir);
	virtual void fmscOnGetSubList(JyMsgLoop *loop, JyDataReadStream &ds);
	virtual void fmscOnRefresh();
	virtual void fmscNeedReconnect();
	virtual void fmscOnGetSysInfo(JyMsgLoop *loop, JyDataReadStream &ds);

	virtual void fmscNotify();

	void transfer(int isDownload, int isMove);
	void delFile(JyMsgLoop *loop);
	void newFolder(JyMsgLoop *loop);

	FileManDlg *parentDlg_;

	unique_ptr<FmFileDlg> dlgRemote_;
	unique_ptr<FmFileDlg> dlgLocal_;
	unique_ptr<FmScheduler> scheduler_;
	unique_ptr<FileManClient> remote_;
	unique_ptr<FmLocal> local_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT onFileManClientNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnRemoteCopyToLocal();
	afx_msg void OnBnClickedBtnLocalCopyToRemote();
	afx_msg void OnBnClickedBtnRemoteMoveToLocal();
	afx_msg void OnBnClickedBtnLocalMoveToRemote();
};
