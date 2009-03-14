#include "stdafx.h"
#include "Exceptions.h"

TCHAR *GetErrorMessage(DWORD dwCode)
{
	TCHAR *pret;
	if(! FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, dwCode, 0, reinterpret_cast<LPTSTR>(&pret), 0, NULL) )
	{
		return NULL;
	}
	return pret;
}

CMMException::CMMException(MMRESULT dwResult, const char* pszMessage)
	: exception(pszMessage)
	, m_dwResult(dwResult)
{
}

CSystemException::CSystemException(DWORD dwError, const char* pszMessage)
	: exception(pszMessage)
{
	m_dwError = dwError;
	m_pszSystemMessage = GetErrorMessage(dwError);
}

CSystemException::CSystemException(const char* pszMessage)
	: exception(pszMessage)
{
	m_dwError = GetLastError();
	m_pszSystemMessage = GetErrorMessage(m_dwError);
}

CSystemException::~CSystemException()
{
	if(m_pszSystemMessage)
	{
		LocalFree(m_pszSystemMessage);
	}
}
