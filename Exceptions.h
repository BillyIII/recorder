#pragma once

// invokes FormatMessage with (allocate_buffer | from_system)
// returns NULL on error
// don't forget to call LocalFree to free the returned buffer
TCHAR *GetErrorMessage(DWORD dwCode);

class CMMException : public std::exception
{
public:
	MMRESULT	m_dwResult;
public:
	CMMException(MMRESULT dwResult, const char* pszMessage);
};

class CInvalidCallException : public std::exception
{
public:
	CInvalidCallException(const char* pszMessage)
		: std::exception(pszMessage) {}
};

class CArgumentException : public std::exception
{
public:
	tstring	m_strArgument;
public:
	CArgumentException(const TCHAR *pszArgument, const char* pszMessage = "Invalid argument.")
		: std::exception(pszMessage)
		, m_strArgument(pszArgument)
	{}
};

class CSystemException : public std::exception
{
private:
	DWORD			m_dwError;
	TCHAR			*m_pszSystemMessage;

public:
	CSystemException(DWORD dwError, const char* pszMessage);
	CSystemException(const char* pszMessage);

	~CSystemException();

	DWORD GetErrorCode() { return m_dwError; }
	TCHAR *GetSystemMessage() { return m_pszSystemMessage; }
};

class CIOException : public CSystemException
{
public:
	CIOException(DWORD dwError, const char* pszMessage)
		: CSystemException(dwError, pszMessage) {}
	CIOException(const char* pszMessage)
		: CSystemException(pszMessage) {}
};

class CSyncException : public CSystemException
{
public:
	CSyncException(DWORD dwError, const char* pszMessage)
		: CSystemException(dwError, pszMessage) {}
	CSyncException(const char* pszMessage)
		: CSystemException(pszMessage) {}
};




