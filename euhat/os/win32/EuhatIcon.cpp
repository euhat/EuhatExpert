#include <EuhatPreDef.h>
#include <Windows.h>
#include <common/OpCommon.h>
#include "EuhatIcon.h"
#include <OpCommonOs.h>
#include <EuhatPostDef.h>

EuhatIcon::EuhatIcon()
{
	icon_ = NULL;
}

EuhatIcon::EuhatIcon(int id)
{
	HINSTANCE hInst = GetModuleHandle(0);
	//icon_ = (HICON)LoadIcon(hInst, MAKEINTRESOURCE(id));
	icon_ = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(id), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_CREATEDIBSECTION | LR_DEFAULTCOLOR);
}

EuhatIcon::EuhatIcon(const wchar_t *fileName)
{
	icon_ = (HICON)::LoadImage(NULL, fileName, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
}

EuhatIcon::EuhatIcon(HICON icon)
{
	icon_ = icon;
}

EuhatIcon::~EuhatIcon()
{
	DestroyIcon(icon_);
}

void EuhatIcon::draw(HICON icon, HDC hdc, int x, int y, int width, int height)
{
/*	BITMAP bitmap;
	ICONINFO iconInfo;
	GetIconInfo(icon_, &iconInfo);
	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bitmap);
	int widthT = bitmap.bmWidth;
	int heightT = bitmap.bmHeight;
	DeleteObject(iconInfo.hbmColor);
	DeleteObject(iconInfo.hbmMask);
*/
	//DrawIcon(hdc, x, y, icon_);
	width = 0;
	height = 0;
	DrawIconEx(hdc, x, y, icon, width, height, 0, NULL, DI_NORMAL);
}

void EuhatIcon::draw(HDC hdc, int x, int y, int width, int height)
{
	draw(icon_, hdc, x, y, width, height);
}

EuhatIconSysPool::EuhatIconSysPool(const char *defaultExt, int maxCount, int useSmall)
{
	defaultExt_ = defaultExt;
	maxCount_ = maxCount;
	useSmall_ = useSmall;
	pickFile("", defaultExt);

	int flags = useSmall_ ? SHGFI_SMALLICON : SHGFI_LARGEICON;
	HICON normal = whFolderIcon(flags);
	HICON selected = whFolderIcon(flags | SHGFI_SELECTED);
	folderBasket_.normal_.icon_ = normal;
	folderBasket_.selected_.icon_ = selected;
}

EuhatIconSysPool::Basket *EuhatIconSysPool::pickFile(const char *filePath, const char *extension)
{
	string ext = strToLower(extension);
	string path = strToLower(filePath);
	if (path.size() > 1 && path[1] == ':')
	{
		if (ext == ".exe")
		{

		}
		else if (ext == ".lnk")
		{

		}
	}
	auto it = pool_.find(ext);
	if (it != pool_.end())
		return &it->second;
	if (maxCount_ > 0)
	{
		if (pool_.size() >= (size_t)maxCount_)
			return pickFile("", defaultExt_.c_str());
	}
	int flags = useSmall_ ? SHGFI_SMALLICON : SHGFI_LARGEICON;
	HICON normal = whFileIcon(ext.c_str(), flags);
	HICON selected = whFileIcon(ext.c_str(), flags | SHGFI_SELECTED);
	Basket &basket = pool_[ext];
	basket.normal_.icon_ = normal;
	basket.selected_.icon_ = selected;
	return &basket;
}
