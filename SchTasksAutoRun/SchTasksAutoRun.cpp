// SchTasksAutoRun.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>
#include "SchTasksAutoRun.h"
#include <string>
#include <filesystem>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include "INIReader.h"
#include <fstream>
#include "tools.h"
using namespace std;

#if NDEBUG
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif

namespace fs = std::filesystem;

#if _DEBUG
#define REMOTE_CONFIG_PATH	"D:\\ShotgunDesktopDaemonConfig.ini"
#define IGNORE_USER_PATH	"D:\\IgnoreUser"
#else
#define REMOTE_CONFIG_PATH	"W:\\Tools\\ShotgunConfig\\ShotgunDesktop\\ShotgunDesktopDaemonConfig.ini"
#define IGNORE_USER_PATH	"W:\\Tools\\ShotgunConfig\\ShotgunDesktop\\Timelog\\IgnoreUser"
#endif



static string gTaskNameNotify = "";
static string gTaskName = "";
static string gStartTime = "";
static string gRemoteFilePath = "";
static string gRemoteFilePathNotify = "";
static string gLogFilePath = "";

int main(int argc, char** argv)
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT("SchTaskAutoRun"));
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
#endif
	}

	//check if ignore this user
	string user_name = GetCurrentUserName();

	string ignore_user_path = IGNORE_USER_PATH;
	ignore_user_path += "\\";
	ignore_user_path += user_name;
	ignore_user_path += ".txt";

	//if the user name is ignore run
	if (FileExist(ignore_user_path))
	{
		return 0;
	}

	string Monday;
	string Tuesday;
	string Wednesday;
	string Thursday;
	string Friday;
	string Saturday;
	string Sunday;


	try
	{
		string str_path = REMOTE_CONFIG_PATH;
		INIReader reader(str_path);

		reader.ParseError();

		Monday = reader.Get("DAY_OF_WEEK", "Monday", "OFF");
		Tuesday = reader.Get("DAY_OF_WEEK", "Tuesday", "OFF");
		Wednesday = reader.Get("DAY_OF_WEEK", "Wednesday", "OFF");
		Thursday = reader.Get("DAY_OF_WEEK", "Thursday", "OFF");
		Friday = reader.Get("DAY_OF_WEEK", "Friday", "OFF");
		Saturday = reader.Get("DAY_OF_WEEK", "Saturday", "OFF");
		Sunday = reader.Get("DAY_OF_WEEK", "Sunday", "OFF");

		gStartTime = reader.Get("TASKS", "StartTime", "18:00");
		gRemoteFilePath = reader.Get("TASKS", "RemoteFilePath", "");
		gTaskName = reader.Get("TASKS", "TaskName", "ShotgunDesktopTimeLogRunTask");
		gRemoteFilePathNotify = reader.Get("TASKS", "RemoteFilePathNotify", "");
		gTaskNameNotify = reader.Get("TASKS", "TaskNameNotify", "ShotgunNotifyRunTask");

		gLogFilePath = reader.Get("LOG", "LogFilePath", "D:\\ShotgunDesktopDaemonLog.txt");
	}
	catch (const std::exception & e)
	{
		WriteLog("读取配置文件失败！");
		return -1;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);

	bool isTodayRun = false;

	switch (st.wDayOfWeek)
	{
	case 1:
		cout << "Monday" << endl;
		if (_stricmp(Monday.c_str(), "ON") == 0)
		{
			isTodayRun = true;
		}
		break;
	case 2:
		cout << "Tuesday" << endl;
		if (_stricmp(Tuesday.c_str(), "ON") == 0)
		{
			isTodayRun = true;
		}
		break;
	case 3:
		cout << "Wednesday" << endl;
		if (_stricmp(Wednesday.c_str(), "ON") == 0)
		{
			isTodayRun = true;
		}
		break;
	case 4:
		cout << "Thursday" << endl;
		if (_stricmp(Thursday.c_str(), "ON") == 0)
		{
			isTodayRun = true;
		}
		break;
	case 5:
		cout << "Friday" << endl;
		if (_stricmp(Friday.c_str(), "ON") == 0)
		{
			isTodayRun = true;
		}
		break;
	case 6:
		cout << "Saturday" << endl;
		if (_stricmp(Saturday.c_str(), "ON") == 0)
		{
			isTodayRun = true;
		}
		break;
	case 7:
		cout << "Sunday" << endl;
		if (_stricmp(Sunday.c_str(), "ON") == 0)
		{
			isTodayRun = true;
		}
		break;
	default:
		isTodayRun = false;
		break;
	}

	if (isTodayRun)
	{
		DoTask();
	}
}

string CmdExec(const char* cmd)
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

bool CopyFileFromRemote(string file_path_from, string file_path_to)
{
	return fs::copy_file(file_path_from, file_path_to, fs::copy_options::overwrite_existing);
}

void CreateOnceTask(string task_name, string file_path, string start_time)
{
	char cmd[256] = { 0 };
	sprintf_s(cmd, "cmd /C schtasks.exe /Create /TN \"%s\" /TR \"%s\" /SC ONCE /ST \"%s\"", task_name.c_str(), file_path.c_str(), start_time.c_str());
	WinExec(cmd, SW_HIDE);
	//CmdExec(cmd);
}

bool QueryTask(string task_name)
{
	char cmd[256] = { 0 };
	sprintf_s(cmd, "cmd /C schtasks.exe /Query /TN \"%s\"", task_name.c_str());
	string result = CmdExec(cmd);
	string::size_type ipos = result.find(gTaskName);
	if (ipos != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void DeleteTask(string task_name)
{
	char cmd[256] = { 0 };
	sprintf_s(cmd, "cmd /C schtasks.exe /Delete /TN \"%s\" /F", task_name.c_str());
	WinExec(cmd, SW_HIDE);
	//CmdExec(cmd);
}

void DoTask()
{
	char path_buffer[_MAX_PATH] = { 0 };
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char filename[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };

	_splitpath_s(gRemoteFilePath.c_str(), drive, dir, filename, ext);

	string local_file_path = GetModuleDir();
	local_file_path += "\\";
	local_file_path += filename;
	local_file_path += ext;

	_splitpath_s(gRemoteFilePathNotify.c_str(), drive, dir, filename, ext);

	string local_file_path_notify = GetModuleDir();
	local_file_path_notify += "\\";
	local_file_path_notify += filename;
	local_file_path_notify += ext;


	try
	{
		//Auto download file from remote path

		if (CopyFileFromRemote(gRemoteFilePath, local_file_path))
		{
			DeleteTask(gTaskName);
			Sleep(200);
			CreateOnceTask(gTaskName, local_file_path, gStartTime);
		}

		if (CopyFileFromRemote(gRemoteFilePathNotify, local_file_path_notify))
		{
			DeleteTask(gTaskNameNotify);
			Sleep(200);
			CreateOnceTask(gTaskNameNotify, local_file_path_notify, gStartTime);
		}

	}
	catch (const std::exception & e)
	{
	}
}

void WriteLog(string str_log)
{
	ofstream outfile;
	outfile.open(gLogFilePath, ios::app);
	outfile << str_log << endl;
	outfile.close();
}

bool FileExist(string file_path)
{
	struct stat buffer;
	return (stat(file_path.c_str(), &buffer) == 0);
}
