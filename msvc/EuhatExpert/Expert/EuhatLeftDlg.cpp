#include "stdafx.h"
#include "Expert.h"
#include "ExpertDlg.h"
#include "EuhatLeftDlg.h"
#include "afxdialogex.h"
#include "local/EuhatLocalDlg.h"
#include "peer/EuhatPeerDlg.h"
#include <OpWin.h>
#include <EuhatTabCtrl.h>
#include <dbop/DbOpSqliteTable.h>
#include <EuhatPostDefMfc.h>

#define CELL_TO_MEMBER_LOOP \
	CELL_TO_MEMBER(rowid, DbOpColumnTypeInt64) \
	CELL_TO_MEMBER(peerName, DbOpColumnTypeText)

EuhatLeftItemTable::EuhatLeftItemTable(DbOpSqlite *db)
{
	tab_.reset(new DbOpSqliteTable());
	tab_->db_ = db;
	tab_->tableName_ = "LeftItem";
#define CELL_TO_MEMBER(_name, _type) tab_->cols_.push_back({ #_name, _type });
	CELL_TO_MEMBER_LOOP
#undef CELL_TO_MEMBER

	tab_->create();
}

EuhatLeftDlg::EuhatLeftDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_LEFT_DIALOG, euhatBase),
	  tab_(euhatBase->db_.get())
{
}

EuhatLeftDlg::~EuhatLeftDlg()
{
}

void EuhatLeftDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PEERS, peers_);
}

BEGIN_MESSAGE_MAP(EuhatLeftDlg, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST_PEERS, &EuhatLeftDlg::OnSelchangeListPeers)
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL EuhatLeftDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	EuhatLocalDlg *localDlg = new EuhatLocalDlg(euhatBase_);
	localDlg->create();
	peers_.insertStr(peers_.GetCount(), _T("Local"), localDlg);

	unnameStr_ = "<Unnamed>";

	list<vector<DbOpCell>> rows;
	tab_.tab_->select("", rows);
	for (auto it = rows.begin(); it != rows.end(); it++)
	{
		vector<DbOpCell> &row = *it;
		if (row.size() < 2)
			continue;
		addItem(row[1].buf_.get());
	}

	euTabs_.reset(new EuhatTabCtrl(m_hWnd, this));
	euTabs_->kUnselText_ = RGB(0, 0, 0);
	euTabs_->kSelText_ = RGB(255, 255, 255);
	euTabs_->brushBack_.reset(new EuhatBrush(GetSysColor(COLOR_BTNFACE)));
	euTabs_->brushBackSel_.reset(new EuhatBrush(RGB(0, 0, 0)));
	euTabs_->penBack_.reset(new EuhatPen(RGB(0, 0, 155)));
	euTabs_->penBackSel_.reset(new EuhatPen(RGB(255, 255, 255)));
	euTabs_->add("Add");
	euTabs_->add("Del");

	peers_.SetCurSel(0);
	OnSelchangeListPeers();

	return TRUE;
}

int EuhatLeftDlg::getItem(EuhatRightDlg *obj)
{
	for (int i = peers_.GetCount(); i--; ) 
	{
		EuhatRightDlg *peer = (EuhatRightDlg *)peers_.getItem(i)->userData_;
		if (peer == obj)
			return i;
	}
	return -1;
}

int EuhatLeftDlg::getItem(const char *name)
{
	for (int i = peers_.GetCount(); i--; )
	{
		if (peers_.getItem(i)->str_ == name)
			return i;
	}
	return -1;
}

void EuhatLeftDlg::modifyItem(EuhatRightDlg *obj, const char *newName)
{
	JyListBox::Item *old = peers_.getItem(getItem(obj));
	string oldName = CT2A(old->str_);

	string sql = "delete from " + tab_.tab_->tableName_ + " where " + tab_.tab_->cols_[1].name_ + " = '" + oldName + "';";
	tab_.tab_->db_->insert(sql.c_str());

	vector<DbOpCell> row;
	row.push_back({ NULL, 0 });
	row.push_back(DbOpCell()); row.back().buf_.reset(opStrDup(newName));

	tab_.tab_->insert(row);

	old->str_ = CA2T(newName);

	peers_.Invalidate();
}

void EuhatLeftDlg::addItem(const char *name)
{
	CString cstr = CA2T(name);
	EuhatPeerDlg *peerDlg = new EuhatPeerDlg(euhatBase_);
	peers_.insertStr(peers_.GetCount(), cstr, peerDlg);
	peerDlg->leftDlg_ = this;
	peerDlg->create();

	peerDlg->correctPos(rcPeer_);
}

void EuhatLeftDlg::delItem(int idx)
{
	if (idx <= 0)
		return;
	JyListBox::Item *item = peers_.getItem(idx);

	string peerName = CT2A(item->str_);
	peerName = gbToUtf8(peerName.c_str());
	string sql = "delete from " + tab_.tab_->tableName_ + " where " + tab_.tab_->cols_[1].name_ + " = '" + peerName + "';";
	tab_.tab_->db_->insert(sql.c_str());

	EuhatRightDlg *peer = (EuhatRightDlg *)item->userData_;
	delete peer;
	delete item;
	peers_.DeleteString(idx);
}

void EuhatLeftDlg::correctPos(CRect &rc)
{
	CRect rcLeft = rc;
	rcLeft.right = EU_EXPERT_DLG_LEFT_WIDTH;
	rcLeft.top = EU_EXPERT_DLG_TITLE_HEIGHT + EU_EXPERT_DLG_MARGIN;
	MoveWindow(&rcLeft);

	rcPeer_ = *rc;
	rcPeer_.left = rcLeft.right;
	rcPeer_.top = rcLeft.top;

	CRect rcList = rcLeft;
	rcList.MoveToXY(0, 0);
	rcList.right -= EU_EXPERT_DLG_MARGIN;
	rcList.bottom -= EU_EXPERT_TOOLBAR_HEIGHT + EU_EXPERT_DLG_MARGIN;
	peers_.MoveWindow(rcList);

	CRect rcTab = rcList;
	rcTab.top = rcList.bottom;
	rcTab.bottom = rcTab.top + EU_EXPERT_TOOLBAR_HEIGHT;
	euTabs_->moveWindow(rcTab);

	for (int i = peers_.GetCount(); i--; ) {
		EuhatRightDlg *peer = (EuhatRightDlg *)peers_.getItem(i)->userData_;
		if (peer->isVisible_)
			peer->correctPos(rcPeer_);
	}
}

void EuhatLeftDlg::OnSelchangeListPeers()
{
	int curIdx = peers_.GetCurSel();

	for (int i = peers_.GetCount(); i--; ) {
		EuhatRightDlg *peer = (EuhatRightDlg *)peers_.getItem(i)->userData_;
		if (curIdx == i) {
			peer->show();
			peer->correctPos(rcPeer_);
		} else {
			peer->hide();
		}
	}
}

void EuhatLeftDlg::OnDestroy()
{
	MfcBasePage::OnDestroy();

	for (int i = peers_.GetCount(); i--; ) {
		EuhatRightDlg *peer = (EuhatRightDlg *)peers_.getItem(i)->userData_;
		delete peer;
		peers_.getItem(i)->userData_ = NULL;
	}
}

void EuhatLeftDlg::OnPaint()
{
	CPaintDC dc(this);

	CRect rc;
	GetClientRect(&rc);

	rc.left = rc.right - EU_EXPERT_DLG_MARGIN;
	rc.right -= EU_EXPERT_DLG_MARGIN - 2;

	fillGradientRect(dc.m_hDC, rc, RGB(170, 170, 255), RGB(240, 240, 255), FILL_STYLE_HORIZONTAL);
}

void EuhatLeftDlg::onClick(EuhatTabCtrl &ctrl)
{
	int curSel = -1;
	if (ctrl.getCurSel() == 0)
	{
		curSel = getItem(unnameStr_.c_str());
		if (curSel < 0)
		{
			addItem(unnameStr_.c_str());
			curSel = getItem(unnameStr_.c_str());
		}
	}
	else if (ctrl.getCurSel() == 1)
	{
		curSel = peers_.GetCurSel();
		if (curSel > 0)
		{
			delItem(curSel);
			curSel = 0;
		}
	}
	peers_.SetCurSel(curSel);
	OnSelchangeListPeers();
	ctrl.setCurSel(-1);
}