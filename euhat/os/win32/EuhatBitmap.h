#pragma once

class EuhatBitmap
{
	HBITMAP bmp_;

public:
	EuhatBitmap();
	~EuhatBitmap();

	HBITMAP loadJpegAsBitmap(const char *fileName);

	int init(const char *path);
	void fini();
	int getWidth();
	int getHeight();
	void draw(HDC hdc, int x, int y, int width, int height);

	HDC hdc_;
};