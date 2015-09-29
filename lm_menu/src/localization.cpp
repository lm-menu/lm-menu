#include "localization.h"

std::unordered_map<wstring, wstring> mMenu;

// Load translation from file
void Localization::loadFromFile(std::ifstream &f)
{
	if (f.fail())
	{
		dprintf("Failed to open translation, skip.\n");
		return;
	}
	dprintf("Read translation ...\n");
	string strKey, strValue;
	while (readPair(f, strKey, strValue))
	{
		set(strKey, strValue);
	}
	dprintf("Translation loaded to memory.\n");
}

void Localization::loadBlacklistFromFile(std::ifstream& f)
{
	if (f.fail())
	{
		dprintf("Failed to open blacklist, skip.\n");
		return;
	}
	dprintf("Read blacklist ...\n");
	string strLine;
	while (readLine(f, strLine))
	{
		set(strLine, strLine);
	}
	dprintf("Blacklist loaded to memory.\n");
}

bool Localization::has(wchar_t* key)
{
	wstring wKey(key);
#ifdef _DEBUG
	if (wKey.empty()) return false;

	bool r = mMenu.find(wKey) == mMenu.end();
	if (r)
	{
		dwprintf(L"Not Found: %s\n", key);
	}
	return !r;
#else
	return !wKey.empty() && mMenu.find(wKey) != mMenu.end();
#endif
}

// Get a translation.
wstring Localization::get(wstring &key, const wchar_t* def)
{
	if (key.empty()) return wstring();

	auto i = mMenu.find(key);

	if (i == mMenu.end()) {
		dwprintf(L"Not Found: %s\n", key.c_str());
		return wstring(def);
	}

	return i->second;
}

wstring Localization::get(wstring &key, wstring &def)
{
	return get(key, def.c_str());
}

wstring Localization::get(wstring &key)
{
	return get(key, key.c_str());
}

wstring Localization::get(wchar_t* key)
{
	return get(key, key);
}

wstring Localization::get(wchar_t* key, const wchar_t* def)
{
	wstring wKey(key);
	return get(wKey, def);
}

// This is UTF-8 format, and will be translated to 
// Unicode for storage in memory.
void Localization::set(string &key, string &val)
{
	auto keyW = STR::utf8_to_unicode(key);
	auto valW = STR::utf8_to_unicode(val);

	set(keyW, valW);
}

void Localization::set(wstring &key, wstring &val)
{
	if (!key.empty() && !val.empty()) {
		STR::unescape_simple(key);
		STR::unescape_simple(val);
		// dwprintf(L">> %s\n", key.c_str());

		mMenu[key] = val;
	}
}

// Read one line, without comment
bool Localization::readLine(std::ifstream &f, string &line)
{
	do
	{
		if (f.eof())
			return false;

		std::getline(f, line);
	} while (line[0] == ';' || line.empty());

	return true;
}

// Read two lines
bool Localization::readPair(std::ifstream &f, string &key, string &value)
{
	return readLine(f, key) && readLine(f, value);
}
