#include "StdAfx.h"
#include "RegConfig.h"

CRegConfig::CRegConfig(void)
: m_hKey(0)
{
}

CRegConfig::~CRegConfig(void)
{
	CloseKey();
}

void CRegConfig::SetPlugin(TCHAR *pszName)
{
	TCHAR path[512];
	DWORD err, disp;

	CloseKey();

	_tcscpy(path, REGCONFIG_PLUGINSPATH);
	_tcscat(path, pszName);
	err = RegCreateKeyEx(
		HKEY_CURRENT_USER, path, 0, _T(""),
		REG_OPTION_NON_VOLATILE, 0, NULL, &m_hKey, &disp);
	if(ERROR_SUCCESS != err)
	{
		throw CSystemException(err, "Failed to create plugin's registry key.");
	}
}

void CRegConfig::CloseKey()
{
	if(m_hKey)
	{
		RegCloseKey(m_hKey);
		m_hKey = 0;
	}
}

int CRegConfig::GetIntValue(TCHAR* Name, int nDefaultValue)
{
	DWORD err, type, size;
	int value;

	size = sizeof(value);
	err = RegQueryValueEx(m_hKey, Name, NULL,
		&type, reinterpret_cast<LPBYTE>(&value), &size);
	if(ERROR_SUCCESS != err)
	{
		//throw CSystemException(err, "Failed to query configuration value.");
		return nDefaultValue;
	}

	if(type != REG_DWORD)
	{
		throw std::exception("Failed to query configuration value (type mismatch).");
	}

	if(size != sizeof(value))
	{
		throw std::exception("Failed to query configuration value (size mismatch).");
	}

	return value;
}

float CRegConfig::GetRealValue(TCHAR* Name, float fDefaultValue)
{
	DWORD err, type, size;
	float value;

	size = sizeof(value);
	err = RegQueryValueEx(m_hKey, Name, NULL,
		&type, reinterpret_cast<LPBYTE>(&value), &size);
	if(ERROR_SUCCESS != err)
	{
		//throw CSystemException(err, "Failed to query configuration value.");
		return fDefaultValue;
	}

	if(type != REG_DWORD)
	{
		throw std::exception("Failed to query configuration value (type mismatch).");
	}

	if(size != sizeof(value))
	{
		throw std::exception("Failed to query configuration value (size mismatch).");
	}

	return value;
}

tstring CRegConfig::GetStringValue(TCHAR* Name, const TCHAR* strDefaultValue)
{
	DWORD err, type, size;
	TCHAR *pbuffer;

	size = 0;
	err = RegQueryValueEx(m_hKey, Name, NULL,
		&type, NULL, &size);
	if(ERROR_SUCCESS != err)
	{
		//throw CSystemException(err, "Failed to query configuration value.");
		return tstring(strDefaultValue);
	}

	if(type != REG_SZ)
	{
		throw std::exception("Failed to query configuration value (type mismatch).");
	}

	pbuffer = new TCHAR[(size - 1) / sizeof(TCHAR)];
	err = RegQueryValueEx(m_hKey, Name, NULL,
		&type, reinterpret_cast<LPBYTE>(pbuffer), &size);
	if(ERROR_SUCCESS != err)
	{
		throw CSystemException(err, "Failed to query configuration value.");
	}

	return tstring(pbuffer);
}

void CRegConfig::SetValue(TCHAR* Name, int nValue)
{
	DWORD err;

	err = RegSetValueEx(m_hKey, Name, NULL,
		REG_DWORD, reinterpret_cast<LPBYTE>(&nValue), sizeof(nValue));
	if(ERROR_SUCCESS != err)
	{
		throw CSystemException(err, "Failed to set configuration value.");
	}
}

void CRegConfig::SetValue(TCHAR* Name, float fValue)
{
	DWORD err;

	err = RegSetValueEx(m_hKey, Name, NULL,
		REG_DWORD, reinterpret_cast<LPBYTE>(&fValue), sizeof(fValue));
	if(ERROR_SUCCESS != err)
	{
		throw CSystemException(err, "Failed to set configuration value.");
	}
}

void CRegConfig::SetValue(TCHAR* Name, const TCHAR* strValue)
{
	DWORD err;

	err = RegSetValueEx(m_hKey, Name, NULL,
		REG_SZ, reinterpret_cast<const BYTE*>(strValue),
		(_tcslen(strValue) + 1) * sizeof(TCHAR));
	if(ERROR_SUCCESS != err)
	{
		throw CSystemException(err, "Failed to set configuration value.");
	}
}
