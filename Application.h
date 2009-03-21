#pragma once

#include "Notifier.h"
#include "TalkRecorder.h"
#include "RegConfig.h"

typedef std::vector<IPlugin*> PluginsList;
typedef std::vector<IPlugin*>::iterator PluginsListIter;

bool StartApplication();
bool StopApplication();

