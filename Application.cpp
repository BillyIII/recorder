#include "stdafx.h"
#include "Application.h"

IPlugin *g_BuiltInPlugins[] =
{
	new CTalkRecorder(),
	NULL
};

PluginsList g_Plugins;
CNotifier	*g_pNotifier;
CRegConfig	*g_pConfig;

void AddPlugin(IPlugin *pp)
{
	INotifListener *plisten;

	g_Plugins.push_back(pp);

	g_pConfig->SetPlugin(pp->GetName());
	pp->LoadConfig(g_pConfig);

	if(plisten = pp->GetListener())
	{
		g_pNotifier->AddListener(plisten);
	}
}

void RemovePlugin(IPlugin *pp)
{
	INotifListener *plisten;

	g_pConfig->SetPlugin(pp->GetName());
	pp->SaveConfig(g_pConfig);

	if(plisten = pp->GetListener())
	{
		g_pNotifier->RemoveListener(plisten);
	}

	pp->Unload();
}

void LoadPlugins()
{
	IPlugin **pp = g_BuiltInPlugins;
	while(*pp)
	{
		AddPlugin(*pp);
		pp ++;
	}
}

void UnloadPlugins()
{
	foreach(PluginsListIter, i, g_Plugins)
	{
		RemovePlugin(*i);
	}
	g_Plugins.clear();
}

bool StartApplication()
{
	try
	{
		g_pNotifier = new CNotifier();
		g_pConfig = new CRegConfig();

		LoadPlugins();
	}
	catch(...)
	{
		return false;
	}

	return true;
}

bool StopApplication()
{
	try
	{
		UnloadPlugins();

		delete g_pNotifier;
		delete g_pConfig;
	}
	catch(...)
	{
		return false;
	}

	return true;
}


