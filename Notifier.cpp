#include "StdAfx.h"
#include "Notifier.h"

CNotifier::CNotifier(void)
: m_strSavePath(_T("\\"))
{
	try
	{
		m_psnTalkingCallerNumber = new CStateNotifyByNameNoMask(SN_PHONETALKINGCALLERNUMBER);
		m_psnTalkingCallerNumber->KeepStateData(true);

		m_psnTalkingCallerName = new CStateNotifyByNameNoMask(SN_PHONETALKINGCALLERNAME);
		m_psnTalkingCallerName->KeepStateData(true);

		m_psnTalkingCallerContact = new CStateNotifyByNameNoMask(SN_PHONETALKINGCALLERCONTACT);
		m_psnTalkingCallerContact->KeepStateData(true);

		m_psnCallTalking = new CStateNotifyByName(SN_PHONECALLTALKING);
		m_psnCallTalking->SetCallback(CallTalkingCallback, reinterpret_cast<DWORD>(this));

		m_pRecorder = new CRecorder();
	}
	catch(...)
	{
		CancelNotifications();

		if(m_pRecorder)
		{
			delete m_pRecorder;
		}

		throw;
	}
}

CNotifier::~CNotifier(void)
{
	CancelNotifications();

	if(m_pRecorder)
	{
		delete m_pRecorder;
	}
}

void CNotifier::CancelNotifications()
{
	delete g_psnTalkingCallerNumber;
	delete g_psnTalkingCallerName;
	delete g_psnTalkingCallerContact;
	delete g_psnCallTalking;
}

void CNotifier::OnPhoneTalkStarted()
{
	m_pRecorder->Record(GetSaveFileName(), 
}

void CNotifier::OnPhoneTalkFinished()
{
}

void CNotifier::CallTalkingCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData)
{
  DWORD state;
  CNotifier *pobj;

  // get the changed value
  state = *((DWORD*)pData);
  pobj = reinterpret_cast<CNotifier*>(dwUserData);

  if (state & SN_PHONECALLTALKING_BITMASK)
  {
	  pobj->OnPhoneTalkStarted();
  }
  else
  {
	  pobj->OnPhoneTalkFinished();
  }
}

TCHAR *CNotifier::GetSaveFileName()
{
	TCHAR buf[512]; // should be enough for mobile
	SYSTEMTIME t;

	GetLocalTime(&t);
	_stprintf(buf, _T("%s\\%02d%02d%02d-%02d%02d%02d.wav"),
		m_strSavePath, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	return buf;
}

