#pragma once

#include <string>
using namespace std;

void AutoStart();
void CancelAutoStart();
bool CopyFileFromRemote(string file_path_from,string file_path_to);
void Jobadd(int hour, int second);
void WriteLog(string str_log);
bool TimeUp(string str_time);
bool ReadConfig();