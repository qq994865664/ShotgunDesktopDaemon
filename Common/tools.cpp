#include "tools.h"
#include <stdio.h>
#include <Windows.h>

string GetCurrentPath()
{
	char lp_filename[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, lp_filename, MAX_PATH);
	string file_name = lp_filename;
	string::size_type ipos = file_name.rfind('\\');
	return file_name.substr(0, ipos);
}

WCHAR* c2w(const char* str)
{
	if (str == NULL)
	{
		return NULL;
	}
	WCHAR wstr[MAX_PATH];
	size_t n = (size_t)MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, (int)n);
	return wstr;
}

WCHAR* c2utf8(const char* str)
{
	if (str == NULL)
	{
		return NULL;
	}
	WCHAR wstr[4096];
	memset(wstr, 0, 4096);
	size_t n = (size_t)MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, (int)n);
	return wstr;
}

char* w2c(const WCHAR* wstr)
{
	if (wstr == NULL)
	{
		return NULL;
	}
	char str[MAX_PATH * 2];
	sprintf_s(str, "%ws", wstr);
	return str;
}

char* w2c_long(const WCHAR* wstr)
{
	if (wstr == NULL)
	{
		return NULL;
	}
	char str[4096 * 2];
	sprintf_s(str, "%ws", wstr);
	return str;
}

char* cn2utf8(const char* m_string)
{
	int len = 0;
	wchar_t* w_string;
	char* utf8_string;
	//计算由ansi转换为unicode后，unicode编码的长度
	len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)m_string, -1, NULL, 0);//cp_acp指示了转换为unicode编码的编码类型
	w_string = (wchar_t*)malloc(2 * len + 2);
	memset(w_string, 0, 2 * len + 2);
	//ansi到unicode转换
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)m_string, -1, w_string, len);//cp_acp指示了转换为unicode编码的编码类型
	//计算unicode转换为utf8后，utf8编码的长度
	len = WideCharToMultiByte(CP_UTF8, 0, w_string, -1, NULL, 0, NULL, NULL);//cp_utf8指示了unicode转换为的类型
	utf8_string = (char*)malloc(len + 1);
	memset(utf8_string, 0, len + 1);
	//unicode到utf8转换
	WideCharToMultiByte(CP_UTF8, 0, w_string, -1, utf8_string, len, NULL, NULL);//cp_utf8指示了unicode转换为的类型
	free(w_string);
	return utf8_string;
}

string GetModuleDir()
{
	char lp_filename[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, lp_filename, MAX_PATH);
	string file_name = lp_filename;
	string::size_type ipos = file_name.rfind('\\');
	return file_name.substr(0, ipos);
}

string int2str(int inum)
{
	char szbuf[128] = { 0 };
	sprintf_s(szbuf, 128, "%d", inum);
	return szbuf;
}

string GetCurrentUserName()
{
	char strBuffer[256] = { 0 };
	DWORD dwSize = 256;
	GetUserName(strBuffer, &dwSize);
	return strBuffer;
}

string GetUserTempPath()
{
	string TempPath;
	char charPath[MAX_PATH];
	if (GetTempPath(MAX_PATH, charPath))
		TempPath = charPath;
	return TempPath;
}