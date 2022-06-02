#ifndef SI_TYPES_H
#define SI_TYPES_H
#include <wchar.h>

/** @file SI_Types.h
 *	SI internal types definitions.
 */

/** SpecCore Boolean type.
 */
typedef bool SI_BOOL;

/** SpecCore Wide character string type.
 */
typedef wchar_t SI_WC;

/** SpecCore Integer type.
 */
typedef long long SI_64;

/** SpecCore unsigned 8 bit type.
 */
typedef unsigned char SI_U8;

/** SpecCore device handle type.
 */
typedef void* SI_H;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
/** OS specific calling convention.
 */
#	define SI_IMPEXP_CONV __cdecl
#else
/** OS specific calling convention.
 */
#	define SI_IMPEXP_CONV
#endif


/** Callback type for the feature callback.
 * 	@code
int SI_IMPEXP_CONV CoreFeatureCallback(SI_H Hndl, SI_WC* Feature, void* Context)
{
	if(wcscmp(Feature, L"Camera.ExposureTime"))
	{
		double dExposureTime = 0.0;
		if(SI_SUCCEEDED(SI_GetFloat(Hndl, Feature, &dExposureTime)))
		{
			CApplication* pApp = (CApplication*) Context;
			pApp->UpdateExposureTime(dExposureTime);
		}
	}

	return 0;
}

CApplication::RegisterCallbacks(void)
{
	SI_RegisterFeatureCallback(handle, L"Camera.ExposureTime", &CoreFeatureCallback, this);
}
*	@endcode
 */
typedef int (SI_IMPEXP_CONV* FeatureCallback)(SI_H Hndl, SI_WC* Feature, void* Context);


/** Callback type for incoming data.
 */
typedef int (SI_IMPEXP_CONV* DataCallBack)(SI_U8* _pBuffer, SI_64 _nFrameSize, SI_64 _nFrameNumber, void* _pContext);


/** Callback type for incoming GPS data and NavSync messages
 */
typedef int (SI_IMPEXP_CONV* GPSDataCallback)(SI_U8* _pBuffer, const SI_64 _nSizeInBytes, void* _pContext);

/** Callback type for incoming IMS Motor messages.
 */
typedef int (SI_IMPEXP_CONV* MotorCallback)(char _cInputChar, void* _pContext);

/** SpecCore boolean true value.
 */
#define SI_TRUE true

/** SpecCore boolean false value.
 */
#define SI_FALSE false

/** SpecCore system handle.
 */
#define SI_SYSTEM 0


/** Infinite timeout value used with #SI_Wait.
 */
#define SI_INFINITE -1

#endif