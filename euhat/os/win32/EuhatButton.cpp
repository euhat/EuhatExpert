#include <EuhatPreDef.h>
#include <Windows.h>
#include "EuhatButton.h"
#include "EuhatBitmap.h"
#include <EuhatPostDef.h>

using namespace euhatwin;

EuhatButton::EuhatButton(HWND parentHwnd, EuhatButtonUser *user)
	: EuhatChildWnd(parentHwnd)
{
	user_ = user;
}

EuhatButton::~EuhatButton()
{
	delete bmpNormal_;
	delete bmpHighlighted_;
	delete bmpDown_;
}

LRESULT EuhatButton::handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return EuhatChildWnd::handleMsg(uMsg, wParam, lParam);
}

void EuhatButton::onLBtnDown(WPARAM wParam, int x, int y)
{
	bmpToDraw_ = bmpDown_;
	InvalidateRect(hwnd_, NULL, 1);
}

void EuhatButton::onMouseMove(WPARAM wParam, int x, int y)
{
	if (bmpToDraw_ != bmpDown_) {
		EuhatRect rc;
		GetClientRect(hwnd_, &rc.rc_);
		if (rc.isIn(x, y)) {
			if (bmpToDraw_ != bmpHighlighted_) {
				bmpToDraw_ = bmpHighlighted_;
				InvalidateRect(hwnd_, NULL, 1);
			}
			SetCapture(hwnd_);
		} else {
			ReleaseCapture();
			if (bmpToDraw_ != bmpNormal_) {
				bmpToDraw_ = bmpNormal_;
				InvalidateRect(hwnd_, NULL, 1);
			}
		}
	}
}

void EuhatButton::onLBtnUp(WPARAM wParam, int x, int y)
{
	EuhatRect rc;
	GetClientRect(hwnd_, &rc.rc_);
	if (rc.isIn(x, y)) {
		bmpToDraw_ = bmpNormal_;
		user_->onBtnClick(this);
	} else {
		bmpToDraw_ = bmpNormal_;
	}
	InvalidateRect(hwnd_, NULL, 1);
}

void EuhatButton::onDraw(HDC hdc)
{
	RECT rc;
	GetClientRect(hwnd_, &rc);
	bmpToDraw_->draw(hdc, 0, 0, rc.right, rc.bottom);
}

void EuhatButton::setSkin(const char *folder, const char *name)
{
	char path[1024];
	sprintf(path, "%s/%s/%s_%s.jpg", EUHAT_BUTTON_SKIN_PATH, folder, "N", name);
	bmpNormal_ = new EuhatBitmap;
	bmpNormal_->init(path);
	sprintf(path, "%s/%s/%s_%s.jpg", EUHAT_BUTTON_SKIN_PATH, folder, "H", name);
	bmpHighlighted_ = new EuhatBitmap;
	bmpHighlighted_->init(path);
	sprintf(path, "%s/%s/%s_%s.jpg", EUHAT_BUTTON_SKIN_PATH, folder, "D", name);
	bmpDown_ = new EuhatBitmap;
	bmpDown_->init(path);

	bmpToDraw_ = bmpNormal_;

	EuhatRect rc;
	GetClientRect(hwnd_, &rc.rc_);

	POINT pt = { 0, 0 };
	ScreenToClient(hwnd_, &pt);
	
	rc.move(pt.x, pt.y);
	rc.setWidthHeight(bmpNormal_->getWidth(), bmpNormal_->getHeight());

	moveWindow(&rc.rc_);
}
