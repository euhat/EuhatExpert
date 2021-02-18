#include "stdafx.h"
#include "../../Expert.h"
#include "FileServerDlg.h"
#include "../../peer/FileMan/EuhatCertificateDlg.h"
#include <dbop/DbOpIni.h>
#include <mfc/OpMfc.h>
#include <EuhatTabCtrl.h>
#include <app/FileMan/server/FileManServer.h>
#include <app/FileMan/common/FileManType.h>
#include <os/EuhatPath.h>
#include <EuhatPostDefMfc.h>

#define INI_SECTION_FILE_SERVER "fileServer"

//#pragma comment(lib, "Crypt32.lib")
//#pragma comment(lib, "libcrypto.lib")

FileServerDlg::FileServerDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_FILE_SERVER_DIALOG, euhatBase)
{
	srv_.reset(new FileManServer());
	certDlg_.reset(new EuhatCertificateDlg(euhatBase_));
}

FileServerDlg::~FileServerDlg()
{

}

void FileServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, editOutput_);
	DDX_Control(pDX, IDC_EDIT_VISIT_CODE, editVisitCode_);
	DDX_Control(pDX, IDC_EDIT_VISIT_CODE, editVisitCode_);
}

BEGIN_MESSAGE_MAP(FileServerDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_START, &FileServerDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_BROWSE_DIR, &FileServerDlg::OnBnClickedBtnBrowseDir)
	ON_MESSAGE(WH_USER_MSG_ADD_MSG, &FileServerDlg::onAddMsg)
	ON_BN_CLICKED(IDC_BTN_STOP, &FileServerDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_SHOW_HIDE, &FileServerDlg::OnBnClickedBtnShowHide)
	ON_BN_CLICKED(IDC_BTN_SHOW_HIDE, &FileServerDlg::OnBnClickedBtnShowHide)
END_MESSAGE_MAP()

LRESULT FileServerDlg::onAddMsg(WPARAM wParam, LPARAM lParam)
{
	return whEditCtrlAddMsg(editOutput_, wParam, lParam);
}

void FileServerDlg::correctPos()
{
	if (NULL == GetDlgItem(IDC_STATIC_CERT_DLG))
		return;

	CRect rcCert;
	GetDlgItem(IDC_STATIC_CERT_DLG)->GetWindowRect(&rcCert);
	ScreenToClient(&rcCert);
	certDlg_->MoveWindow(rcCert);
}

BOOL FileServerDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	certDlg_->certName_ = JY_TCP_INI_SERVER_CERT_KEY;
	certDlg_->create(this);

	string baseDir = euhatBase_->ini_->readStr(INI_SECTION_FILE_SERVER, "baseDir", "");
	baseDir = utf8ToGb(baseDir.c_str());
	CString cstr = CA2T(baseDir.c_str());
	SetDlgItemText(IDC_EDIT_FILE_SRV_PATH, cstr);

	int port = euhatBase_->ini_->readInt(INI_SECTION_FILE_SERVER, "port", FILE_MAN_DEFAULT_PORT);
	SetDlgItemInt(IDC_EDIT_IP_PORT, port);

	string visitCode = euhatBase_->ini_->readStr(INI_SECTION_FILE_SERVER, "visitCode", "");
	cstr = CA2T(visitCode.c_str());
	SetDlgItemText(IDC_EDIT_VISIT_CODE, cstr);

	GetDlgItem(IDC_BTN_STOP)->EnableWindow(0);

	correctPos();

	return TRUE;
}

void FileServerDlg::OnSize(UINT nType, int cx, int cy)
{
	MfcBasePage::OnSize(nType, cx, cy);

	correctPos();
}

void FileServerDlg::OnBnClickedBtnStart()
{
	if (!certDlg_->isGenerated())
	{
		MessageBox(_T("Certificate is not generated, can't start."), _T("Error"), 0);
		return;
	}
	
	CString cstr;
	GetDlgItemText(IDC_EDIT_VISIT_CODE, cstr);
	string visitCode = CT2A(cstr);
	if (visitCode.empty())
	{
		MessageBox(_T("Visit Code can't be empty, can't start."), _T("Error"), 0);
		return;
	}
	euhatBase_->ini_->write(INI_SECTION_FILE_SERVER, "visitCode", visitCode.c_str());

	GetDlgItemText(IDC_EDIT_FILE_SRV_PATH, cstr);
	string str = CT2A(cstr);
	string baseDir = gbToUtf8(str.c_str());
	EuhatPath euPath;
	euPath.inStr(baseDir.c_str());
	baseDir = euPath.toStr();
	if (baseDir.empty())
	{
		MessageBox(_T("Share Dir can't be empty, can't start."), _T("Error"), 0);
		return;
	}
	euhatBase_->ini_->write(INI_SECTION_FILE_SERVER, "baseDir", baseDir.c_str());

	FileManServer *fmSrv = (FileManServer *)srv_.get();
	fmSrv->setDbOp(euhatBase_->db_.get());
	fmSrv->baseDir_ = baseDir;
	fmSrv->visitCode_ = visitCode;

	int port = GetDlgItemInt(IDC_EDIT_IP_PORT);
	euhatBase_->ini_->write(INI_SECTION_FILE_SERVER, "port", port);

	srv_->serverPort_ = port;
	srv_->start();

	GetDlgItem(IDC_BTN_START)->EnableWindow(0);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(1);

	msg("File server starts.\r\n");
}

void FileServerDlg::OnBnClickedBtnStop()
{
	srv_.reset(new FileManServer());

	GetDlgItem(IDC_BTN_START)->EnableWindow(1);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(0);
}

void FileServerDlg::msg(const char *txt)
{
	whEditCtrlMsg(m_hWnd, txt);
}

void FileServerDlg::OnBnClickedBtnBrowseDir()
{
	CString cstr;
	if (whBrowsePath(m_hWnd, cstr))
		SetDlgItemText(IDC_EDIT_FILE_SRV_PATH, cstr);
}

void FileServerDlg::OnBnClickedBtnShowHide()
{
	if (editVisitCode_.GetPasswordChar() == 0)
	{
		editVisitCode_.SetPasswordChar('*');
		SetDlgItemText(IDC_BTN_SHOW_HIDE, _T("Show"));
	}
	else
	{
		editVisitCode_.SetPasswordChar(0);
		SetDlgItemText(IDC_BTN_SHOW_HIDE, _T("Hide"));
	}
	editVisitCode_.RedrawWindow(NULL, NULL);
}
