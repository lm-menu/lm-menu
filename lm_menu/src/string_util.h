#pragma once
#include "stdafx.h"

namespace string_util
{
	wstring A2W(string &str, int nCodePage);
	string W2A(wstring &str, int nCodePage);

	LPWSTR A2W(LPSTR str, int nCodePage, int nLength = -1);

	wstring utf8_to_unicode(string &str);
	wstring ansi_to_unicode(string &str);
	string unicode_to_utf8(wstring str);
	string unicode_to_ansi(wstring str);
	void unescape_simple(wstring &str);
}

#define STR string_util