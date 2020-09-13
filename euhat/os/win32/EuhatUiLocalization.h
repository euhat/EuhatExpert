#pragma once

namespace tinyxml2
{
	class XMLDocument;
	class XMLElement;
}

class EuhatUiLocalization
{
	tinyxml2::XMLElement *gotoFirstGrandChildNode(const char *xmlPath);
	const char *getAttr(tinyxml2::XMLElement *elmt, const char *attr);
	int getAttrInt(tinyxml2::XMLElement *elmt, const char *attr);

	string xmlPath_;
	tinyxml2::XMLDocument *docXml;

public:
	EuhatUiLocalization();
	~EuhatUiLocalization();

	int init(const char *xmlPath);
	int fini();

	int modDlg(HWND hwnd, vector<pair<string, string> > &locText, const char *xmlTag);
};