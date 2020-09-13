#include "stdafx.h"
#include "../../Expert.h"
#include "EuhatCertificateDlg.h"
#include <app/FileMan/common/FileManFileOp.h>
#include <common/JyTcpSelector.h>
#include <EuhatPostDefMfc.h>

EuhatCertificateDlg::EuhatCertificateDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_CERTIFICATE_DIALOG, euhatBase)
{

}

EuhatCertificateDlg::~EuhatCertificateDlg()
{

}

void EuhatCertificateDlg::DoDataExchange(CDataExchange *pDX)
{
	MfcBasePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CERT_LENGTH, comboCertLen_);
}

BEGIN_MESSAGE_MAP(EuhatCertificateDlg, MfcBasePage)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_GENERATE, &EuhatCertificateDlg::OnBnClickedBtnGenerate)
	ON_CBN_SELCHANGE(IDC_COMBO_CERT_LENGTH, &EuhatCertificateDlg::OnSelchangeComboCertLength)
END_MESSAGE_MAP()

void EuhatCertificateDlg::correctPos()
{
	if (NULL == GetDlgItem(IDC_BTN_GENERATE))
		return;

}

BOOL EuhatCertificateDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	comboCertLen_.InsertString(comboCertLen_.GetCount(), _T("256 bits"));
	comboCertLen_.InsertString(comboCertLen_.GetCount(), _T("512 bits"));
	comboCertLen_.InsertString(comboCertLen_.GetCount(), _T("1024 bits"));
	comboCertLen_.InsertString(comboCertLen_.GetCount(), _T("2048 bits"));
	comboCertLen_.SetCurSel(2);

	updateResultMsg();

	correctPos();

	return TRUE;
}

void EuhatCertificateDlg::OnSize(UINT nType, int cx, int cy)
{
	MfcBasePage::OnSize(nType, cx, cy);

	correctPos();
}

void EuhatCertificateDlg::OnBnClickedBtnGenerate()
{
	int curSel = comboCertLen_.GetCurSel();
	int bits;
	switch (curSel)
	{
	case 0: bits = 256; break;
	case 1: bits = 512; break;
	case 2: bits = 1024; break;
	default: bits = 2048; break;
	}
	string result = "Generating...";
	CString cstr = CA2T(result.c_str());
	SetDlgItemText(IDC_STATIC_GENERATE_RESULT, cstr);

	fmGenAndWriteCert(*euhatBase_->ini_.get(), certName_.c_str(), bits);

	result = "Successful!";
	cstr = CA2T(result.c_str());
	SetDlgItemText(IDC_STATIC_GENERATE_RESULT, cstr);

	updateResultMsg();
}

void EuhatCertificateDlg::updateResultMsg()
{
	int curSel = comboCertLen_.GetCurSel();
	string aboutBits;
	switch (curSel)
	{
	case 0: aboutBits = "It is very fast to generate, but security is very poor."; break;
	case 1: aboutBits = "It is fast to generate, but security is poor."; break;
	case 2: aboutBits = "It needs several seconds to generate, security is ok."; break;
	default: aboutBits = "It needs 10 minutes at least to generate in i5 cpu, but security is very good."; break;
	}
	CString cstr;
	cstr = CA2T(aboutBits.c_str());
	SetDlgItemText(IDC_STATIC_ABOUT_BITS, cstr);

	string key = certName_ + "_lastGenDate";
	string val = euhatBase_->ini_->readStr(JY_TCP_INI_CERT_SECTION, key.c_str());
	key = certName_ + +"_e";
	JyBuf buf;
	euhatBase_->ini_->readBuf(JY_TCP_INI_CERT_SECTION, key.c_str(), buf);
	string msg;
	if (val.length() > 0)
		msg = "Last generated date: " + val + ", length: " + intToString(buf.size_ * 8) + " bits.";
	else
		msg = "Certificate is empty, please generate first.";
	cstr = CA2T(msg.c_str());
	SetDlgItemText(IDC_STATIC_LAST_GEN_DATE, cstr);
}

int EuhatCertificateDlg::isGenerated()
{
	string key = certName_ + "_lastGenDate";
	string val = euhatBase_->ini_->readStr(JY_TCP_INI_CERT_SECTION, key.c_str());
	if (val.length() <= 0)
	{
		return 0;
	}
	return 1;
}

void EuhatCertificateDlg::OnSelchangeComboCertLength()
{
	updateResultMsg();
}
