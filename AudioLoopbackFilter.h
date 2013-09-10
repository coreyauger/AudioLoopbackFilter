//------------------------------------------------------------------------------
// File: AudioLoopbackFilter.h
//
// Desc: DirectShow
//
// Copyright (c) Corey Auger.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __AUDIOLOOPBACKFILTER__
#define __AUDIOLOOPBACKFILTER__

//CLSID_AudioLoopbackFilter
//{3DDE0EEE-479E-4157-A57D-1F13C1D0E352}
DEFINE_GUID(CLSID_AudioLoopbackFilter,
0x3DDE0EEE, 0x479E, 0x4157, 0xA5, 0x7D, 0x1F, 0x13, 0xC1, 0xD0, 0xE3, 0x52);

//CLSID_SynthFilterPropertyPage
//{79A98DE1-BC00-11ce-AC2E-444553541111}
DEFINE_GUID(CLSID_AudioLoopbackPropertyPage,
0x79a98de1, 0xbc00, 0x11ce, 0xac, 0x2e, 0x44, 0x45, 0x53, 0x54, 0x11, 0x11);

const int WaveBufferSize = 16*1024;     // Size of each allocated buffer
                                        // Originally used to be 2K, but at
                                        // 44khz/16bit/stereo you would get
                                        // audio breaks with a transform in the
                                        // middle.

const int SILENCE_BUF_SIZE = 1792;				// (CA) need to calc this..
	

#define WM_PROPERTYPAGE_ENABLE  (WM_USER + 100)

// below stuff is implementation-only....
#ifdef _AUDIOSYNTH_IMPLEMENTATION_

HRESULT get_default_device(IMMDevice **ppMMDevice);

// -------------------------------------------------------------------------
// CAudioLoopback
// -------------------------------------------------------------------------
class CAudioLoopback {
public:
	CAudioLoopback(CCritSec* pStateLock);
	~CAudioLoopback();

	WAVEFORMATEX* Format(){ return m_pwfx; }

	HRESULT Initialize();
	HRESULT FillPCMAudioBuffer(const WAVEFORMATEX& wfex, BYTE pBuf[], long& iSize);
	void GetPCMFormatStructure(WAVEFORMATEX* pwfex);

	REFERENCE_TIME			m_hnsDefaultDevicePeriod;

protected:
	UINT32					pnFrames;
	bool					bFirstPacket;
	HANDLE					m_hTask;
	IAudioClient*			m_pAudioClient;
	IAudioCaptureClient*	m_pAudioCaptureClient;
	WAVEFORMATEX*			m_pwfx;	 


	BYTE*					m_silenceBuf;
};


class CSynthStream;
class CAudioLoopback;

// -------------------------------------------------------------------------
// CAudioLoopbackFilter
// -------------------------------------------------------------------------
// CAudioLoopbackFilter manages filter level stuff

class CAudioLoopbackFilter :    public IAudioLoopbackFilter,
                        public CPersistStream,
                        public ISpecifyPropertyPages,
                        public CDynamicSource {

public:

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
    ~CAudioLoopbackFilter();

    DECLARE_IUNKNOWN;

    // override this to reveal our property interface
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    // --- ISpecifyPropertyPages ---

    // return our property pages
    STDMETHODIMP GetPages(CAUUID * pPages);

    // --- IPersistStream Interface

    STDMETHODIMP GetClassID(CLSID *pClsid);
    int SizeMax();
    HRESULT WriteToStream(IStream *pStream);
    HRESULT ReadFromStream(IStream *pStream);
    DWORD GetSoftwareVersion(void);
    
    CAudioLoopback *m_Loopback;

private:

    // it is only allowed to to create these objects with CreateInstance
    CAudioLoopbackFilter(LPUNKNOWN lpunk, HRESULT *phr);

    // When the format changes, reconnect...
    void ReconnectWithNewFormat(void);

};


// -------------------------------------------------------------------------
// CAudioLoopbackPin
// -------------------------------------------------------------------------
// CAudioLoopbackPin manages the data flow from the output pin.

class CAudioLoopbackPin : public CDynamicSourceStream {

public:

    CAudioLoopbackPin(HRESULT *phr, CAudioLoopbackFilter *pParent, LPCWSTR pPinName);
    ~CAudioLoopbackPin();

    BOOL ReadyToStop(void) {return FALSE;}

    // stuff an audio buffer with the current format
    HRESULT FillBuffer(IMediaSample *pms);

    // ask for buffers of the size appropriate to the agreed media type.
    HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
                             ALLOCATOR_PROPERTIES *pProperties);

    HRESULT GetMediaType(CMediaType *pmt);

    HRESULT CompleteConnect(IPin *pReceivePin);
    HRESULT BreakConnect(void);

    // resets the stream time to zero.
    HRESULT Active(void);
	HRESULT Run(REFERENCE_TIME tStart);
	
	HRESULT SetSyncSource(IReferenceClock *pClock);

private:

    void DerivePCMFormatFromADPCMFormatStructure(const WAVEFORMATEX& wfexADPCM, WAVEFORMATEX* pwfexPCM);

    // Access to this state information should be serialized with the filters
    // critical section (m_pFilter->pStateLock())

    // This lock protects: m_dwTempPCMBufferSize, m_hPCMToMSADPCMConversionStream,
    // m_rtSampleTime, m_fFirstSampleDelivered and m_llSampleMediaTimeStart
    CCritSec    m_cSharedState;     

    CRefTime     m_rtSampleTime;    // The time to be stamped on each sample
    HACMSTREAM m_hPCMToMSADPCMConversionStream;

    DWORD m_dwTempPCMBufferSize;
    bool m_fFirstSampleDelivered;
    LONGLONG m_llSampleMediaTimeStart;
	DWORD_PTR m_dwAdviseToken;
	HANDLE					m_hSemaphore;

    CAudioLoopback *m_Loopback;
    CAudioLoopbackFilter *m_pParent;
	IReferenceClock *m_pClock;
};

#endif // _AUDIOSYNTH_IMPLEMENTATION_ implementation only....

#endif /* __AUDIOSYNTH__ */



