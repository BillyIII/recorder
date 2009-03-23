#include "stdafx.h"
#include "Application.h"

IPlugin *g_BuiltInPlugins[] =
{
	new CTalkRecorder(),
	NULL
};

CApplication::CApplication()
: m_pNotifier(NULL)
, m_pConfig(NULL)
{
	Start();
}

CApplication::~CApplication()
{
	try
	{
		Stop();
	}
	catch(...)
	{
		// TODO: log error
	}
}

void CApplication::AddPlugin(IPlugin *pp)
{
	INotifListener *plisten;

	m_pConfig->SetPlugin(pp->GetName());
	if(m_pConfig->GetIntValue(APP_CFG_PLUGINENABLED, 1))
	{
		m_Plugins.push_back(pp);

		pp->LoadConfig(m_pConfig);

		if(plisten = pp->GetListener())
		{
			m_pNotifier->AddListener(plisten);
		}
	}
}

void CApplication::RemovePlugin(IPlugin *pp)
{
	INotifListener *plisten;

	m_pConfig->SetPlugin(pp->GetName());
	pp->SaveConfig(m_pConfig);

	if(plisten = pp->GetListener())
	{
		m_pNotifier->RemoveListener(plisten);
	}

	pp->Unload();
}

void CApplication::LoadPlugins()
{
	IPlugin **pp = g_BuiltInPlugins;
	while(*pp)
	{
		AddPlugin(*pp);
		pp ++;
	}

	tstring plgdir, plgmask, path;
	WIN32_FIND_DATA fd;
	HANDLE hfind;
	HINSTANCE hlib;
	IPlugin *pplg;

	m_pConfig->SetPlugin(APP_CFG_PLUGINNAME);

	plgdir = m_pConfig->GetStringValue(APP_CFG_PLUGINSPATH, APP_DEFAULT_PLUGINSPATH);
	plgmask = m_pConfig->GetStringValue(APP_CFG_PLUGINSMASK, APP_DEFAULT_PLUGINSMASK);
	path = plgdir;
	path += plgmask;
	hfind = FindFirstFile(path.c_str(), &fd);
	if(INVALID_HANDLE_VALUE != hfind)
	{
		do
		{
			path.resize(plgdir.size());
			path += fd.cFileName;

			hlib = LoadLibrary(path.c_str());
			if(NULL == hlib)
			{
				// TODO: log error
				continue;
			}

			GETPLUGINPROC proc =
				reinterpret_cast<GETPLUGINPROC>(GetProcAddress(hlib, PLUGIN_PROC_NAME));
			if(NULL == proc)
			{
				// TODO: log error
				FreeLibrary(hlib);
				continue;
			}

			pplg = proc();
			if(NULL == pplg)
			{
				// TODO: log error
				FreeLibrary(hlib);
				continue;
			}

			AddPlugin(pplg);
			m_PluginLibraries.push_back(hlib);
		}
		while(FindNextFile(hfind, &fd));

		FindClose(hfind);
	}
}

void CApplication::UnloadPlugins()
{
	foreach(PluginsListIter, i, m_Plugins)
	{
		RemovePlugin(*i);
	}
	m_Plugins.clear();

	foreach(std::vector<HMODULE>::const_iterator, i, m_PluginLibraries)
	{
		FreeLibrary(*i);
	}
	m_PluginLibraries.clear();
}

void CApplication::Start()
{
	try
	{
		m_pNotifier = new CNotifier();
		m_pConfig = new CRegConfig();

		LoadPlugins();
	}
	catch(...)
	{
		Stop();
		throw;
	}
}

void CApplication::Stop()
{
	UnloadPlugins();

	delete m_pNotifier;
	delete m_pConfig;
}


