#pragma once

class EuhatIcon
{
public:
	EuhatIcon();
	EuhatIcon(int id);
	EuhatIcon(const wchar_t *fileName);
	EuhatIcon(HICON icon);
	~EuhatIcon();

	void draw(HDC hdc, int x, int y, int width = 0, int height = 0);
	static void draw(HICON icon, HDC hdc, int x, int y, int width = 0, int height = 0);

	HICON icon_;
};

class EuhatIconSysPool
{
public:
	class Basket
	{
	public:
		EuhatIcon normal_;
		EuhatIcon selected_;
	};
	EuhatIconSysPool(const char *defaultExt = ".dat", int maxCount = -1, int useSmall = 1);
	Basket *pickFile(const char *path, const char *extension);

	map<string, Basket> pool_;
	Basket folderBasket_;
	string defaultExt_;
	int maxCount_;
	int useSmall_;
};