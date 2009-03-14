#include "StdAfx.h"
#include "StateNotify.h"

CStateNotify::CStateNotify(DWORD dwMask, HKEY pszRoot, LPCTSTR pszPath, LPCTSTR pszValue)
:m_hNotify(NULL)
,m_dwUserData(0)
,m_Callback(NULL)
,m_bKeepStateData(false)
,m_pStateData(NULL)
,m_cbStateDataSize(0)
,m_cbStateDataBufferSize(0)
{
  HRESULT hr;
  NOTIFICATIONCONDITION nc;

  nc.ctComparisonType = REG_CT_ANYCHANGE;
  nc.TargetValue.dw   = 0;
  nc.dwMask = dwMask;

  hr = RegistryNotifyCallback(pszRoot, pszPath, pszValue, StaticCallback,
	  reinterpret_cast<DWORD>(this), &nc, &m_hNotify);
  if(FAILED(hr))
  {
	  throw "Failed to register callback.";
  }
}

CStateNotify::~CStateNotify(void)
{
	RegistryCloseNotification(m_hNotify);
	ClearStateData(true);
}

void CStateNotify::StaticCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData)
{
	CStateNotify *ps = reinterpret_cast<CStateNotify*>(dwUserData);
	if(ps)
	{
		ps->OnStateChanged(hNotify, pData, cbData);
	}
}

void CStateNotify::OnStateChanged(HREGNOTIFY hNotify, const PBYTE pData, const UINT cbData)
{
	if(m_bKeepStateData)
	{
		// cbData == 0 means value is reset
		// we shall keep it
		if(cbData)
		{
			if(m_cbStateDataBufferSize < cbData)
			{
				ClearStateData(true);
				m_cbStateDataBufferSize = max(m_cbStateDataBufferSize * 2, cbData);
				m_pStateData = new BYTE[m_cbStateDataBufferSize];
			}
			memcpy(m_pStateData, pData, cbData);
			m_cbStateDataSize = cbData;
		}
	}
	if(m_Callback)
	{
		m_Callback(hNotify, m_dwUserData, pData, cbData);
	}
}

void CStateNotify::ClearStateData(bool bDeallocate)
{
	if(bDeallocate)
	{
		if(m_pStateData)
		{
			delete [] m_pStateData;
			m_pStateData = NULL;
		}
		m_cbStateDataBufferSize = 0;
	}
	m_cbStateDataSize = 0;
}
