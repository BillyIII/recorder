#pragma once

#include "Plugin.h"
#include "Exceptions.h"

#define REGCONFIG_PLUGINSPATH	_T("Software\\Recorder3\\Plugins\\")

class CRegConfig : public IConfig
{
private:
	HKEY	m_hKey;

public:
	CRegConfig(void);
	~CRegConfig(void);

	void SetPlugin(TCHAR *pszName);
	void CloseKey();

	// -------------------------------
	//       interface IConfig
	// -------------------------------
	int GetIntValue(TCHAR* Name);
	float GetRealValue(TCHAR* Name);
	tstring GetStringValue(TCHAR* Name);

	void SetValue(TCHAR* Name, int nValue);
	void SetValue(TCHAR* Name, float fValue);
	void SetValue(TCHAR* Name, TCHAR* strValue);
};
