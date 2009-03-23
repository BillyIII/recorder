#pragma once

enum LogLevel
{
	LogLevel_Info,
	LogLevel_Warning,
	LogLevel_Error,
	LogLevel_Fatal
};

extern LogLevel	g_nLogLevel;

bool LogStart();
void LogStop();

// NOTE:  pszLocation should not contain '|'
// NOTE2: all logging errors result in std::exception being thrown
void LogMessage(LogLevel nLevel, TCHAR *pszLocation, TCHAR *pszMessage);
void LogMessageFmt(LogLevel nLevel, TCHAR *pszLocation, TCHAR *pszFormat, ...);

void InstallCrashHandler();
// TODO: arguments
void InvokeCrashHandler();


