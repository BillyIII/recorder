#pragma once


__interface INotifListener
{
	void OnPhoneTalkStarted();
	void OnPhoneTalkFinished(const TCHAR *pszName, const TCHAR *pszNumber);
};

__interface IConfig
{
	int GetIntValue(TCHAR* Name, int nDefaultValue);
	float GetRealValue(TCHAR* Name, float fDefaultValue);
	tstring GetStringValue(TCHAR* Name, const TCHAR* strDefaultValue);

	void SetValue(TCHAR* Name, int nValue);
	void SetValue(TCHAR* Name, float fValue);
	void SetValue(TCHAR* Name, const TCHAR* strValue);
};

__interface IPlugin
{
	TCHAR *GetName();

	void LoadConfig(IConfig *pConfig);
	void SaveConfig(IConfig *pConfig);

	// should return same object for different calls
	INotifListener *GetListener();

	// deletes object
	void Unload();
};

// External plugins are dll's exporting "GetPlugin" method
#define PLUGIN_PROC_NAME _T("GetPlugin")
typedef IPlugin* (*GETPLUGINPROC)();






