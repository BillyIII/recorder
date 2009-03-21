#pragma once

#include "StateNotify.h"
#include "Plugin.h"
#include "Exceptions.h"

typedef std::vector<INotifListener*> ListenersList;
typedef std::vector<INotifListener*>::iterator ListenersListIter;

class CNotifier
{
private:
	// state trackers
	CStateNotify	*m_psnTalkingCallerNumber;
	CStateNotify	*m_psnTalkingCallerName;
	CStateNotify	*m_psnTalkingCallerContact;
	CStateNotify	*m_psnCallTalking;

	ListenersList	m_pListeners;

private:
	static void CallTalkingCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData);
	void CancelNotifications();

	void OnPhoneTalkStarted();
	void OnPhoneTalkFinished();

	bool FindListener(INotifListener *pListener, ListenersListIter *pIter);

public:
	CNotifier(void);
	~CNotifier(void);

	void AddListener(INotifListener *pListener);
	bool HasListener(INotifListener *pListener);
	void RemoveListener(INotifListener *pListener);
};
