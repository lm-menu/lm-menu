#pragma once
#include "stdafx.h"

namespace AppConfig
{
	extern int nCodePage;

	void init();
	wstring ReadConfig(wstring wItem, wstring wDefault);
	bool ConfigIs(wstring wItem, wstring wDefault, wstring wExptected);
	int ReadConfig(wstring lpItem, int lpDefault);
}