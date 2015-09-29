#include "iat_patcher.h"

IMAGE_IMPORT_DESCRIPTOR* gImportTable = nullptr;

void initImportTable(HMODULE module)
{
	IMAGE_DOS_HEADER* dosHeader = PIMAGE_DOS_HEADER(module);

	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		dprintf("dosHeader->e_magic is wrong.\n");
		return ;
	}

	IMAGE_NT_HEADERS* pNTHeaders = PIMAGE_NT_HEADERS(LPBYTE(module) + dosHeader->e_lfanew);

	auto optionalHeader = pNTHeaders->OptionalHeader;

	if (optionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC) {
		dprintf("optionalHeader.magic is wrong.\n");
		return ;
	}

	auto iat = optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	if (iat.Size == 0 || iat.VirtualAddress == 0) {
		dprintf("IAT Section is empty.\n");
		return ;
	}

	gImportTable = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(LPBYTE(module) + iat.VirtualAddress);
}

bool IAT::PatchFunction(char* lpLibrary, char* lpFunctionName, LPVOID myFunction)
{
	auto base = GetModuleHandle(nullptr);
	if (gImportTable == nullptr) initImportTable(base);
	
	auto iat = gImportTable;

	if (iat == nullptr)
	{
		dprintf("No impoart table for this executable.\n");
		return false;
	}

	dprintf("Searching for %s:%s\n", lpLibrary, lpFunctionName);
	while (iat->FirstThunk)
	{
		auto lpDllName = LPSTR(PBYTE(base) + DWORD(iat->Name));
		// dprintf("Found dll: %s..\n", lpDllName);

		if (_stricmp(lpDllName, lpLibrary) == 0)
		{
			int n = 0;
			IMAGE_THUNK_DATA* thunkData = PIMAGE_THUNK_DATA(LPBYTE(base) + iat->OriginalFirstThunk);
			while (thunkData->u1.Function)
			{
				DWORD addr = thunkData->u1.AddressOfData;
				if (!(addr & 0x80000000))
				{
					auto importName = PIMAGE_IMPORT_BY_NAME(PBYTE(base) + addr);
					auto importFunctionName = LPSTR(importName->Name);
					// dprintf("Functoin: %s\n", importFunctionName);
					if (strcmp(importFunctionName, lpFunctionName) == 0)
					{
						dprintf("Patching: %s\t", importFunctionName);
						DWORD* lpFunction = (DWORD*)(LPBYTE(base) + iat->FirstThunk) + n;

						DWORD oldProtection;
						VirtualProtect(lpFunction, sizeof(DWORD), PAGE_READWRITE, &oldProtection);
						dprintf("at: 0x%08X\n", *lpFunction);
						*lpFunction = DWORD(myFunction);
						VirtualProtect(lpFunction, sizeof(DWORD), oldProtection, &oldProtection);
						return true;
					}
				}

				n++;
				thunkData++;
			}
		}

		iat++;
	}

	dprintf("WARN: Didn't find the function you are looking for.");

	return false;
}