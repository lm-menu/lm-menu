#include "stdafx.h"
#include "menu_hook.h"
#include "iat_patcher.h"
#include "my_api.h"
#include "localization.h"
#include "app_config.h"
#include <commctrl.h>
#include <VersionHelpers.h>

std::unordered_map<wstring, LPSTR> mString;

void writeRVA(LPVOID position, LPVOID target)
{
	// DWORD oldProtect;
	// VirtualProtect(position, sizeof DWORD, PAGE_EXECUTE_READWRITE, &oldProtect);
	*reinterpret_cast<DWORD *>(position) = DWORD(target) - DWORD(position) - 4;
	// VirtualProtect(position, sizeof DWORD, oldProtect, &oldProtect);
}

void __stdcall procFindTranslation (LPDWORD lpString)
{
	LPSTR lpOrigionalString = LPSTR(*lpString);
	LPWSTR lpOrigionalUnicode = STR::A2W(lpOrigionalString, AppConfig::nCodePage);
	if (lpOrigionalUnicode == nullptr) return;

	auto i = mString.find(lpOrigionalUnicode);
	if (i != mString.end())
	{
		// dprintf("Cache: %s->%s\n", lpOrigionalString, i->second);
		*lpString = DWORD(i->second);
		delete lpOrigionalUnicode;
		return;
	}

	if (!Localization::has(lpOrigionalUnicode)) {
		delete lpOrigionalUnicode;
		return;
	}

	wstring wsUnicode = Localization::get(lpOrigionalUnicode);
	string lpTranslationOnline = STR::W2A(wsUnicode, AppConfig::nCodePage);

	int nSize = lpTranslationOnline.size();
	char *lpTranlationA = new char[nSize + 1];
	strncpy_s(lpTranlationA, nSize + 1, lpTranslationOnline.c_str(), nSize);
	lpTranlationA[nSize] = 0;
	mString[lpOrigionalUnicode] = lpTranlationA;

	*lpString = DWORD(lpTranlationA);

	// dprintf("Load: %s->%s\n", lpOrigionalString, lpTranlationA);

	delete lpOrigionalUnicode;
	return;
}

void doSprintfHook()
{
	wstring doHook = AppConfig::ReadConfig(L"Hook sprintf", L"False");
	if (doHook == L"False")
	{
		dprintf("sprintf hook disabled.\n");
		return;
	}

	const wchar_t* lpHookEntry = doHook.c_str();
	DWORD offset = 0, nValidate = 0;
	swscanf_s(lpHookEntry, L"offset: 0x%08X, validate: 0x%08X", &offset, &nValidate);
	dwprintf(L"=== Begin Hook sprintf Config ===\n"
		"offset:\t 0x%08X\n"
		"validate:\t 0x%08X\n"
		L"=== End Hook sprintf Config ===\n", offset, nValidate);

	if (offset < 0x10000)
	{
		printf("Invalid offset, skip sprintf hook.\n");
		return;
	}

	auto fnSprintf = LPBYTE(DWORD(GetModuleHandle(nullptr)) + offset);

	if (*LPDWORD(fnSprintf) != nValidate)
	{
		dprintf("Validate failed(Expected 0x%08X, got 0x%08X), skip hook.\n", *LPDWORD(fnSprintf), nValidate);
		return;
	}

	LPBYTE memHooker = LPBYTE(VirtualAlloc(nullptr, 15, MEM_COMMIT, PAGE_EXECUTE_READWRITE));

	BYTE arrShell[] = {
		// 01380000    8D4424 08       lea eax, dword ptr ss : [esp + 0x8]
		0x8D, 0x44, 0x24, 0x08,

		// 01380004    50              push eax
		0x50,

		// 01380005    E8 6E56FC10     call 12345678; ????????
		0xE8
	};

	memcpy(memHooker, arrShell, sizeof arrShell);
	writeRVA(memHooker + 6,  procFindTranslation);

	*(memHooker + 10) = 0xE9;
	writeRVA(memHooker + 11, wsprintfA);

	DWORD oldProtect;
	VirtualProtect(fnSprintf, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

	*(fnSprintf) = 0xE9;
	writeRVA(fnSprintf + 1, memHooker);
	VirtualProtect(fnSprintf, 5, oldProtect, &oldProtect);

	dprintf("Hook sprintf complete!\n"
		"sprintf: 0x%08X\n"
		"xprintf: 0x%08X\n", DWORD(fnSprintf), DWORD(memHooker));
}

void doHookMenu()
{
	// InitCommonControls();
#ifdef _DEBUG
	// Re-enable printf functions.
	FILE *stream;

	if (AppConfig::ConfigIs(L"Log Target", L"Console", L"Console"))
	{
		if (!AttachConsole(-1))
			AllocConsole();

		freopen_s(&stream, "CONOUT$", "w+", stdout);
	}
	else
	{
		freopen_s(&stream, "lm_menu.log", "w+", stdout);
	}

	dprintf("lm_menu localization helper by Jixun\n"
		"The program is freeware and provided \"AS IS\"...\n"
		"The author cannot be held liable for damages of any\n"
		"kind arising from its use or presence.\n\n");

	std::ifstream fBlacklist("lm_blacklist.txt");
	Localization::loadBlacklistFromFile(fBlacklist);
	fBlacklist.close();
#endif

	std::ifstream fTranslation("lm_menu.txt");
	Localization::loadFromFile(fTranslation);
	fTranslation.close();

	dprintf("Begin Hooking..\n");
	char* lpUser32 = "user32.dll";
	char* lpGdi32  = "gdi32.dll";
	IAT::PatchFunction(lpUser32, "AppendMenuA", MyAPI::MyAppendMenuA);
	IAT::PatchFunction(lpUser32, "DialogBoxParamA", MyAPI::MyDialogBoxParamA);
	IAT::PatchFunction(lpUser32, "CreateWindowExA", MyAPI::MyCreateWindowExA);
	// IAT::PatchFunction(lpUser32, "RegisterClassExA", MyAPI::MyRegisterClassExA);
	if(AppConfig::ConfigIs(L"Hook SendMessageA", L"False", L"True"))
	{
		IAT::PatchFunction(lpUser32, "SendMessageA", MyAPI::MySendMessageA);
	} else
	{
		dprintf("Skip hook SendMessageA\n");
	}

	if (AppConfig::ConfigIs(L"Fix Locale", L"False", L"True"))
	{
		IAT::PatchFunction(lpUser32, "SetWindowTextA", MyAPI::MySetWindowTextA);
		IAT::PatchFunction(lpGdi32, "ExtTextOutA", MyAPI::MyExtTextOutA);
	}

	// IAT::PatchFunction(lpUser32, "DefWindowProcA", DefWindowProcW);
	// IAT::PatchFunction(lpUser32, "DispatchMessageA", DispatchMessageW);
	// IAT::PatchFunction(lpUser32, "GetMessageA", GetMessageW);
	dprintf("Done Hooking!\n");

	doSprintfHook();
}