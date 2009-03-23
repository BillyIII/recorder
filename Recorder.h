#pragma once

// ---------------------------------------------------------------------
// ---------------------- Simple sound recorder ------------------------
// ---------------------------------------------------------------------

#include "Exceptions.h"

//
// Wave file structures
//

#pragma pack(push,1)

#define WAVCHUNK_RIFF	0x46464952	// "RIFF"
#define WAVCHUNK_FMT	0x20746d66	// "fmt "
#define WAVCHUNK_DATA	0x61746164	// "data"

#define WAVHEAD_FORMAT	0x45564157	// "WAVE"

struct wavChunk
{
    DWORD ChunkID;
    DWORD ChunkSize;
};

struct wavHead : wavChunk
{
    DWORD Format;
};

struct wavFmt : wavChunk
{
    WORD AudioFormat;
    WORD NumChannels;
    DWORD SampleRate;
    DWORD ByteRate;
    WORD BlockAlign;
    WORD BitsPerSample;
};

struct wavData : wavChunk
{
};

struct wavFullHeader
{
	wavHead Head;
	wavFmt Format;
	wavData Data;
};

#pragma pack(pop)

// fixes bug in Stop() method
// makes it wait until all buffers are filled and released
#define RECORDER_FORCEDONE_FIX

#define RECORDER_NUM_BUFFERS	2
// record buffer size in seconds
// if RECORDER_FORCEDONE_FIX is enabled, 
//  buffer is returned _only_ after it is filled
#define RECORDER_BUFFERED_TIME	0.5f
// reasonable timeout for mutexes
#define RECORDER_MAXIMAL_WAIT	25000

#define MMCALL( res, msg, call )\
{\
	res = call;\
	MMCHECK(res, msg);\
}

#define MMCHECK( res, msg )\
{\
	if(MMSYSERR_NOERROR != res)\
	{\
		throw CMMException(res, msg);\
	}\
}

enum RecorderState
{
	RecState_Stopped,
	RecState_Running,
	RecState_Stopping
};

class CRecorder
{
private:
	// input device
	HWAVEIN			m_hDevice;
	// output file
	HANDLE			m_hFile;
	HANDLE			m_hOutputMutex;

	// use GetState/SetState
	RecorderState	m_nState;
	HANDLE			m_hStateMutex;

#ifdef RECORDER_FORCEDONE_FIX
	HANDLE			m_hBuffersSemaphore;
#endif

	// format of input (and output)
	WAVEFORMATEX	m_fmtInput;
	// input buffers
	WAVEHDR			m_hdrBuffers[RECORDER_NUM_BUFFERS];

	// callbacks for input
	static void CALLBACK StaticCallback(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance,
		DWORD_PTR dwParam1, DWORD_PTR dwParam2);
	void OnRecordedData(WAVEHDR *pBuffer);
	void OnRecordStopped();

	void PrepareWaveFile(const TCHAR *pszFile);
	void FinalizeWaveFile();

	// getter/setter for recorder state
	RecorderState GetState();
	void SetState(RecorderState State);

public:
	CRecorder(void);
	~CRecorder(void);

	void Record(const TCHAR *pszFile, WORD dwSampleRate, WORD dwBitsPerSample);
	void Stop();
};

