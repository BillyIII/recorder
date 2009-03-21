#include "StdAfx.h"
#include "TalkRecorder.h"

CTalkRecorder::CTalkRecorder(void)
: m_strSavePath(_T("\\"))
, m_nSampleRate(44100)
, m_nBitsPerSample(16)
{
	m_pRecorder = new CRecorder();
}

CTalkRecorder::~CTalkRecorder(void)
{
	delete m_pRecorder;
}

void CTalkRecorder::GetSaveFileName(TCHAR *pBuffer)
{
	SYSTEMTIME t;

	GetLocalTime(&t);
	_stprintf(pBuffer, _T("%s\\%02d%02d%02d-%02d%02d%02d.wav"),
		m_strSavePath, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
}

// -------------------------------
//       interface IPlugin
// -------------------------------
TCHAR *CTalkRecorder::GetName()
{
	return TALKREC_PLUGINNAME;
}

void CTalkRecorder::LoadConfig(IConfig *pConfig)
{
	if(pConfig)
	{
		m_strSavePath = pConfig->GetStringValue(TALKREC_CFG_SAVEPATH);
		m_nSampleRate = pConfig->GetIntValue(TALKREC_CFG_SAMPLERATE);
		m_nBitsPerSample = pConfig->GetIntValue(TALKREC_CFG_BITSPERSAMPLE);
	}
}

void CTalkRecorder::SaveConfig(IConfig *pConfig)
{
	if(pConfig)
	{
		pConfig->SetValue(TALKREC_CFG_SAVEPATH, (TCHAR*)m_strSavePath.c_str());
		pConfig->SetValue(TALKREC_CFG_SAMPLERATE, m_nSampleRate);
		pConfig->SetValue(TALKREC_CFG_BITSPERSAMPLE, m_nBitsPerSample);
	}
}

INotifListener *CTalkRecorder::GetListener()
{
	return (INotifListener*)this;
}

void CTalkRecorder::Unload()
{
	delete this;
}

// -------------------------------
//    interface INotifListener
// -------------------------------
void CTalkRecorder::OnPhoneTalkStarted()
{
	// TODO: check buffer size
	TCHAR filename[512];
	GetSaveFileName(filename);
	m_pRecorder->Record(filename, m_nSampleRate, m_nBitsPerSample);
}

void CTalkRecorder::OnPhoneTalkFinished()
{
	m_pRecorder->Stop();
}




















