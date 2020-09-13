#include <EuhatPreDef.h>
#include <Windows.h>
#include "EuhatBitmap.h"
#include <common/OpCommon.h>
#include "EuhatJpgOp.h"
#include <EuhatPostDef.h>

EuhatBitmap::EuhatBitmap()
{
	hdc_ = NULL;
	bmp_ = NULL;
}

EuhatBitmap::~EuhatBitmap()
{
	fini();
}

HBITMAP EuhatBitmap::loadJpegAsBitmap(const char *fileName)
{
	return readJpegAsBitmap(fileName);
}

int EuhatBitmap::init(const char *path)
{
	bmp_ = loadJpegAsBitmap(path);

	hdc_ = CreateCompatibleDC(NULL);
	SelectObject(hdc_, (HGDIOBJ)bmp_);

	return 1;
}

void EuhatBitmap::fini()
{
	DeleteObject(bmp_);
	bmp_ = NULL;

	DeleteDC(hdc_);
	hdc_ = NULL;
}

int EuhatBitmap::getWidth()
{
	BITMAP bm;
	GetObject(bmp_, sizeof(BITMAP), (void *)&bm);
	return bm.bmWidth;
}

int EuhatBitmap::getHeight()
{
	BITMAP bm;
	GetObject(bmp_, sizeof(BITMAP), (void *)&bm);
	return bm.bmHeight;
}

void EuhatBitmap::draw(HDC hdc, int x, int y, int width, int height)
{
	SetStretchBltMode(hdc, COLORONCOLOR);
	BitBlt(hdc, x, y, width, height, hdc_, 0, 0, SRCCOPY);
}