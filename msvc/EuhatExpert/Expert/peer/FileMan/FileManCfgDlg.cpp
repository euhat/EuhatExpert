#include "stdafx.h"
#include "../../Expert.h"
#include "FileManCfgDlg.h"
#include "FileManDlg.h"
#include "EuhatCertificateDlg.h"
#include <EuhatTabCtrl.h>
#include <dbop/DbOpIni.h>
#include <app/FileMan/client/FileManClient.h>
#include <app/FileMan/common/FileManType.h>
#include <common/JyTcpSelector.h>
#include <EuhatPostDefMfc.h>

#define INI_SECTION_FILE_MAN "fileMan_"

FileManCfgDlg::FileManCfgDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_FILE_MAN_CFG_DIALOG, euhatBase)
{
	certDlg_.reset(new EuhatCertificateDlg(euhatBase_));
}

FileManCfgDlg::~FileManCfgDlg()
{

}

void FileManCfgDlg::DoDataExchange(CDataExchange *pDX)
{
	MfcBasePage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(FileManCfgDlg, MfcBasePage)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_GO, &FileManCfgDlg::OnBnClickedBtnGo)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &FileManCfgDlg::OnBnClickedBtnCancel)
END_MESSAGE_MAP()

void FileManCfgDlg::correctPos()
{
	if (NULL == GetDlgItem(IDC_STATIC_CERT_DLG))
		return;

	CRect rcCert;
	GetDlgItem(IDC_STATIC_CERT_DLG)->GetWindowRect(&rcCert);
	ScreenToClient(&rcCert);
	certDlg_->MoveWindow(rcCert);
}

void FileManCfgDlg::loadData()
{
	string section = INI_SECTION_FILE_MAN + peerName_;

	ip_ = euhatBase_->ini_->readStr(section.c_str(), "ip", "");

	port_ = euhatBase_->ini_->readInt(section.c_str(), "port", FILE_MAN_DEFAULT_PORT);

	visitCode_ = euhatBase_->ini_->readStr(section.c_str(), "visitCode");
}

void FileManCfgDlg::dataToUi()
{
	string section = INI_SECTION_FILE_MAN + peerName_;

	CString cstr = CA2T(peerName_.c_str());
	SetDlgItemText(IDC_EDIT_PEER_NAME, cstr);

	cstr = CA2T(ip_.c_str());
	SetDlgItemText(IDC_IPADDRESS_FILE_SERVER, cstr);

	SetDlgItemInt(IDC_EDIT_PORT, port_);

	cstr = CA2T(visitCode_.c_str());
	SetDlgItemText(IDC_EDIT_VISIT_CODE, cstr);
}

int FileManCfgDlg::uiToData()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_PEER_NAME, cstr);
	string peerName = CT2A(cstr);

	GetDlgItemText(IDC_IPADDRESS_FILE_SERVER, cstr);
	ip_ = CT2A(cstr);

	port_ = GetDlgItemInt(IDC_EDIT_PORT);

	GetDlgItemText(IDC_EDIT_VISIT_CODE, cstr);
	visitCode_ = CT2A(cstr);

	return 1;
}

BOOL FileManCfgDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	certDlg_->certName_ = JY_TCP_INI_CLIENT_CERT_KEY;
	certDlg_->create(this);

	loadData();

	dataToUi();

	correctPos();

	return TRUE;
}

void FileManCfgDlg::OnSize(UINT nType, int cx, int cy)
{
	MfcBasePage::OnSize(nType, cx, cy);

	correctPos();
}

void FileManCfgDlg::OnBnClickedBtnGo()
{
	if (!certDlg_->isGenerated())
	{
		MessageBox(_T("Certificate is not generated, can't start."), _T("Error"), 0);
		return;
	}

	uiToData();

	parent_->dataToEngine();
}

void FileManCfgDlg::saveData(const char *peerName)
{
	if (peerName_ != peerName)
	{
		string sql = "delete from " + euhatBase_->ini_->tab_->tableName_ + " where section = '" + INI_SECTION_FILE_MAN + peerName_ + "';";
		euhatBase_->db_->insert(sql.c_str());
	}

	peerName_ = peerName;
	string section = INI_SECTION_FILE_MAN + peerName_;

	euhatBase_->ini_->write(section.c_str(), "ip", ip_.c_str());
	euhatBase_->ini_->write(section.c_str(), "port", port_);
	euhatBase_->ini_->write(section.c_str(), "visitCode", visitCode_.c_str());
}

void FileManCfgDlg::OnBnClickedBtnCancel()
{
	ShowWindow(SW_HIDE);
}
