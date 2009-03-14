#pragma once

#define CStateNotifyByName(name) CStateNotify(name##_BITMASK, name##_ROOT, name##_PATH, name##_VALUE);

#define CStateNotifyByNameNoMask(name) CStateNotify(0, name##_ROOT, name##_PATH, name##_VALUE);

class CStateNotify
{
private:
	HREGNOTIFY				m_hNotify;
	DWORD					m_dwUserData;
	REGISTRYNOTIFYCALLBACK	m_Callback;

	bool					m_bKeepStateData;
	BYTE					*m_pStateData;
	UINT					m_cbStateDataSize;
	UINT					m_cbStateDataBufferSize;

	static void StaticCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData);

	void OnStateChanged(HREGNOTIFY hNotify, const PBYTE pData, const UINT cbData);

public:
	CStateNotify(DWORD dwMask, HKEY pszRoot, LPCTSTR pszPath, LPCTSTR pszValue);
	virtual ~CStateNotify(void);

	REGISTRYNOTIFYCALLBACK GetCallback() {return m_Callback;}
	DWORD GetCallbackUserData() {return m_dwUserData;}
	void SetCallback(REGISTRYNOTIFYCALLBACK cb, DWORD dw) {m_Callback = cb; m_dwUserData = dw;}

	BYTE *GetStateData() {return m_pStateData;}
	UINT GetStateDataSize() {return m_cbStateDataSize;}
	void KeepStateData(bool bKeep) {m_bKeepStateData = bKeep;}
	void ClearStateData(bool bDeallocate = false);
};
