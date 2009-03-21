#pragma once

#include "Plugin.h"
#include "Recorder.h"

#define TALKREC_CFG_SAVEPATH		_T("SavePath")
#define TALKREC_CFG_SAMPLERATE		_T("SampleRate")
#define TALKREC_CFG_BITSPERSAMPLE	_T("BitsPerSample")

#define TALKREC_PLUGINNAME			_T("TalkRecorder")

class CTalkRecorder : public IPlugin, public INotifListener
{
private:
	CRecorder	*m_pRecorder;

	tstring		m_strSavePath;
	int			m_nSampleRate;
	int			m_nBitsPerSample;

private:
	void GetSaveFileName(TCHAR *pBuffer);

public:
	CTalkRecorder(void);
	~CTalkRecorder(void);

	// -------------------------------
	//       interface IPlugin
	// -------------------------------
	TCHAR *GetName();

	void LoadConfig(IConfig *pConfig);
	void SaveConfig(IConfig *pConfig);

	INotifListener *GetListener();

	void Unload();

	// -------------------------------
	//    interface INotifListener
	// -------------------------------
	void OnPhoneTalkStarted();
	void OnPhoneTalkFinished();
};
