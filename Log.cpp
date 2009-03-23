#include "StdAfx.h"
#include "Log.h"

HANDLE		g_hFile;
LogLevel	g_nLogLevel = LogLevel_Info;

bool LogStart()
{
	TCHAR *pbuf;
	DWORD size, n;

	// get executable name
	// 100% errorproof ;)
	size = 256 - 5;
	pbuf = NULL;
	do
	{
		if(pbuf)			
			delete pbuf;
		size += 256;
		// make room for ".log"
		pbuf = new TCHAR[size + 5];

		n = GetModuleFileName(NULL, pbuf, size);
	} while(n == size);

	_tcscpy(&pbuf[n], _T(".log"));

	// FILE_FLAG_WRITE_THROUGH disables lazy flush
	g_hFile = CreateFile(pbuf, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);

	delete [] pbuf;

	if(INVALID_HANDLE_VALUE == g_hFile)
	{
		return false;
	}

	return true;
}

void LogStop()
{
	CloseHandle(g_hFile);
}

// formats string, allocating buffer if needed
// btw, i know that my english is bad )
// nBufferSize contains buffer size in bytes
// on return nBufferSize contains result length in bytes (not including terminating zero)
// if result fits in nBufferSize, returns pBuffer
// else
//  new buffer of size = (result_length + 1) is allocated
//  returns new buffer
// NOTE: MSDN suggests to use StringCbVPrintf instead of vprintf
TCHAR* SafeFormatStringVA(TCHAR *pBuffer, DWORD *nBufferSize, TCHAR *pszFormat, va_list argptr)
{
	size_t bsize, left;
	TCHAR *pb;
	HRESULT hr;
	
	bsize = *nBufferSize;

	// try to fit result into the supplied buffer
	hr = StringCbVPrintfEx(pBuffer, bsize, NULL, &left, 0, pszFormat, argptr);
	if(STRSAFE_E_INSUFFICIENT_BUFFER != hr)
	{
		if(FAILED(hr))
		{
			throw std::exception("Logging: Formatting failed.");
		}

		*nBufferSize = bsize - left;
		return pBuffer;
	}

	// it seems that mobile runtime doesn't have a "_vsctprintf" function :(
	// so we keep allocating larger buffer until it's big enough
	pb = NULL;
	do
	{
		if(pb)
		{
			LocalFree(pb);
		}
		bsize *= 2;
		pb = reinterpret_cast<TCHAR*>(LocalAlloc(LMEM_FIXED, bsize));

		hr = StringCbVPrintfEx(pb, bsize, NULL, &left, 0, pszFormat, argptr);
	} while(STRSAFE_E_INSUFFICIENT_BUFFER == hr);

	if(FAILED(hr))
	{
		throw std::exception("Logging: Formatting failed.");
	}

	*nBufferSize = bsize - left;
	return pb;
}

TCHAR* SafeFormatString(TCHAR *pBuffer, DWORD *nBufferSize, TCHAR *pszFormat, ...)
{
	va_list argptr;

	va_start(argptr, pszFormat);

	return SafeFormatStringVA(pBuffer, nBufferSize, pszFormat, argptr);
}


void LogMessage(LogLevel nLevel, TCHAR *pszLocation, TCHAR *pszMessage)
{
	TCHAR buf[512];
	TCHAR *pb;
	DWORD size;
	SYSTEMTIME st;
	FILETIME ft;

	// check log level
	if(nLevel < g_nLogLevel)
	{
		return;
	}

	// format string: timestamp|level|where|text
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	// here goes first part (without message text)
	size = sizeof(buf) / sizeof(TCHAR);
	pb = SafeFormatString(buf, &size,
		_T("\n%X%X|%d|%s|"), ft.dwHighDateTime, ft.dwLowDateTime,
		nLevel, pszLocation);

	// print debug output before we release msg buffer
#	ifdef _DEBUG
	OutputDebugString(pb);
	OutputDebugString(pszMessage);
	OutputDebugString(_T("\r\n"));
#	endif

	size = WriteFile(g_hFile, pb, size, &size, NULL);

	if(pb != buf)
	{	// pb was allocated by SafeFormatString
		LocalFree(pb);
	}
	if(!size)
	{
		throw std::exception("Logging: WriteFile(prefix) failed.");
	}

	// here goes actual message text
	size = _tcslen(pszMessage) * sizeof(TCHAR);
	if(!WriteFile(g_hFile, pszMessage, size, &size, NULL))
	{
		throw std::exception("Logging: WriteFile(msg) failed.");
	}

	// no flush is needed as long as log file
	// is opened with FILE_FLAG_WRITE_THROUGH flag
}

void LogMessageFmt(LogLevel nLevel, TCHAR *pszLocation, TCHAR *pszFormat, ...)
{
	va_list argptr;
	TCHAR buf[512];
	TCHAR *pb;
	DWORD size;

	// check log level
	if(nLevel < g_nLogLevel)
	{
		return;
	}

	// format message and pass it to the LogMessage function
	va_start(argptr, pszFormat);

	size = sizeof(buf) / sizeof(TCHAR);
	pb = SafeFormatStringVA(buf, &size, pszFormat, argptr);

	LogMessage(nLevel, pszLocation, pb);

	if(pb != buf)
	{	// pb was allocated by SafeFormatString
		LocalFree(pb);
	}
}

bool InstallCrashHandler()
{
	// TODO: code
	return true;
}

void InvokeCrashHandler()
{
	// TODO: arguments
	// TODO: code
}
