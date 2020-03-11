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
		//1���ҵ�ϵͳ��������  
		if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			//2��ɾ��ֵ
			RegDeleteValue(hKey, KEY_NAME);
			//3���ر�ע���
			RegCloseKey(hKey);
		}
		
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strRegPath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			//2��ɾ��ֵ
			RegDeleteValue(hKey, KEY_NAME);
			//3���ر�ע���
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
	////�����Զ����ش���
	//HWND hwnd;
	//hwnd = FindWindow("ConsoleWindowClass", NULL);//�ҵ���ǰ���ھ��

	//if (hwnd)
	{
#if NDEBUG

		//ShowOwnedPopups(hwnd, SW_HIDE);//��ʾ��������ָ���������е�ȫ������ʽ����
		//ShowWindow(hwnd, SW_HIDE);//���ƴ��ڵĿɼ���
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
