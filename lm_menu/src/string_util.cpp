#include "stdafx.h"
#include "string_util.h"
#include "app_config.h"

wstring string_util::A2W(string &str, int nCodePage)
{
	if (str.empty()) return wstring();

	auto nSize = int(str.size());
	auto pString = str.data();

	size_t charsNeeded = MultiByteToWideChar(nCodePage, 0, pString, nSize, nullptr, 0);
	if (charsNeeded == 0) return wstring();

	std::vector<wchar_t> buffer(charsNeeded);
	int charsConverted = MultiByteToWideChar(nCodePage, 0, pString, nSize, &buffer[0], charsNeeded);
	if (charsConverted == 0) return wstring();

	return wstring(&buffer[0], charsConverted);
}

LPWSTR string_util::A2W(LPSTR str, int nCodePage, int nLength)
{
	if (str == nullptr) return nullptr;

	size_t charsNeeded = MultiByteToWideChar(nCodePage, 0, str, -1, nullptr, 0);
	if (charsNeeded == 0) return nullptr;

	auto lpUnicode = new wchar_t[charsNeeded + 1];
	charsNeeded = MultiByteToWideChar(nCodePage, 0, str, -1, lpUnicode, charsNeeded);
	if (charsNeeded == 0) return nullptr;

	return lpUnicode;
}

string string_util::W2A(wstring &str, int nCodePage)
{
	if (str.empty()) return string();

	auto nSize = int(str.size());
	auto pString = str.data();

	size_t charsNeeded = WideCharToMultiByte(nCodePage, 0, pString, nSize, nullptr, 0, nullptr, nullptr);
	if (charsNeeded == 0) return string();

	std::vector<char> buffer(charsNeeded);
	int charsConverted = WideCharToMultiByte(nCodePage, 0, pString, nSize, &buffer[0], charsNeeded, nullptr, nullptr);
	if (charsConverted == 0) return string();

	return string(&buffer[0], charsConverted);
}

wstring string_util::utf8_to_unicode(string &str)
{
	return A2W(str, CP_UTF8);
}

wstring string_util::ansi_to_unicode(string &str)
{
	return A2W(str, AppConfig::nCodePage);
}

string string_util::unicode_to_utf8 (wstring str)
{
	return W2A(str, CP_UTF8);
}

string string_util::unicode_to_ansi(wstring str)
{
	return W2A(str, AppConfig::nCodePage);
}

void string_util::unescape_simple(wstring &str)
{
	wstring::size_type pos = 0;
	while ((pos = str.find(L"\\r", pos)) != wstring::npos)
	{
		str.replace(pos, 2, L"\r");
	}

	pos = 0;
	while ((pos = str.find(L"\\n", pos)) != wstring::npos)
	{
		str.replace(pos, 2, L"\n");
	}

	pos = 0;
	while ((pos = str.find(L"\\t", pos)) != wstring::npos)
	{
		str.replace(pos, 2, L"\t");
	}
}
