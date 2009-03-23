#pragma once

#include "Notifier.h"
#include "TalkRecorder.h"
#include "RegConfig.h"

#define APP_CFG_PLUGINENABLED	_T("Enabled")

#define APP_CFG_PLUGINNAME	_T("Application")
#define APP_CFG_PLUGINSPATH	_T("PluginsPath")
#define APP_CFG_PLUGINSMASK	_T("PluginsMask")

// Must end with '\\'
#define APP_DEFAULT_PLUGINSPATH	_T("\\Program Files\\Recorder3\\Plugins\\")
#define APP_DEFAULT_PLUGINSMASK	_T("*.plg")

typedef std::vector<IPlugin*> PluginsList;
typedef std::vector<IPlugin*>::const_iterator PluginsListIter;

class CApplication
{
private:


	PluginsList			 m_Plugins;
	CNotifier			*m_pNotifier;
	CRegConfig			*m_pConfig;
	std::vector<HMODULE> m_PluginLibraries;

private:
	void AddPlugin(IPlugin *pp);
	void RemovePlugin(IPlugin *pp);
	void LoadPlugins();
	void UnloadPlugins();

	void Start();
	void Stop();

public:
	CApplication();
	~CApplication();
};


