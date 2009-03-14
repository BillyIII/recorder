#pragma once

#include "StateNotify.h"
#include "Recorder.h"

#ifndef _UNICODE
typedef std::string tstring;
#else
typedef std::wstring tstring;
#endif

class CNotifier
{
private:
	// state trackers
	CStateNotify	*m_psnTalkingCallerNumber;
	CStateNotify	*m_psnTalkingCallerName;
	CStateNotify	*m_psnTalkingCallerContact;
	CStateNotify	*m_psnCallTalking;

	// sound recorder
	CRecorder		*m_pRecorder;

	tstring			m_strSavePath;

private:
	static void CallTalkingCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData);
	void CancelNotifications();

	void OnPhoneTalkStarted();
	void OnPhoneTalkFinished();

	TCHAR *GetSaveFileName();

public:
	CNotifier(void);
	~CNotifier(void);

	TCHAR *GetSavePath() {return m_strSavePath;}
	void SetSavePath(TCHAR *pstrPath) {m_strSavePath = pstrPath;}
};
