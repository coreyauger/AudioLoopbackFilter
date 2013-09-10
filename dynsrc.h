//------------------------------------------------------------------------------
// File: DynSrc.h
//
//
// Copyright (c) Corey Auger.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __CDYNAMICSOURCE__
#define __CDYNAMICSOURCE__

class CDynamicSourceStream;  // The class that will handle each pin

class CDynamicSource: public CBaseFilter {
public:

    CDynamicSource(TCHAR *pName, LPUNKNOWN lpunk, CLSID clsid, HRESULT *phr);
#ifdef UNICODE
    CDynamicSource(CHAR *pName, LPUNKNOWN lpunk, CLSID clsid, HRESULT *phr);
#endif
    ~CDynamicSource();

    int       GetPinCount(void);
    CBasePin *GetPin(int n);

    // -- Utilities --

    CCritSec*   pStateLock(void) { return &m_cStateLock; }  // provide our critical section

    HRESULT     AddPin(CDynamicSourceStream *);
    HRESULT     RemovePin(CDynamicSourceStream *);

    STDMETHODIMP FindPin(
        LPCWSTR Id,
        IPin ** ppPin
    );

    int FindPinNumber(IPin *iPin);
    
    STDMETHODIMP JoinFilterGraph(IFilterGraph* pGraph, LPCWSTR pName);
    STDMETHODIMP Stop(void);
    STDMETHODIMP Pause(void);
		
	STDMETHODIMP SetSyncSource(IReferenceClock *pClock);
    
protected:

    CAMEvent m_evFilterStoppingEvent;

    int m_iPins;    // The number of pins on this filter. Updated by 
                    // CDynamicSourceStream constructors & destructors.
                    
    CDynamicSourceStream **m_paStreams;   // the pins on this filter.

    CCritSec m_csPinStateLock;

    CCritSec m_cStateLock;

};



class CDynamicSourceStream : public CAMThread, public CDynamicOutputPin {
public:

    CDynamicSourceStream(TCHAR *pObjectName,
                         HRESULT *phr,
                         CDynamicSource*pms,
                         LPCWSTR pName);
#ifdef UNICODE
    CDynamicSourceStream(CHAR *pObjectName,
                         HRESULT *phr,
                         CDynamicSource*pms,
                         LPCWSTR pName);
#endif
    virtual ~CDynamicSourceStream(void);  // virtual destructor ensures derived 
                                          // class destructors are called too

    HRESULT DestroySourceThread(void);

	virtual HRESULT SetSyncSource(IReferenceClock *pClock) PURE;	// CA - added this..

protected:

    CDynamicSource*m_pFilter;             // The parent of this stream

    virtual HRESULT FillBuffer(IMediaSample *pSamp) PURE;
	

    // Called as the thread is created/destroyed - use to perform
    // jobs such as start/stop streaming mode
    // If OnThreadCreate returns an error the thread will exit.
    virtual HRESULT OnThreadCreate(void) {return NOERROR;};
    virtual HRESULT OnThreadDestroy(void) {return NOERROR;};
    virtual HRESULT OnThreadStartPlay(void) {return NOERROR;};

    HRESULT Active(void);    // Starts up the worker thread

    HRESULT BreakConnect(void);

public:
    // thread commands
    enum Command {CMD_INIT, CMD_PAUSE, CMD_RUN, CMD_STOP, CMD_EXIT};

    HRESULT Init(void) { return CallWorker(CMD_INIT); }
    HRESULT Exit(void) { return CallWorker(CMD_EXIT); }
    HRESULT Run(void) { return CallWorker(CMD_RUN); }
    HRESULT Pause(void) { return CallWorker(CMD_PAUSE); }
    HRESULT Stop(void) { return CallWorker(CMD_STOP); }

    void OutputPinNeedsToBeReconnected(void);

protected:
    Command GetRequest(void) { return (Command) CAMThread::GetRequest(); }
    BOOL    CheckRequest(Command *pCom) { return CAMThread::CheckRequest( (DWORD *) pCom); }

    // override these if you want to add thread commands
    virtual DWORD ThreadProc(void);         // the thread function

    virtual HRESULT DoBufferProcessingLoop(void);    // the loop executed whilst running
    
    void FatalError(HRESULT hr);

    // *
    // * AM_MEDIA_TYPE support
    // *

    // If you support more than one media type then override these 2 functions
    virtual HRESULT CheckMediaType(const CMediaType *pMediaType);
    virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);  // List pos. 0-n

    // If you support only one type then override this fn.
    // This will only be called by the default implementations
    // of CheckMediaType and GetMediaType(int, CMediaType*)
    // You must override this fn. or the above 2!
    virtual HRESULT GetMediaType(CMediaType *pMediaType) {return E_UNEXPECTED;}

    STDMETHODIMP QueryId(
        LPWSTR * Id
    );

    bool m_fReconnectOutputPin;
};
    
#endif // __CDYNAMICSOURCE__

