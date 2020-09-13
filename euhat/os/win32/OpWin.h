#pragma once

int mySystemNoWait(const char *cmdLine, int showIt = SW_HIDE);
int mySystem(const char *cmdLine, int showIt = SW_HIDE, DWORD timeOut = INFINITE);

#define FILL_STYLE_VERTICAL	0
#define FILL_STYLE_HORIZONTAL	1

void fillGradientRect(HDC hdc, RECT *rc, COLORREF fromRGB, COLORREF toRGB, int fillStyle);