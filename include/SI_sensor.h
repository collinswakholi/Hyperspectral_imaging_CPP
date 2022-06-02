#ifndef SI_Sensor_h
#define SI_Sensor_h

/** @file SI_Sensor.h
 *	Public exported interface for SpecSensor
 */

#ifdef _WIN32
    #ifdef _SIDLL
    #	define SI_IMPEXP extern "C" __declspec(dllexport)
    #else
    #	define SI_IMPEXP extern "C" __declspec(dllimport)
    #endif
#else
    #define SI_IMPEXP
#endif

#include "SI_types.h"


/** Loads the library's internal structure. Must be called before any other functions can be used.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_Load(const SI_WC* LicensePath);


/** Unloads the library's internal structure and cleans up the memory.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_Unload(void);


/** Opens the sensor device on the selected DeviceIndex from the SpecSensor's device list and sets the opened sensor handle to the Handle variable.
 *	@param DeviceIndex Zero based index for the sensor device to be opened. Use "DeviceCount" feature to get the number of sensor devices in the system. Note: DeviceIndex is between 0 and DeviceCount-1.
 *	@param Handle Handle pointer to retrieve the opened device handle.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_Open(int DeviceIndex, SI_H* Handle);


/** Closes the sensor device specified by the Handle variable.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_Close(SI_H Handle);


/** Checks if a feature is implemented.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Implemented Boolean pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_IsImplemented(SI_H Handle, const SI_WC* Feature, SI_BOOL* Implemented);


/** Checks if a feature is read only
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param ReadOnly Boolean pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_IsReadOnly(SI_H Handle, const SI_WC* Feature, SI_BOOL* ReadOnly);


/** Checks if a feature is writable.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Writable Boolean pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_IsWritable(SI_H Handle, const SI_WC* Feature, SI_BOOL* Writable);


/** Checks if a feature is readable.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Readable Boolean pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_IsReadable(SI_H Handle, const SI_WC* Feature, SI_BOOL* Readable);


/** Registers a feature callback. Example:
 *	@code
int SI_IMPEXP_CONV SensorFeatureCallback(SI_H Handle, SI_WC* Feature, void* Context)
{
	if(wcscmp(Feature, L"Camera.ExposureTime"))
	{
		double dExposureTime = 0.0;
		if(SI_SUCCEEDED(SI_GetFloat(Handle, Feature, &dExposureTime)))
		{
			CApplication* pApp = (CApplication*) Context;
			pApp->UpdateExposureTime(dExposureTime);
		}
	}
}

CApplication::RegisterCallbacks(void)
{
    SI_RegisterFeatureCallback(Handle, L"Camera.ExposureTime", &SensorFeatureCallback, this);
}
	 *	@endcode
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Callback Callback function to be registered. Check #FeatureCallback for the correct signature.
 *	@param Context Optional data pointer which usually is the class pointer of the caller object so it can be cast back to the correct context when the callback function is called.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_RegisterFeatureCallback(SI_H Handle, const SI_WC* Feature, FeatureCallback Callback, void* Context);


/** Unregisters a specific feature callback if already registered to the selected feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Callback Callback function to be unregistered.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_UnregisterFeatureCallback(SI_H Handle, const SI_WC* Feature, FeatureCallback Callback);


/** Unregisters a specific feature callback with a specific context if already registered to the selected feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Callback Callback function to be unregistered.
 *	@param Context Data pointer that was used when the feature callback was registered with the callback function.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_UnregisterFeatureCallbackEx(SI_H Handle, const SI_WC* Feature, FeatureCallback Callback, void* Context);


/** Sets the value of an integer feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value New integer value to be set.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetInt(SI_H Handle, const SI_WC* Feature, SI_64 Value);


/** Gets the value of an integer feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value Integer pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetInt(SI_H Handle, const SI_WC* Feature, SI_64* Value);


/** Gets the maximum value of an integer feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param MaxValue Integer pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetIntMax(SI_H Handle, const SI_WC* Feature, SI_64* MaxValue);


/** Gets the minimum value of an integer feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param MinValue Integer pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetIntMin(SI_H Handle, const SI_WC* Feature, SI_64* MinValue);


/** Sets the maximum value of an integer feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param MaxValue Integer maximum value to be set.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetIntMax(SI_H Handle, const SI_WC* Feature, SI_64 MaxValue);


/** Sets the minimum value of an integer feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param MinValue Integer minimum value to be set.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetIntMin(SI_H Handle, const SI_WC* Feature, SI_64 MinValue);


/** Sets the value of a floating point (double precision) feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value New double value to be set.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetFloat(SI_H Handle, const SI_WC* Feature, double Value);


/** Gets the value of a floating point (double precision) feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value Double pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetFloat(SI_H Handle, const SI_WC* Feature, double* Value);

/** Gets the maximum value of a floating point feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param MaxValue Double pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetFloatMax(SI_H Handle, const SI_WC* Feature, double* MaxValue);


/** Gets the minimum value of a floating point feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param MinValue Double pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetFloatMin(SI_H Handle, const SI_WC* Feature, double* MinValue);


/** Sets the maximum value of a floating point feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param MaxValue Double maximum value to be set.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetFloatMax(SI_H Handle, const SI_WC* Feature, double MaxValue);


/** Sets the minimum value of a floating point feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param MinValue Double minimum value to be set.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetFloatMin(SI_H Handle, const SI_WC* Feature, double MinValue);


/** Sets the value of a boolean feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value New value to be set.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetBool(SI_H Handle, const SI_WC* Feature, SI_BOOL Value);


/** Gets the value of a boolean feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value Boolean pointer to retrieve the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetBool(SI_H Handle, const SI_WC* Feature, SI_BOOL* Value);


/** Executes a command feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Command name to be executed.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_Command(SI_H Handle, const SI_WC* Feature);


/** Sets the value of a string feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value New value to be set.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetString(SI_H Handle, const SI_WC* Feature, SI_WC* Value);


/** Gets the value of string feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value SI_WC String pointer to retrieve the value.
 *	@param StringLength Maximum allowed length for #Value string.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetString(SI_H Handle, const SI_WC* Feature, SI_WC* Value, int StringLength);


/** Gets the maximum length of a string feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param MaxStringLength Integer pointer to retrieve the value. Note: The length does not include null termination.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetStringMaxLength(SI_H Handle, const SI_WC* Feature, int* MaxStringLength);


/** Gets the maximum string length of an enumerated feature at selected index.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Index Enumeration index.
 *	@param MaxStringLength Integer pointer to retrieve the value. Note: The length does not include null termination.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetEnumStringMaxLength(SI_H Handle, const SI_WC* Feature, int Index, int* MaxStringLength);


/** Sets the index of an enumerated feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value New zero based index to be set. Use #SI_GetEnumCount function to get size of the enumeration. Note: Value is between 0 and size-1.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetEnumIndex(SI_H Handle, const SI_WC* Feature, int Value);


/** Sets the index of an enumerated feature using a string.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param String String value to be set. If the string is found within the enumeration, the index of the enumeration is changed to be off that string. Use #SI_GetEnumStringByIndex and #SI_GetEnumCount to get the all the possible string values that a specified enumerated feature can have.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetEnumIndexByString(SI_H Handle, const SI_WC* Feature, const SI_WC* String);


/** Replaces the string at the current index of an enumerated feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param String New string value to be set.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_SetEnumString(SI_H Handle, const SI_WC* Feature, SI_WC* String);


/** Gets the current selected index of an enumerated feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value Integer pointer to retrieve the current index.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetEnumIndex(SI_H Handle,const  SI_WC* Feature, int* Value);


/** Gets the item count of an enumerated feature.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Value Integer pointer to retrieve the item count.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetEnumCount(SI_H Handle,const  SI_WC* Feature, int* Value);


/** Gets the string of an enumerated feature at the selected index.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Index Enumeration item index.
 *	@param String String pointer to retrieve the value.
 *	@param StringLength Maximum length of #String variable.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetEnumStringByIndex(SI_H Handle,const  SI_WC* Feature, int Index, SI_WC* String, int StringLength);


/** Checks if the selected index at an enumerated feature is available.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Index Enumeration item index.
 *	@param Available Boolean pointer to return the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_IsEnumIndexAvailable(SI_H Handle,const  SI_WC* Feature, int Index, SI_BOOL* Available);


/** Checks if the selected index at an enumerated feature is implemented.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature name.
 *	@param Index Enumeration item index.
 *	@param Implemented Boolean pointer to return the value.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_IsEnumIndexImplemented(SI_H Handle,const  SI_WC* Feature, int Index, SI_BOOL* Implemented);


/** Returns the string representation of an #siError error code.
 *	@param Error #siError error code.
 *	@return Error string for the error code given.
 */
SI_IMPEXP const SI_WC* SI_GetErrorString(int Error);


/******************************************************************************
 * Data acquisition
 ******************************************************************************/

/** Waits for data to be available and fills up the user allocated #BufferAddress buffer with a new frame if data is received before #Timeout (in milliseconds) expires.
 *  @note If a data callback function has been registered using #SI_RegisterDataCallback function, this function will be disabled.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param BufferAddress Pointer to a user allocated buffer.
 *	@param FrameSize Pointer to an integer to retrieve the size of the data buffer. 
 *	@param FrameNumber Pointer to an integer to retrieve the frame number of the received frame.
 *	@param Timeout Timeout in milliseconds. Infinite waiting by default.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_Wait(SI_H Handle, SI_U8* BufferAddress, SI_64* FrameSize, SI_64* FrameNumber, SI_64 Timeout = SI_INFINITE);


/** Certain frame grabbers require allocating the buffers through their own software API's.
 *  This function can be used to create proper memory aligned buffers to be used with #SI_Wait.
 *	@note Buffers created with #SI_CreateBuffer @b must be deleted
 *	using #SI_DisposeBuffer.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param BufferSize Size of the buffer in bytes.
 *	@param BufferAddressPtr Pointer to a pointer to hold the data buffer.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_CreateBuffer(SI_H Handle, SI_64 BufferSize, void** BufferAddressPtr);


/** Disposes a frame buffers created with #SI_CreateBuffer.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param BufferAddress Pointer to the data buffer to be disposed.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_DisposeBuffer(SI_H Handle, void* BufferAddress);


/** Registers a callback function to retrieve the frame data.
 *  @note If a data callback function has been registered using this function, #SI_Wait function will be disabled.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Callback Callback function. Check #DataCallBack for the correct signature.
 *	@param Context Optional data pointer which usually is the class pointer of the caller object so it can be cast back to the correct context when the callback function is called.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_RegisterDataCallback(SI_H Handle, DataCallBack Callback, void* Context);


/** Unregisters the data callback.
 *  @note Enables the usage of #SI_Wait for the selected #Handle.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_UnregisterDataCallback(SI_H Handle);


/** Registers a callback function to retrieve the GPS/INS data.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Callback Callback function. Check #GPSDataCallBack for the correct signature.
 *	@param Context Optional data pointer which usually is the class pointer of the caller object so it can be cast back to the correct context when the callback function is called.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_RegisterGPSDataCallback(SI_H Handle, GPSDataCallback Callback, void* Context);


/** Unregisters the GPS/INS callback.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_UnregisterGPSDataCallback(SI_H Handle, GPSDataCallback Callback, void* Context);


/** Gets the type of a feature as a string.
 *	@param Handle Valid handle opened with #SI_Open.
 *	@param Feature Feature Feature name.
 *	@param String String pointer to retrieve the value.
 *	@param StringLength Maximum length of #String.
 *	@return Returns an #siError error code, #siNoError if function was successful. Check SI_Errors.h for more information and use SI_GetErrorString() function to retrieve the error as a string.
 */
SI_IMPEXP int SI_GetFeatureType(SI_H Handle, const SI_WC* Feature, SI_WC* String, int StringLength);


#endif