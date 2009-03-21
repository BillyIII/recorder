#include "StdAfx.h"
#include "Notifier.h"

CNotifier::CNotifier(void)
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
	}
	catch(...)
	{
		CancelNotifications();

		throw;
	}
}

CNotifier::~CNotifier(void)
{
	CancelNotifications();
}

void CNotifier::CancelNotifications()
{
	delete m_psnTalkingCallerNumber;
	delete m_psnTalkingCallerName;
	delete m_psnTalkingCallerContact;
	delete m_psnCallTalking;
}

void CNotifier::OnPhoneTalkStarted()
{
	foreach(ListenersListIter, i, m_pListeners)
	{
		(*i)->OnPhoneTalkStarted();
	}
}

void CNotifier::OnPhoneTalkFinished()
{
	foreach(ListenersListIter, i, m_pListeners)
	{
		(*i)->OnPhoneTalkFinished();
	}
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

bool CNotifier::FindListener(INotifListener *pListener, ListenersListIter *pIter)
{
	foreach(ListenersListIter, i, m_pListeners)
	{
		if (*i == pListener)
		{
			if(pIter)
			{
				*pIter = i;
			}
			return true;
		}
	}
	return false;
}

void CNotifier::AddListener(INotifListener *pListener)
{
	if(HasListener(pListener))
		throw CArgumentException(_T("pListener"), "Listener is already registered.");
	m_pListeners.push_back(pListener);
}

bool CNotifier::HasListener(INotifListener *pListener)
{
	return FindListener(pListener, NULL);
}

void CNotifier::RemoveListener(INotifListener *pListener)
{
	ListenersListIter i;
	if(!FindListener(pListener, &i))
		throw CArgumentException(_T("pListener"), "Listener is not registered.");
	m_pListeners.erase(i);
}

