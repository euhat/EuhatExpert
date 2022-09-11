#include <EuhatPreDef.h>
#include "EuhatPath.h"
#include <EuhatPostDef.h>

EuhatPath::EuhatPath()
{
	isUnix_ = 0;
	isAbsPath_ = 0;
}

void EuhatPath::goSub(const char *fileName)
{
	path_.push_back(move(string(fileName)));
}

void EuhatPath::goUp()
{
	if (path_.size() == 0)
		return;

	path_.pop_back();
}

void EuhatPath::inStr(const char *path)
{
	path_.clear();

	unique_ptr<char[]> buf(opStrDup(path));
	strReplaceChar(buf.get(), '\\', '/');

	isAbsPath_ = 0;
	size_t len = strlen(buf.get());
	if (len > 1)
	{
		if (buf.get()[0] == '/')
			isAbsPath_ = 1;
	}

	vector<string> out;
	splitTokenString(buf.get(), '/', out);

	for (auto it = out.begin(); it != out.end(); it++)
		if (!it->empty())
			path_.push_back(move(*it));
}

string EuhatPath::toStr()
{
	char split = '\\';
	ostringstream ss;
	if (isUnix_)
	{
		split = '/';
		if (isAbsPath_)
		{
			ss << split;
		}
	}
	for (auto it = path_.begin(); it != path_.end(); it++)
	{
		if (it != path_.begin())
			ss << split;
		ss << it->c_str();
	}
	return ss.str();
}
