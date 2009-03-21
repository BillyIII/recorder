#pragma once


__interface INotifListener
{
	void OnPhoneTalkStarted();
	void OnPhoneTalkFinished();
};

__interface IConfig
{
	int GetIntValue(TCHAR* Name);
	float GetRealValue(TCHAR* Name);
	tstring GetStringValue(TCHAR* Name);

	void SetValue(TCHAR* Name, int nValue);
	void SetValue(TCHAR* Name, float fValue);
	void SetValue(TCHAR* Name, TCHAR* strValue);
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

typedef IPlugin* (*GETPLUGINPROC)();






