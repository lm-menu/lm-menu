#include "app_config.h"

#define __APP_NAME L"lm_menu"
#define __CONF_NAME L".\\lm_menu.ini"

int AppConfig::nCodePage;

void AppConfig::init()
{
	nCodePage = GetPrivateProfileInt(__APP_NAME, L"Code Page", CP_ACP, __CONF_NAME);
}

bool AppConfig::ConfigIs(wstring wItem, wstring wDefault, wstring wExptected)
{
	return ReadConfig(wItem, wDefault) == wExptected;
}

wstring AppConfig::ReadConfig(wstring lpItem, wstring lpDefault)
{
	wchar_t lpContent[MAX_PATH];
	GetPrivateProfileString(__APP_NAME, lpItem.c_str(), lpDefault.c_str(), lpContent, _countof(lpContent), __CONF_NAME);
	return lpContent;
}

int AppConfig::ReadConfig(wstring lpItem, int lpDefault)
{
	//wchar_t lpDefaultW[10];
	//swprintf_s(lpDefaultW, _countof(lpDefaultW), L"%d", lpDefault);
	//wstring wValue = ReadConfig(lpItem, lpDefaultW);
	//return _wtoi(wValue.c_str());
	return GetPrivateProfileInt(__APP_NAME, lpItem.c_str(), lpDefault, __CONF_NAME);
}