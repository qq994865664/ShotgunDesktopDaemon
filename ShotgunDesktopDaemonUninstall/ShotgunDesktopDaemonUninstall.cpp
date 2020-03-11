// ShotgunDesktopDaemonUninstall.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>

#if NDEBUG
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif

#define KEY_NAME			"SHOTGUN_DESKTOP_DAEMON"

void CancelAutoStart()
{
	HKEY hKey;
	std::string strRegPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	try
	{
		//1、找到系统的启动项  
		if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			//2、删除值
			RegDeleteValue(hKey, KEY_NAME);
			//3、关闭注册表
			RegCloseKey(hKey);
		}
		
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strRegPath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			//2、删除值
			RegDeleteValue(hKey, KEY_NAME);
			//3、关闭注册表
			RegCloseKey(hKey);
		}
	}
	catch (std::exception e)
	{
		
	}

}

int main()
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT("ShotgunDesktopDaemonUninstall"));
	if ((hMutex == NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
	{
		CloseHandle(hMutex);
		return 0;
	}
	////开机自动隐藏窗口
	//HWND hwnd;
	//hwnd = FindWindow("ConsoleWindowClass", NULL);//找到当前窗口句柄

	//if (hwnd)
	{
#if NDEBUG

		//ShowOwnedPopups(hwnd, SW_HIDE);//显示或隐藏由指定窗口所有的全部弹出式窗口
		//ShowWindow(hwnd, SW_HIDE);//控制窗口的可见性
		CancelAutoStart();
#endif
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
