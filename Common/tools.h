#pragma once
#include <Windows.h>
#include <string>
#include <iostream>

using namespace std;

string GetCurrentPath();

WCHAR* c2w(const char* str);

WCHAR* c2utf8(const char* str);

char* w2c(const WCHAR* wstr);

char* w2c_long(const WCHAR* wstr);

char* cn2utf8(const char* input);

string GetModuleDir();

string int2str(int inum);

string GetCurrentUserName();
