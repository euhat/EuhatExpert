#pragma once

#include <common/OpCommon.h>

template<class T>
class JyList
{
public:
	using iterator = typename list<T>::iterator;
	JyList()
	{
		split_ = ',';
	}
	int load()
	{
		list_.clear();

		list<string> lines;
		if (!readLinesFromFile(filePath_.c_str(), lines))
			return 0;

		for (list<string>::iterator it = lines.begin(); it != lines.end(); it++)
		{
			vector<string> cells;
			splitTokenString(it->c_str(), split_, cells);
			
			T t;
			if (!t.load(cells))
				continue;
			list_.push_back(t);
		}
		return 1;
	}
	int save()
	{
		FILE *fp = fopen(filePath_.c_str(), "w+");
		if (NULL == fp)
			return 0;

		for (list<T>::iterator it = list_.begin(); it != list_.end(); it++)
		{
			it->save(fp, split_);
			fputc('\n', fp);
		}

		fclose(fp);
		return 1;
	}
	list<T> list_;
	string filePath_;
	char split_;
};