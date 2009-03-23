#include "StdAfx.h"
#include "TalkRecorder.h"

CTalkRecorder::CTalkRecorder(void)
: m_strSavePath(_T("\\Storage Card"))
, m_nSampleRate(44100)
, m_nBitsPerSample(16)
{
	m_pRecorder = new CRecorder();
}

CTalkRecorder::~CTalkRecorder(void)
{
	delete m_pRecorder;
}

tstring CTalkRecorder::GetSaveFileName(const TCHAR *pszCaller)
{
	SYSTEMTIME t;
	TCHAR buf[412];

	if(!pszCaller)
	{
		pszCaller = _T("Anonymous");
	}

	GetLocalTime(&t);
	_stprintf(buf, _T("%s\\%02d%02d%02d-%02d%02d%02d-%s.wav"),
		m_strSavePath.c_str(), t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond,
		pszCaller);

	return tstring(buf);
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
		m_strSavePath = pConfig->GetStringValue(TALKREC_CFG_SAVEPATH, m_strSavePath.c_str());
		m_nSampleRate = pConfig->GetIntValue(TALKREC_CFG_SAMPLERATE, m_nSampleRate);
		m_nBitsPerSample = pConfig->GetIntValue(TALKREC_CFG_BITSPERSAMPLE, m_nBitsPerSample);
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
	//TCHAR filename[512];
	m_strSavedFileName = GetSaveFileName(NULL);
	//m_strSavedFileName = tstring(filename);
	m_pRecorder->Record(m_strSavedFileName.c_str(), m_nSampleRate, m_nBitsPerSample);
}

void CTalkRecorder::OnPhoneTalkFinished(const TCHAR *pszName, const TCHAR *pszNumber)
{
	m_pRecorder->Stop();

	const TCHAR *pszname = NULL;
	pszname = pszNumber ? pszNumber : pszname;
	pszname = pszName ? pszName : pszname;

	if(pszname)
	{
		MoveFile(m_strSavedFileName.c_str(), GetSaveFileName(pszname).c_str());
	}
}




















