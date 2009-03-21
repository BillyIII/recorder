#include "stdafx.h"
#include "StateNotify.h"
#include "Notification.h"
#include "Recorder.h"

// notification handlers
CStateNotify	*g_psnIncomingCall			= NULL;
CStateNotify	*g_psnTalkingCallerNumber	= NULL;
CStateNotify	*g_psnTalkingCallerName		= NULL;
CStateNotify	*g_psnTalkingCallerContact	= NULL;
CStateNotify	*g_psnCallTalking			= NULL;

CRecorder		*g_pRecorder				= NULL;

// phone callbacks
void PhoneIncomingCallCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData);
void PhoneCallTalkingCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData);

bool InitPhoneNotification()
{
	try
	{
		g_psnIncomingCall = new CStateNotifyByName(SN_PHONEINCOMINGCALL);
		g_psnIncomingCall->SetCallback(PhoneIncomingCallCallback, 0);

		g_psnTalkingCallerNumber = new CStateNotifyByNameNoMask(SN_PHONETALKINGCALLERNUMBER);
		g_psnTalkingCallerNumber->KeepStateData(true);

		g_psnTalkingCallerName = new CStateNotifyByNameNoMask(SN_PHONETALKINGCALLERNAME);
		g_psnTalkingCallerName->KeepStateData(true);

		g_psnTalkingCallerContact = new CStateNotifyByNameNoMask(SN_PHONETALKINGCALLERCONTACT);
		g_psnTalkingCallerContact->KeepStateData(true);

		g_psnCallTalking = new CStateNotifyByName(SN_PHONECALLTALKING);
		g_psnCallTalking->SetCallback(PhoneCallTalkingCallback, 0);

		g_pRecorder = new CRecorder();
	}
	catch(char *perr)
	{
		StopPhoneNotification();
		return false;
	}
	return true;
}

#define DELETE_OBJECT(obj)\
	if(obj)\
	{\
		delete obj;\
		obj = NULL;\
	}

bool StopPhoneNotification()
{
	DELETE_OBJECT(g_psnIncomingCall);
	DELETE_OBJECT(g_psnTalkingCallerNumber);
	DELETE_OBJECT(g_psnTalkingCallerName);
	DELETE_OBJECT(g_psnTalkingCallerContact);
	DELETE_OBJECT(g_psnCallTalking);

	DELETE_OBJECT(g_pRecorder);

	return true;
}

//
// Handles incoming call
//
void PhoneIncomingCallCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData)
{
  DWORD state;

  // get the changed value
  state = *((DWORD*)pData);

  if (state & SN_PHONEINCOMINGCALL_BITMASK)
  {
	  // incoming call is in progress
  }
  else
  {
	  // incoming call has ended
  }
}

//
// Handles talks
//
void PhoneCallTalkingCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData)
{
  DWORD state;

  // get the changed value
  state = *((DWORD*)pData);

  if (state & SN_PHONECALLTALKING_BITMASK)
  {
  }
  else
  {
	  // do something else!
  }
}







