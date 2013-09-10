//------------------------------------------------------------------------------
// File: IAudioLoopbackFilter.h
//
// Desc: DirectShow 
//
// Copyright (c) Corey Auger.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __IAUDIOLOOPBACKFILTER__
#define __IAUDIOLOOPBACKFILTER__

#ifdef __cplusplus
extern "C" {
#endif


//
// IAudioLoopbackFilter's GUID
//
// {4E48A6E8-481D-45CE-AAAE-7557BE49F5FB}
DEFINE_GUID(IID_IAudioLoopbackFilter, 
0x4E48A6E8, 0x481D, 0x45CE, 0xAA, 0xAE, 0x75, 0x57, 0xBE, 0x49, 0xF5, 0xFB);

enum SYNTH_OUTPUT_FORMAT
{
    SYNTH_OF_PCM,
    SYNTH_OF_MS_ADPCM
};

//
// IAudioLoopbackFilter
//
DECLARE_INTERFACE_(IAudioLoopbackFilter, IUnknown) {

    
};


#ifdef __cplusplus
}
#endif

#endif // __ISYNTH2__


