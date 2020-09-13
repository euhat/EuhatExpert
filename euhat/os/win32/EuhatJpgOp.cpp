#include <EuhatPreDef.h>
#include <stdio.h>
#include <setjmp.h>
#include <windows.h>
extern "C" {
#include "libjpeg/jpeglib.h" 
#include "libjpeg/jerror.h"
}
#include <EuhatPostDef.h>

#pragma comment(lib, "jpeg.lib")

struct my_error_mgr {
	struct jpeg_error_mgr pub;

	jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr)cinfo->err;

	(*cinfo->err->output_message) (cinfo);

	longjmp(myerr->setjmp_buffer, 1);
}

HBITMAP readJpegAsBitmap(const char *fileName)
{
	BITMAP bmp;
	BITMAPINFO bi = {};
	LPBYTE lpBuf, pb = NULL;
	HBITMAP hBmp = NULL;
	JSAMPARRAY buffer{};
	int row_stride;
	UINT row;

	FILE *fp = fopen(fileName, "rb");
	if (fp == NULL)
	{
		return NULL;
	}

	unsigned char sign[4];
	fread(sign, 1, 4, fp);
	if (sign[0] != 0xff || sign[1] != 0xd8)
	{
		fclose(fp);
		return NULL;
	}
	fseek(fp, 0, SEEK_SET);

	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer))
	{
		goto failout;
	}

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, fp);

	jpeg_read_header(&cinfo, TRUE);

	jpeg_start_decompress(&cinfo);

	//row = ((cinfo.output_width * 3 + 3) & ~3);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = cinfo.output_width;
	bi.bmiHeader.biHeight = cinfo.output_height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
//	bi.bmiHeader.biSizeImage = row * cinfo.output_height;

	hBmp = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&lpBuf, NULL, 0);
	if (hBmp == NULL)
		goto failout;

	GetObject(hBmp, sizeof(BITMAP), (void *)&bmp);
	row = bmp.bmWidthBytes;

	pb = lpBuf + row * cinfo.output_height;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		pb -= row;
		jpeg_read_scanlines(&cinfo, buffer, 1);

		if (cinfo.out_color_components == 1)
		{
			LPBYTE p = (LPBYTE)buffer[0];
			for (UINT i = 0; i < cinfo.output_width; i++)
			{
				pb[3 * i + 0] = p[i];
				pb[3 * i + 1] = p[i];
				pb[3 * i + 2] = p[i];
			}
		}
		else if (cinfo.out_color_components == 3)
		{
			LPBYTE p = (LPBYTE)buffer[0];
			for (UINT i = 0; i < cinfo.output_width; i++)
			{
				UINT j = i * 3;
				pb[j + 0] = p[j + 2]; // Blue
				pb[j + 1] = p[j + 1]; // Green
				pb[j + 2] = p[j + 0];  // Red
			}
		}
		else
		{
			goto failout;
		}
	}

	SetDIBits(NULL, hBmp, 0, cinfo.output_height, lpBuf, &bi, DIB_RGB_COLORS);

	jpeg_finish_decompress(&cinfo);

failout:

	jpeg_destroy_decompress(&cinfo);

	fclose(fp);

	return hBmp;
}