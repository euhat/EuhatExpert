#pragma once
#include <EuhatBackWnd.h>
#include "../../base/EuhatWtPayload.h"

#define EUHAT_CHART_X_AXIS_HEIGHT 400
#define EUHAT_CHART_MARK_HEIGHT 50
#define EUHAT_CHART_WIDTH EUHAT_WT_PAYLOAD_COUNT

class WorkTraceChart : public EuhatBackWnd
{
	LRESULT handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void onMouseMove(WPARAM wParam, int x, int y);

	void onDraw(HDC hdc);

	EuhatWtPayload payload_;
	int x_;
	int y_;

public:
	WorkTraceChart(HWND parentHwnd, EuhatBase *euhatBase);
	virtual ~WorkTraceChart();

	int read(const char *filePath);

	unique_ptr<EuhatPen> penNoRunning_;
	unique_ptr<EuhatPen> penNoMove_;
	unique_ptr<EuhatPen> penHangUp_;
	unique_ptr<EuhatPen> penMouse_;
	unique_ptr<EuhatPen> penKeyboard_;
	unique_ptr<EuhatPen> penScaler_;
};