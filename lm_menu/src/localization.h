#pragma once
#include "stdafx.h"
#include "string_util.h"

namespace Localization
{
	bool readLine(std::ifstream &f, string &line);
	bool readPair(std::ifstream &f, string &key, string &value);
	void loadFromFile(std::ifstream &f);
	void loadBlacklistFromFile(std::ifstream &f);

	bool has(wchar_t* key);

	void set(string &key, string &val);
	void set(wstring &key, wstring &val);
	wstring get(wstring &key, const wchar_t* def);
	wstring get(wstring &key, wstring &def);
	wstring get(wstring &key);
	wstring get(wchar_t* key);
	wstring get(wchar_t* key, const wchar_t* def);
}