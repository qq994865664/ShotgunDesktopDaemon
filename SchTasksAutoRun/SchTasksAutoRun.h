#pragma once

#include <string>
using namespace std;

typedef struct _DAY_OF_WEEK
{
	string Monday;
	string Tuesday;
	string Wednesday;
	string Thursday;
	string Friday;
	string Saturday;
	string Sunday;
}DAY_OF_WEEK;

void DoTask();
bool CopyFileFromRemote(string file_path_from, string file_path_to);

string CmdExec(const char* cmd);

bool QueryTask(string task_name);
void CreateOnceTask(string task_name, string file_path, string start_time);
void DeleteTask(string task_name);

void WriteLog(string str_log);

bool FileExist(string file_path);