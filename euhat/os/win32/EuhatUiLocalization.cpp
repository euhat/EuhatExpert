#include <EuhatPreDef.h>
#include <windows.h>
#include <common/OpCommon.h>
#include "EuhatUiLocalization.h"
#include <xml/tinyxml2.h>
#include <EuhatPostDef.h>

using namespace tinyxml2;

EuhatUiLocalization::EuhatUiLocalization()
{
	docXml = new tinyxml2::XMLDocument;
}

EuhatUiLocalization::~EuhatUiLocalization()
{
	delete docXml;
}

int EuhatUiLocalization::init(const char *xmlPath)
{
	tinyxml2::XMLError errXml = docXml->LoadFile(xmlPath);
	if (tinyxml2::XML_SUCCESS != errXml)
	{
		DBG(("can't load xml file [%s]\n", xmlPath));
		return 0;
	}
	xmlPath_ = xmlPath;
	return 1;
}

int EuhatUiLocalization::fini()
{
	return 1;
}

tinyxml2::XMLElement *EuhatUiLocalization::gotoFirstGrandChildNode(const char *xmlPath)
{
	tinyxml2::XMLElement *elmtRoot = docXml->RootElement();
	tinyxml2::XMLElement *elmtCur = elmtRoot;

	vector<string> tags;
	splitTokenString(xmlPath, '/', tags);

	for (vector<string>::iterator it = tags.begin(); it != tags.end(); it++)
	{
		elmtCur = elmtCur->FirstChildElement(it->c_str());
		if (elmtCur == NULL)
		{
			DBG(("route to [%s] in [%s] failed.\n", xmlPath, xmlPath_.c_str()));
			return NULL;
		}
	}
	return elmtCur;
}

const char *EuhatUiLocalization::getAttr(tinyxml2::XMLElement *elmt, const char *attr)
{
	const char *v = elmt->Attribute(attr);
	if (NULL == v)
	{
		DBG(("[%s] of [%s] in xml [%s] not exist.\n", attr, elmt->Name(), xmlPath_.c_str()));
	}
	return v;
}

int EuhatUiLocalization::getAttrInt(tinyxml2::XMLElement *elmt, const char *attr)
{
	const char *v = getAttr(elmt, attr);
	if (NULL == v)
		return 0;
	return atoi(v);
}

int EuhatUiLocalization::modDlg(HWND hwnd, vector<pair<string, string> > &locText, const char *xmlTag)
{
	tinyxml2::XMLElement *elmtDlg = gotoFirstGrandChildNode(xmlTag);
	if (NULL == elmtDlg)
		return 0;

	const char *title = elmtDlg->Attribute("title");
	::SetWindowText(hwnd, utf8ToWstr(title).c_str());

	tinyxml2::XMLElement *elmtCur = elmtDlg->FirstChildElement("Item");
	while (elmtCur != NULL)
	{
		int id = getAttrInt(elmtCur, "id");
		const char *name = getAttr(elmtCur, "name");

		SetDlgItemText(hwnd, id, utf8ToWstr(name).c_str());

		elmtCur = elmtCur->NextSiblingElement("Item");
	}

	int idx = 0;
	elmtCur = elmtDlg->FirstChildElement("Text");
	while (elmtCur != NULL && idx < (int)locText.size())
	{
		locText[idx].first = getAttr(elmtCur, "id");
		locText[idx++].second = getAttr(elmtCur, "name");

		elmtCur = elmtCur->NextSiblingElement("Text");
	}
	return 1;
}