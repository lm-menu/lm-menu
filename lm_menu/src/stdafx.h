#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <map>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::wstring;

typedef unsigned int uint;

#ifdef _DEBUG
#define dprintf(...)   printf(__VA_ARGS__)
#define dwprintf(...) wprintf(__VA_ARGS__)
#else
#define dprintf(...)  do { } while(0)
#define dwprintf(...) do { } while(0)
#endif