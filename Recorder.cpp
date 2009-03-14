#include "StdAfx.h"
#include "Recorder.h"

CRecorder::CRecorder(void)
:m_hDevice(NULL)
,m_hFile(INVALID_HANDLE_VALUE)
,m_nState(RecState_Stopped)
{
	m_hStateMutex = CreateMutex(NULL, FALSE, NULL);
	m_hOutputMutex = CreateMutex(NULL, FALSE, NULL);
#ifdef RECORDER_FORCEDONE_FIX
	m_hBuffersSemaphore = CreateSemaphore(NULL,
		RECORDER_NUM_BUFFERS, RECORDER_NUM_BUFFERS, NULL);
#endif
}

CRecorder::~CRecorder(void)
{
	if(RecState_Running == GetState())
	{
		Stop();
	}

	WaitForSingleObject(m_hOutputMutex, RECORDER_MAXIMAL_WAIT);
	CloseHandle(m_hOutputMutex);

	WaitForSingleObject(m_hStateMutex, RECORDER_MAXIMAL_WAIT);
	CloseHandle(m_hStateMutex);

#ifdef RECORDER_FORCEDONE_FIX
	CloseHandle(m_hBuffersSemaphore);
#endif
}

void CALLBACK CRecorder::StaticCallback(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance,
	DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	CRecorder *prec = reinterpret_cast<CRecorder*>(dwInstance);
	if(prec)
	{
		if(uMsg == WIM_DATA)
		{
			prec->OnRecordedData(reinterpret_cast<WAVEHDR*>(dwParam1));
		}
		else if(uMsg == WIM_CLOSE)
		{
			prec->OnRecordStopped();
		}
#ifdef _DEBUG
		else if (uMsg != WIM_OPEN)
		{	// just for testing
			//throw "Unexpected waveIn message!";
		}
#endif
	}
}

void CRecorder::OnRecordedData(WAVEHDR *pBuffer)
{
	try
	{

	DWORD t;
	MMRESULT res;

	// TODO: throwing from unknown thread doesn't look too nice

	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hOutputMutex, RECORDER_MAXIMAL_WAIT))
	{
		throw CSyncException("Wait() failed for output mutex.");
	}

	if(!WriteFile(m_hFile, pBuffer->lpData, pBuffer->dwBytesRecorded, &t, 0))
	{
		ReleaseMutex(m_hOutputMutex);
		throw CIOException("WriteFile failed");
	}

	if(!ReleaseMutex(m_hOutputMutex))
	{
		throw CSyncException("Release() failed for output mutex.");
	}

	waveInUnprepareHeader(m_hDevice, pBuffer, sizeof(WAVEHDR));

#ifdef RECORDER_FORCEDONE_FIX
	if(!ReleaseSemaphore(m_hBuffersSemaphore, 1, NULL))
	{
		throw CSyncException("Release() failed for buffers semaphore.");
	}
#endif

	if(RecState_Running == GetState())
	{	// if we are still running, add buffer again
#ifdef RECORDER_FORCEDONE_FIX
		if(WAIT_OBJECT_0 == WaitForSingleObject(m_hBuffersSemaphore, 0))
#endif
		{	// Look at Stop()
			// just in case the argument is a copy - use our buffer
			WAVEHDR *pb = &m_hdrBuffers[pBuffer->dwUser];
			pb->dwBufferLength =
				static_cast<DWORD>(m_fmtInput.nAvgBytesPerSec * RECORDER_BUFFERED_TIME);

			res = waveInPrepareHeader(m_hDevice, pb, sizeof(WAVEHDR));
			MMCHECK(res, "waveInPrepareHeader failed.");

			res = waveInAddBuffer(m_hDevice, pb, sizeof(WAVEHDR));
			MMCHECK(res, "waveInAddBuffer failed.");
		}
	}
	else
	{	// if we are stopping, free the buffer
		delete [] pBuffer->lpData;
	}

	}
	catch(const std::exception &ex)
	{
		TCHAR buf[1024];
		_stprintf(buf, _T("ERR_Data: %S\n"), ex.what());
		OutputDebugString(buf);
	}
}

void CRecorder::OnRecordStopped()
{
	try
	{
	FinalizeWaveFile();
	SetState(RecState_Stopped);
	m_hDevice = 0;
	}
	catch(const std::exception &ex)
	{
		TCHAR buf[1024];
		_stprintf(buf, _T("ERR_Done: %S\n"), ex.what());
		OutputDebugString(buf);
	}
}

void CRecorder::Record(TCHAR *pszFile, WORD dwSampleRate, WORD dwBitsPerSample)
{
	if(RecState_Stopped != GetState())
	{
		throw CInvalidCallException("Invalid Record call.");
	}

	MMRESULT res;

	m_fmtInput.wFormatTag = WAVE_FORMAT_PCM;
	m_fmtInput.nChannels = 1;
	m_fmtInput.nSamplesPerSec = dwSampleRate;
	// SampleRate * NumChannels * BitsPerSample/8
	m_fmtInput.nAvgBytesPerSec = dwSampleRate * 1 * dwBitsPerSample / 8;
	// NumChannels * BitsPerSample/8
	m_fmtInput.nBlockAlign = 1 * dwBitsPerSample / 8;
 	m_fmtInput.wBitsPerSample = dwBitsPerSample;
	m_fmtInput.cbSize = 0;

	//m_bMustStop = false;

	// TODO: waveInOpen(WAVE_FORMAT_QUERY)
	UINT ndev = waveInGetNumDevs();
	res = waveInOpen(&m_hDevice, WAVE_MAPPER, &m_fmtInput,
		reinterpret_cast<DWORD>(StaticCallback), reinterpret_cast<DWORD>(this),
		CALLBACK_FUNCTION);
	MMCHECK(res, "waveInOpen failed.");

	SetState(RecState_Running);

	try
	{
		PrepareWaveFile(pszFile);

		for(DWORD i=0; i<RECORDER_NUM_BUFFERS; i++)
		{
#ifdef RECORDER_FORCEDONE_FIX
			if(WAIT_OBJECT_0 != WaitForSingleObject(m_hBuffersSemaphore, RECORDER_MAXIMAL_WAIT))
			{	// Look at Stop()
				throw CSyncException("Wait() failed for buffers semaphore.");
			}
#endif

			memset(&m_hdrBuffers[i], 0, sizeof(m_hdrBuffers[i]));
			m_hdrBuffers[i].dwBufferLength =
				static_cast<DWORD>(m_fmtInput.nAvgBytesPerSec * RECORDER_BUFFERED_TIME);
			m_hdrBuffers[i].lpData = new CHAR[m_hdrBuffers[i].dwBufferLength];
			m_hdrBuffers[i].dwUser = i;

			res = waveInPrepareHeader(m_hDevice, &m_hdrBuffers[i], sizeof(WAVEHDR));
			MMCHECK(res, "waveInPrepareHeader failed.");

			res = waveInAddBuffer(m_hDevice, &m_hdrBuffers[i], sizeof(WAVEHDR));
			MMCHECK(res, "waveInAddBuffer failed.");
		}

		MMCALL( res, "waveInStart failed.",
			waveInStart(m_hDevice) );
	}
	catch(...)
	{
/*		for(DWORD i=0; (i < RECORDER_NUM_BUFFERS) && m_hdrBuffers[i].lpData; i++)
		{
			delete [] m_hdrBuffers[i].lpData;
		}
		waveInReset(m_hDevice);
		waveInClose(m_hDevice);
*/
		Stop();
		throw;
	}
}

void CRecorder::Stop()
{
	if(RecState_Running != GetState())
	{
		throw CInvalidCallException("Invalid Stop call.");
	}

	MMRESULT res;

	SetState(RecState_Stopping);

	// TODO: os bug? deadlock?
	// there must be some sync problem - deadlock occurs if we
	// don't free all buffers before a call to reset is made
	// no exception is thrown, so it's not a mutex error
#ifdef RECORDER_FORCEDONE_FIX
	for(DWORD i=0; i<RECORDER_NUM_BUFFERS; i++)
	{
		if(WAIT_OBJECT_0 != WaitForSingleObject(m_hBuffersSemaphore, RECORDER_MAXIMAL_WAIT))
		{	// TODO: exception??
			throw CSyncException("Wait() failed for buffers' semaphore.");
		}
	}
#endif

	MMCALL( res, "waveInReset failed.",
			waveInReset(m_hDevice) );
	MMCALL( res, "waveInClose failed.",
			waveInClose(m_hDevice) );
}

void CRecorder::PrepareWaveFile(TCHAR *pszFile)
{
	DWORD t;

	if(INVALID_FILE_ATTRIBUTES != GetFileAttributes(pszFile))
	{
		if(!DeleteFile(pszFile))
		{
			throw CIOException("Failed to delete existing file.");
		}
	}

	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hOutputMutex, RECORDER_MAXIMAL_WAIT))
	{
		throw CSyncException("Wait() failed for output mutex.");
	}

	m_hFile = CreateFile(pszFile, GENERIC_WRITE, 0, 0, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL, 0);
	if(INVALID_HANDLE_VALUE == m_hFile)
	{
		ReleaseMutex(m_hOutputMutex);
		throw CIOException("Failed to open file for writing.");
	}

	try
	{
		// write dummy header
		// the real one is written after recording is finished
		wavFullHeader head;
		memset(&head, 0xCC, sizeof(head));
		if(!WriteFile(m_hFile, &head, sizeof(head), &t, 0))
		{
			throw CIOException("Failed to write wave header.");
		}
		// trim file in case it is overwritten
		//if(!SetEndOfFile(m_hFile))
		//{
		//	throw CIOException("Failed to trim file.");
		//}
	}
	catch(...)
	{
		CloseHandle(m_hFile);
		ReleaseMutex(m_hOutputMutex);
		throw;
	}

	if(!ReleaseMutex(m_hOutputMutex))
	{
		throw CSyncException("Release() failed for output mutex.");
	}
}

void CRecorder::FinalizeWaveFile()
{
	DWORD t, size;
	wavFullHeader header;
	
	try
	{	
		if(WAIT_OBJECT_0 != WaitForSingleObject(m_hOutputMutex, RECORDER_MAXIMAL_WAIT))
		{
			throw CSyncException("Wait() failed for output mutex.");
		}

		size = SetFilePointer(m_hFile, 0, 0, FILE_CURRENT);
		if(0xFFFFFFFF == size)
		{
			throw CIOException("Failed to retrieve file size.");
		}

		if(0xFFFFFFFF == SetFilePointer(m_hFile, 0, 0, FILE_BEGIN))
		{
			throw CIOException("Failed to set file pointer.");
		}

		header.Head.ChunkID = WAVCHUNK_RIFF;
		header.Head.ChunkSize = size - sizeof(wavChunk);
		header.Head.Format = WAVHEAD_FORMAT;

		header.Format.ChunkID = WAVCHUNK_FMT;
		header.Format.ChunkSize = 16;
		header.Format.AudioFormat = 1;
		header.Format.NumChannels = m_fmtInput.nChannels;
		header.Format.SampleRate = m_fmtInput.nSamplesPerSec;
		header.Format.ByteRate = m_fmtInput.nAvgBytesPerSec;
		header.Format.BlockAlign = m_fmtInput.nBlockAlign;
		header.Format.BitsPerSample = m_fmtInput.wBitsPerSample;

		header.Data.ChunkID = WAVCHUNK_DATA;
		header.Data.ChunkSize = size - sizeof(wavFullHeader);

		if(!WriteFile(m_hFile, &header, sizeof(header), &t, 0))
		{
			throw CIOException("Failed to write wave header.");
		}
	}
	catch(...)
	{
		// close handle anyway
		CloseHandle(m_hFile);
		ReleaseMutex(m_hOutputMutex);

		throw;
	}

	CloseHandle(m_hFile);

	if(!ReleaseMutex(m_hOutputMutex))
	{
		throw CSyncException("Release() failed for output mutex.");
	}

#ifdef RECORDER_FORCEDONE_FIX
	if(!ReleaseSemaphore(m_hBuffersSemaphore, RECORDER_NUM_BUFFERS, NULL))
	{
		throw CSyncException("Release() failed for buffers' semaphore.");
	}
#endif
}

void CRecorder::SetState(RecorderState State)
{
	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hStateMutex, RECORDER_MAXIMAL_WAIT))
	{
		throw CSyncException("Wait() failed for state mutex.");
	}

	m_nState = State;

	if(!ReleaseMutex(m_hStateMutex))
	{
		throw CSyncException("Release() failed for state mutex.");
	}
}

RecorderState CRecorder::GetState()
{
	RecorderState state;

	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hStateMutex, RECORDER_MAXIMAL_WAIT))
	{
		throw CSyncException("Wait() failed for state mutex.");
	}

	state = m_nState;

	if(!ReleaseMutex(m_hStateMutex))
	{
		throw CSyncException("Release() failed for state mutex.");
	}

	return state;
}
