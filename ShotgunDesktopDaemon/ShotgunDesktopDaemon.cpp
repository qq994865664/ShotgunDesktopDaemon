// ShotgunDesktopDaemon.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include "tools.h"
#include <string>
#include "tchar.h"
#include "ShotgunDesktopDaemon.h"
#include <filesystem>
#include <lmcons.h> 
#include <fstream>
#include "INIReader.h"
#include <lmat.h> 
#pragma comment(lib,"NETAPI32.LIB")


namespace fs = std::filesystem;

#if NDEBUG
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif

#define KEY_NAME			"SHOTGUN_DESKTOP_DAEMON"
#if _DEBUG
#define REMOTE_CONFIG_PATH	"D:\\ShotgunDesktopDaemonConfig.ini"
#else
#define REMOTE_CONFIG_PATH	"W:\\Tools\\ShotgunConfig\\ShotgunDesktop\\ShotgunDesktopDaemonConfig.ini"
#endif

static string gSyncTime = "";
static string gLogFilePath = "";
static string gRemoteFilePath = "";
static int gReadConfigFlag = 0;
static int gSyncTimeFlag = 0;

void AutoStart()
{
	HKEY hKey;
	std::string strRegPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	try
	{
		//1、找到系统的启动项  
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strRegPath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///打开启动项       
		{
			//2、得到本程序自身的全路径
			TCHAR strExeFullDir[MAX_PATH] = { 0 };
			GetModuleFileName(NULL, strExeFullDir, MAX_PATH);


			//3、判断注册表项是否已经存在
			TCHAR strDir[MAX_PATH] = {};
			DWORD nLength = MAX_PATH;
			long result = RegGetValue(hKey, nullptr, KEY_NAME, RRF_RT_REG_SZ, 0, strDir, &nLength);


			//4、不存在
			if (result != ERROR_SUCCESS || _tcscmp(strExeFullDir, strDir) != 0)
			{
				//5、添加一个子Key,并设置值，"GISRestart"是应用程序名字（不加后缀.exe） 
				RegSetValueEx(hKey, KEY_NAME, 0, REG_SZ, (LPBYTE)strExeFullDir,
					(lstrlen(strExeFullDir) + 1) * sizeof(TCHAR));
				//6、关闭注册表
				RegCloseKey(hKey);
			}
		}
		else
		{
			WriteLog("警告!系统参数错误,不能随系统启动");
			std::cout << "警告\n系统参数错误,不能随系统启动" << std::endl;
		}
	}
	catch (std::exception e)
	{
		WriteLog("AutoStart，打开注册表出现异常");
	}
}

void CancelAutoStart()
{
	HKEY hKey;
	std::string strRegPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	try
	{
		//1、找到系统的启动项  
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
		WriteLog("CancelAutoStart，打开注册表出现异常");
	}

}

bool CopyFileFromRemote(string file_path_from, string file_path_to)
{
	return fs::copy_file(file_path_from, file_path_to, fs::copy_options::overwrite_existing);
}

void Jobadd(int hour, int second)
{
	DWORD JobId, ret;
	AT_INFO ai;
	char* filepath;
	long Len;
	char RootPath[MAX_PATH];
	WCHAR szFilePath[256];
	GetSystemDirectory(RootPath, MAX_PATH);
	filepath = new char[strlen(RootPath) + 11];
	strcpy(filepath, RootPath);
	strcat(filepath, "\\notepad.exe"); //完整路径 
	memset(&ai, 0, sizeof(ai));
	Len = MultiByteToWideChar(CP_ACP, 0, filepath, strlen(filepath), szFilePath, sizeof(szFilePath));
	szFilePath[Len] = '\0';
	ai.Command = szFilePath;
	ai.DaysOfMonth = 0;
	ai.DaysOfWeek = 0x7F; //7F等于二进制的7个1，就是每周的7天全部运行 
	ai.Flags = JOB_RUN_PERIODICALLY;
	ai.JobTime = hour * 60 * 60 * 1000 + second * 60 * 1000; //22点21分，这里是以毫秒为单位的，所以需要乘这些值 
	ret = NetScheduleJobAdd(NULL, LPBYTE(&ai), &JobId);
	//GetOverlappedResult
	int i = GetLastError();
	if (ret == ERROR_SUCCESS)
		std::cout << " SUCCESS!!" << std::endl;
	else
		std::cout << "Error" << std::endl;

}

void WriteLog(string str_log)
{
	ofstream outfile;
	outfile.open("D:\\ShotgunDesktopDaemonLog.txt", ios::app);
	outfile << str_log << endl;
	outfile.close();
}


bool TimeUp(string str_time)
{
	int hour = atoi(str_time.substr(0, 2).c_str());
	int min = atoi(str_time.substr(3, 4).c_str());
	SYSTEMTIME st;
	GetLocalTime(&st);
	/*printf("%4d-%2d-%2d\n", st.wYear, st.wMonth, st.wDay);
	printf("%2d:%2d:%2d\n", st.wHour, st.wMinute, st.wSecond);
	printf("%d\n", st.wDayOfWeek);*/
	if (st.wHour == hour && st.wMinute == min)
		return true;
	else
		return false;
}

bool ReadConfig()
{
	try
	{
		string remote_config_path = REMOTE_CONFIG_PATH;

		char path_buffer[_MAX_PATH] = { 0 };
		char drive[_MAX_DRIVE] = { 0 };
		char dir[_MAX_DIR] = { 0 };
		char filename[_MAX_FNAME] = { 0 };
		char ext[_MAX_EXT] = { 0 };

		_splitpath_s(remote_config_path.c_str(), drive, dir, filename, ext);

		string temp_config_path = GetUserTempPath();
		temp_config_path += filename;
		temp_config_path += ext;

		//copy file from remote path
		CopyFileFromRemote(remote_config_path, temp_config_path);

		INIReader reader(temp_config_path);

		reader.ParseError();

		gRemoteFilePath = reader.Get("DAEMON", "RemoteFilePath", "");
		gSyncTime = reader.Get("DAEMON", "SyncTime", "13:00");
		gLogFilePath = reader.Get("LOG", "LogFilePath", "D:\\ShotgunDesktopDaemonLog.txt");
		
		return true;
	}
	catch (const std::exception& e)
	{
		WriteLog("读取配置文件失败！");
		return false;
	}
}

void DownloadFile()
{
	char path_buffer[_MAX_PATH] = { 0 };
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char filename[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };

	_splitpath_s(gRemoteFilePath.c_str(), drive, dir, filename, ext);

	//string local_file_path = fs::current_path().string();
	string local_file_path;
	local_file_path = GetModuleDir();
	local_file_path += "\\";
	local_file_path += filename;
	local_file_path += ext;

	try
	{
		//Auto download file from remote path
		bool b = CopyFileFromRemote(gRemoteFilePath, local_file_path);
		if (b)
		{
			//WriteLog("copy file from remote success");
			//WinExec 函数: 控制窗口的显示形式
			WinExec(local_file_path.c_str(), SW_HIDE);
		}
		else
		{
			WriteLog("copy file from remote fail");
		}

	}
	catch (const std::exception & e)
	{
		char szErr[256] = { 0 };
		sprintf(szErr, "copy file error %s", e.what());
		WriteLog(szErr);
		printf(szErr);
	}
}

int main()
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT("ShotgunDesktopDaemon"));
	if ((hMutex == NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
	{
		CloseHandle(hMutex);
		return 0;
	}

	//开机自动隐藏窗口
	HWND hwnd;
	hwnd = FindWindow("ConsoleWindowClass", NULL);//找到当前窗口句柄
	if (hwnd)
	{

#if NDEBUG
		
		ShowOwnedPopups(hwnd, SW_HIDE);//显示或隐藏由指定窗口所有的全部弹出式窗口
		ShowWindow(hwnd, SW_HIDE);//控制窗口的可见性
		//CancelAutoStart();
		//Sleep(100);
		AutoStart();
#endif
		ReadConfig();
		DownloadFile();
		while (true)
		{
			// Auto read config file at 12:15
			if (TimeUp("12:15") && gReadConfigFlag == 0)
			{
				ReadConfig();
				gReadConfigFlag++;
				// only read config once
			}
			else
			{
				gReadConfigFlag = 0;
			}

			bool isTimeUp = TimeUp(gSyncTime);
			if (isTimeUp && gSyncTimeFlag == 0)
			{
				gSyncTimeFlag++;
				
				DownloadFile();

			}
			else
			{
				gSyncTimeFlag = 0;
			}

			Sleep(30*1000);// sleep 30 second in loop
		}
	}
}