#ifndef SI_Errors_H
#define SI_Errors_H

#include "SI_types.h"


/** @file SI_errors.h
 *	This files contains the errors codes, error checking macros and siGetErrorString.
 */

/** Return true if the function failed. Example:
 *	@code
	if(SI_FAILED(SI_Load()))
	{
		//...
	}
 *	@endcode
 */
#define SI_FAILED(stmt) ((stmt) < 0)


/** Return true if the function succeeded. Example:
 *	@code
	if(SI_SUCCEEDED(SI_Load()))
	{
		//...
	}
 *	@endcode
 */
#define SI_SUCCEEDED(stmt) ((stmt) >= 0)


/** If the function fails, SI_CHK sets the last error to a local variable named
 *	nError and go to the Error label. Example:
 *	@code
	int nError = siNoError;
	SI_64 nCount = 0;
	SI_CHK(SI_Load());
	SI_CHK(SI_GetInt(SI_SYSTEM, L"DeviceCount", &nCount));
	//...
Error:
	return nError;
 *	@endcode
 */
#define SI_CHK(stmt) do { nError = (stmt); if (SI_FAILED(nError)) goto Error; } while(0)

#ifdef QT_VERSION
	#ifdef _WIN32
		#define SI_LOGCHK(stmt) do {nError = (stmt); if (SI_FAILED(nError)) { qWarning() << QString("Function '").toLatin1().constData() << (QString(SLOT(stmt)).remove(0,1)).toLatin1().constData() << QString("' in ").toLatin1().constData() << ##__FUNCTION__ << QString(" failed with error code:").toLatin1().constData() << ##stmt << QString("( file:").toLatin1().constData() << ##__FILE__ << QString(", line:").toLatin1().constData() << ##__LINE__ << QString(").").toLatin1().constData(); goto Error; } }while(0)
	#else
	        #define SI_LOGCHK(stmt) do {nError = (stmt); if (SI_FAILED(nError)) { goto Error; } }while(0)
	#endif
#else
	#define SI_LOGCHK(stmt) do {nError = (stmt); if (SI_FAILED(nError)) { std:cout << "Function failed in " << ##__FUNCTION__ << " with error code:" << ##stmt << "( file:" << ##__FILE__ << ", line:" << ##__LINE__ << ")."; goto Error; } }while(0)
#endif

/** Set the last error to a local variable named nError and go to the Error: label. Example:
 *	@code
	int nError = siNoError;
	
	// ...
	if(nIndex > 100)
	{
		SI_GOTO(siOutOfRange);
	}
	
	//...
Error:
	return nError;
 *	@endcode
 */
#define SI_GOTO(stmt) { nError = (stmt); goto Error; }
//#define SI_GOTO(stmt) do { nError = (stmt); goto Error; } while(0)

/** Used to check if pointers are null before accessing them. If the pointer is null, siInvalidPointer error will be set and the code will go to the Error label.
 */
#define SI_NULLCHK(statement) if (statement == 0) { nError = siInvalidPointer; goto Error; }

/** SpecCore errors. Error codes are negative values, 0 is succeeded and positive 
 *	values are warnings.
 */
enum siError
{
	siRtNoError = 0,									/**< Success */
	siRtSessionNameAlreadyExists = -4000,				/**< Failed to create session since another session with the same name already exists*/
	siRtScriptFunctionDoesNotExists = -4002,			/**< The passed script function does not exist. */
	siRtSessionNotInitialized = -4003,					/**< The session has not been initialize. Call 'initialize'.*/
	siRtScriptErrorError = -4004,						/**< */
	siRtAlreadySubscribingToEvent = -4005,				/**< A matching subscription has already been registered for the event */
	siRtWaitForPreviousEventCompleteTimeout = -4006,	/**< Timed out while waiting for the previous event invoke to finish executing */
	siRtSessionRunning = -4010,							/**< Unable to complete the previous call since the session is already executing another instruction set. */
	siRtAcquireAccessTimeout = -4011,					/**< Exclusive access to the require object did not occur within a give time period */
	siRtUnspecifiedError = -4012,						/**< */
	siRtInvalidSessionHandle = -4014,					/**< */
	siRtScriptError = -4015,							/**< */
	siRtSessionAlreadyInitialized = -4016,				/**< */
	siRtNullPointerArgument = -4018,					/**< The passed script state pointer was a null. */
	siRtNoScriptStateSet = -4021,						/**< Unable to start execution since no script state has been set. */
	siRtScriptMemoryAllocFailure = -4022,				/**< Failed to allocate the specified amount of memory */
	siRtScriptScriptGCFailed = -4023,					/**< An error occured while running script garbage collection */
	siRtScriptRuntimeError = -4024,						/**< A runtime error occured while executing the script. Check script state stack for error message */
	siRtScriptTerminatedByForce = -4025,				/**< User terminated the script before it had a chance to complete */
	siRtFailedToAllocateMemory = -4026,					/**< Malloc failed.*/
	siRtScriptSyntaxError = -4027,						/**< Failed to pre-compile script due to syntax error(s) */
	
	// si_remote errors
	siRemoteDynamicSetFailed = -11000,		/**< Remote dynamic set failed. */
	siRemoteDynamicGetFailed = -11001,		/**< Remote dynamic get failed. */

	// 10000 HOST errors
	siHOSTInvalidLicence = -10000,					/**< HOST Invalid Licence. */
	siHOSTLicenceExpired = -10001,					/**< HOST Licence Expired. */
	siHOSTLicenceNotActivated = -10002,				/**< HOST Licence Not Activated. */
	siHOSTInvalidHostHandle = -10003,				/**< HOST Invalid Host Handle. */
	siHOSTInvalidPeerHandle = -10004,				/**< HOST Invalid Peer Handle. */
	siHOSTInvalidCallback = -10005,					/**< HOST Invalid Notification Callback Function. */
	siHOSTCreateServerFailed = -10006,				/**< HOST CreateServer Failed. */
	siHOSTCreateClientFailed = -10007,				/**< HOST CreateClient Failed. */
	siHOSTConnectFailed = -10008,					/**< HOST Peer Connect Failed. */
	siHOSTDisconnectFailed = -10009,					/**< HOST Peer Disconnect Failed. */
	siHOSTDisconnectPeersFailed = -10010,			/**< HOST Peer Disconnect Failed. */
	siHOSTSetPeerDataFailed = -10011,				/**< HOST Set Peer Data Failed. */
	siHOSTSendPeerFailed = -10012,					/**< HOST Send Peer Failed. */
	siHOSTPeerPingFailed = -10013,					/**< HOST Peer Ping Failed. */
	siHOSTBroadcastPeersFailed= -10014,				/**< HOST Broadcast Host Failed. */
	siHOSTGetPeerDataFailed = -10015,				/**< HOST Get Peer Data Failed. */
	siHOSTSetHostDataFailed = -10016,				/**< HOST Set Host Data Failed. */
	siHOSTGetHostDataFailed = -10017,				/**< HOST Get Host Data Failed. */
	siHOSTGetHostListFailed = -10018,				/**< HOST Get Host List Failed. */
	siHOSTGetPeerListFailed = -10019,				/**< HOST Get Peer List Failed. */
	siHOSTGetPeerListXMLFailed = -10020,			/**< HOST Get Peer List XML Failed. */
	siHOSTGetHostStatsFailed = -10021,				/**< HOST Get Host Stats Failed. */
	siHOSTSetHostStatsFailed = -10022,				/**< HOST Set Host Stats Failed. */
	siHOSTGetHostStructFailed = -10023,				/**< HOST Get Host Struct Failed. */
	siHOSTGetPeerStructFailed = -10024,				/**< HOST Get Peer Struct Failed. */
	siHOSTSetDropOnTimeoutsFailed = -10025,			/**< HOST Set Drop On Timeouts Failed. */
	siHOSTGetDropOnTimeoutsFailed = -10026,			/**< HOST Get Drop On Timeouts Failed. */
	siHOSTSetHostTimeoutsFailed = -10027,			/**< HOST Set Host Timeouts Failed. */
	siHOSTGetHostTimeoutsFailed = -10028,			/**< HOST Get Host Timeouts Failed. */
	siHOSTMessageHasNoHeader = -10029,				/**< HOST Message Has no Header. */
	siHOSTMessageHasInvalidHeaderSync = -10030,		/**< HOST Message Has Invalid HeaderSync. */
	siHOSTMessageHasInvalidVersionNumber = -10031,	/**< HOST Message Has Invalid Version Number. */
	siHOSTSetHostCompressionFailed = -10032,			/**< HOST SetCompression Failed. */
	siHOSTGetHostCompressionFailed = -10033,			/**< HOST GetHostCompression Failed.*/
	siHOSTConnectionInProgress = -10034,				/**< HOST Connection In Progress.*/
	siHOSTMissingApplicationsDefinition = -10035,    /**< HOST Applications definition is missing. */
	siHOSTHostUninitialised = -10036,				/**< HOST Host Uninitialised. */
	siHOSTPeerNotFound = -10037,						/**< HOST Peer Not Found. */
	siHOSTThreadNotSafe = -10038,					/**< HOST Thread Not Safe - Parent != Current. */
	siHOSTNotAServer = -10039,					/**< HOST Not A Server. */
	siHOSTNotAClient = -10040,					/**< HOST Not A Client. */
	siHOSTAddressNotAvailable = -10041,			/**< HOST Address Not Available. */
	siHOSTNotCreated = -10042,					/**< HOST Not Created. */
	siHOSTNotAFileTransferPeer = -10043,			/**< HOST Not a file transfer peer. */
	siHOSTNotAControlPeer = -10044,					/**< HOST Not a Control peer. */
	siHOSTNotADataPeer = -10045,					/**< HOST Not a Data peer. */
	siHOSTFileNotFound = -10046,					/**< HOST File not found. */
	siHOSTFileCreationFailed = -10047,			/**< HOST File creation failed. */
	siHOSTInsufficientSpace = -10048,		/**< HOST Insufficient Space. */
	siHOSTFileTransferInProgress = -10049,		/**< HOST File Transfer in Progress. */
	siHOSTFileTransferIdle = -10050,		/**< HOST File Transfer Idle. */
	siHOSTFileTransferStatusCallbackUndefined = -10051, /**< HOST File Transfer Status Callback undefined. */
	siHOSTFileTransferFileAccessPermissions = -10052, /**< HOST File Transfer Access permissions. */
	siHOSTFileTransferFileErrorUnspecified = -10053, /**< HOST File Transfer Error unspecified. */
	siHOSTFileTransferDataConnectTimeout = -10054,    /**< File Transfer DataConnect Timeout. */
	siHOSTDataPeerConnectTimeout = - 10055,  /**< DataPeer Connect Timeout. */
	siHOSTControlServerNotListening = - 10056,  /**< DataServer Not Listening. */
	siHOSTDataServerNotListening = - 10057,  /**< DataServer Not Listening. */
	siHOSTQFileReadError = -10058,	/**< QFile ReadError. */
	siHOSTQFileWriteError = -10059,	/**< QFile WriteError. */
	siHOSTQFileFatalError = -10060,	/**< QFile FatalError. */
	siHOSTQFileResourceError = -10061,	/**< QFile ResourceError. */
	siHOSTQFileOpenError = -10062,	/**< QFile OpenError. */
	siHOSTQFileAbortError = -10063,	/**< QFile AbortError. */
	siHOSTQFileTimeOutError = -10064, /**< QFile TimeOutError. */
	siHOSTQFileUnspecifiedError = -10065, /**< QFile UnspecifiedError. */
	siHOSTQFileRemoveError = -10066, /**< QFile RemoveError. */
	siHOSTQFileRenameError = -10067, /**< QFile RenameError. */
	siHOSTQFilePositionError = -10068, /**< QFile PositionError. */
	siHOSTQFileResizeError = -10069, /**< QFile ResizeError. */
	siHOSTQFilePermissionsError = -10070, /**< QFile PermissionsError. */
	siHOSTQFileCopyError = -10071, /**< QFile CopyError. */
	siHOSTFileCreateFailed = -10072, /**< OS File Create Failed. */
	siHOSTFileOpenFailed = -10073, /**< OS File Open Failed. */
	siHOSTQuotaExceeded = -10074, /**< OS Storage Quota exceeded. */
	siHOSTDriveFull = -10075, /**< OS Storage Drive Full. */
	siHOSTDriveDisconnected = -10076, /**< OS Storage Drive Disconnected. */
	siHOSTAccessDenied = -10077, /**< OS Storage Access Denied. */
	siHOSTUnmappedOsError = -10078, /**< OS Unmapped Error. */
	siHOSTPeerNotConnected = -10079, /**< Peer NOT Connected. */


	// 9000 SMP Profile and Group errors
	siGroupNotFound = -9000,							/**< The Group cannot be found. (SMP)*/
	siMissingGroupsDefinition = -9001,					/**< The profile Groups definition is missing. */
	siMissingInvokesDefinition = -9002,					/**< The profile Invokes definition is missing. */
	siEventGroupNotFound = -9003,						/**< The EventGroup not found. */
	siEventInvokeNotFound = -9004,						/**< The EventInvoke not found. */
	siMissingEventGroupsDefinition = -9005,				/**< The profile EventGroups definition is missing. */
	siMissingEventInvokesDefinition = -9006,			/**< The profile EventInvokes definition is missing. */
	siGroupFeatureIndexOutOfRange = -9007,				/**< The GroupFeatureIndex is out of Range. */
	siGroupAlreadyExists = -9008,				        /**< The Group Name already exists. */
	siGroupMissingName = -9009,							/**< The Group Name is missing. */
	siGroupNumberMissing = -9010,						/**< The Group Message Number missing. */
	siGroupInvalidInterval = -9011,						/**< Invalid Group Interval. */
	siResponseEmpty = -9012,							/**< Response Empty. */
	siHostMessageResponseNullPointer = -9013,			/**< HostMessageResponse Null Pointer. */
	siHostMessageResponseTimeout = -9014,			    /**< HostMessageResponse Time out. */
	siHostMessageCompletionTimeout = -9015,			    /**< HostMessageResponse Time out. */
	siHostMessageNoPeerSpecified = -9016,			    /**< HostMessage No Peer Specified. */

	// 1700 QAQC errors
	siQAQCDarkerThanReference = -1700,				/**< QAQC darker than reference.*/
	siQAQCBrighterThanReference = -1701,			/**< QAQC brighter than reference.*/
	siQAQCUnevenDistribution = -1702,				/**< QAQC uneven distribution detected.*/
	siQAQCIncompatible = -1703,						/**< Incompatible reference. */

	// 1600 Calibration errors
	siCalibrationFailed = -1600,					/**< Calibration failed on the sensor.*/
	siInvalidCalibrationPack = -1601,				/**< Invalid calibration pack.*/

	// 1500 Motor errors
	siMotorInitFailed = -1500,						/**< Failed to initialize the selected motor.*/
	siMotorReadTimeout = -1501,						/**< Motor read function timed out.*/
	siMotorWriteTimeout = -1502,					/**< Motor write function timed out.*/
	siMotorIOError = -1503,							/**< Motor IO error.*/
	siMotorDataError = -1504,						/**< Motor data error.*/
	siMotorProgramError = -1505,					/**< Motor program error.*/
	siMotorCommunicationError = -1506,				/**< Motor communication error.*/
	siMotorSystemError = -1507,						/**< Motor system error.*/
	siMotorMotionError = -1508,						/**< Motor motion error.*/
	siMotorStepError = -1509,						/**< Motor AccuStep error.*/

	// 1400 Licensing errors
	siLicensingInitError = -1400,					/**< Call LoadLicense successfully before using other functionality.*/
	siLicensingErrorOpeningSharedMemory = -1401,	/**< Error opening a shared memory for licensing.*/
	siLicensingHashNotSupported = -1402,			/**< Selected hash function is not supported.*/
	siLicensingCipherNotSupported = -1403,			/**< Selected cipher function is not supported.*/
	siLicensingEncryptionFailed = -1404,			/**< Licensing encryption failed.*/
	siLicensingDecryptionFailed = -1405,			/**< Licensing decryption failed.*/
	siLicensingFileNotFound = -1406,				/**< Licensing related file not found.*/
	siLicensingEmptyLicense = -1407,				/**< Empty license file.*/
	siLicensingExpired = -1408,						/**< License expired.*/
	siLicensingProductExpired = -1409,				/**< Product expired.*/
	siLicensingModuleExpired = -1410,				/**< Module expired.*/
	siLicensingFeatureExpired = -1411,				/**< Feature expired.*/
	siLicensingApplicationNotFound = -1412,			/**< Application license not found.*/
	siLicensingModuleNotFound = -1413,				/**< Module license not found.*/
	siLicensingFeatureNotFound = -1414,				/**< Feature license not found.*/
	siLicensingSyntaxError = -1415,					/**< Licensing syntax error.*/
	siLicensingTimestampError = -1416,				/**< Licensing real-time timestamp error.*/
	siLicensingSupportExpired = -1417,				/**< License support expired. */

	// 1300 SCB errors
	siSCBChecksumFailed = -1300,	/**< SCB Checksum error.*/
	siSCBReadFailed = -1301,		/**< SCB Read failed.*/
	siSCBWriteFailed = -1302,		/**< SCB Write failed.*/
	siSCBFlashReadFailed = -1303,	/**< SCB Read flash failed.*/
	siSCBFlashWriteFailed = -1304,	/**< SCB Write flash failed.*/
	siSCBFlashEraseFailed = -1305,	/**< SCB Erase flash failed.*/
	siSCBNotInitialized = -1306,	/**< SCB Not Initialized.*/

	// 1200 Camera errors
	siCameraInvalidChannel = -1200, /**< Invalid camera channel */
	siCameraSerialCommunicationFailure = -1201,	/**< Serial communication failure in camera. */
	siCameraCoolingNotCompleted = -1202,
	siCameraConnectingError = -1203, /** < Camera connection failure. The camera may be already in use. */
	siCameraNotFound = -1204,

	// 1100 Communication errors
	siCommunicationFailure = -1100, /**< Communication failure. */
	siCommunicationTimeout = -1101, /**< Communication timeout. */
	siCommunicationOverrun = -1102, /**< Communication buffer overrun. */
	siCommunicationCRC = -1103, /**< Communication CRC error. */
	siCommunicationNotAcknowledged = -1104, /**< Communication not acknowledged. */
	siCommunicationPortInUse = -1105, /**< Communication port in use. */
	siCommunicationBaudRateNotSupported = -1106, /**< Baud rate not supported. */
	siCommunicationBusy = -1107, /**< Communication is busy.*/

	// 1000 file storage errors
	siStorageTimeout = -1000,			/**< Storage timeout. */
	siStoragePendingFileOpen = -1001,	/**< Storage has pending file open. */
	siStoragePendingFileClose = -1002,	/**< Storage has pending file close. */
	siStorageFailedFileOpen = -1003,	/**< Storage file open failed. */
	siStorageFailedFileClose = -1004,	/**< Storage file close failed. */
	siStorageFileNotOpen = -1005,		/**< Storage file not open. */
	siStorageWriteFailed = -1006,		/**< Storage write failed. */
	siStorageReadFailed = -1007,		/**< Storage read failed. */
	siStorageDriveFull = -1008,			/**< Storage drive full. */
	siStorageDriveDisconnected = -1009,	/**< Storage drive disconnected. */
	siStorageAccessDenied = -1010,		/**< Storage access denied. */
	siStorageQuotaExceeded = -1011,		/**< Storage quota exceeded. */
	siStorageFileNotFound = -1012,		/**< Storage file not found. */

	// 900 ring buffer errors
	siRingBufferNotInitialized = -900,	/**< Buffer is not initialized. */
	siRingBufferInvalidHandle = -901,	/**< Invalid reader handle.*/
	
	// 800 header file errors
	siHeaderInconsistentHeader = -800, /**<  Inconsistent header file. */
	siHeaderParseError = -801, /**< The header file has a parsing error. */
	siHeaderKeywordAlreadyExists = -802, /**< The header keyword already exists. */
	siHeaderInvalidType = -803, /**< Invalid type for this keyword. */
	siHeaderInvalidData = -804, /**< Invalid data for this keyword. */

	// 700 acquisition errors
	siAcquisitionTimeout = -700, /**< The acquisition has timed out. */
	siAcquisitionAborted = -701, /**< The acquisition was aborted. */
	siAcquisitionBufferAlreadyAllocated = -702, /**< Buffer already allocated. First delete the memory using #SI_DisposeBuffer. */
	siAcquisitionInvalidBuffer = -703, /**< Invalid buffer. The buffer is either NULL or has not been created using #SI_CreateBuffer. */
	siAcquisitionInProgress = -704, /**< Acquisition in progress. */
	siAcquisitionMultigrabbingInvalidWidth = -705, /**<	Multi-grabbing widths don't match. */
	siAcquisitionNotInProgress = -706,	/**< Acquisition is not in progress.*/
	
	// 600 module errors
	siModuleNotFound = -600, /**< The module cannot be found. */
	siModuleAlreadyDeclaredWithDifferentSignature = -601, /**< The module is already declared but with a different signature. */
	siModuleUnresolvedSymbol = -602, /**< The module symbol could not be resolved. */
	siModuleNotLoaded = -603, /**< The module is not loaded. */
	siModuleLoadFailed = -604, /**< Loading the module failed. */
	siModuleNotRegistered = -605, /**< The module is not registered. */

	// 500 command errors
	siCommandNotFound = -500, /**< The command cannot be found */
	siCommandNotAvailable = -501, /**< The command is not available */
	siCommandInvokeFailed = -502, /**< The invoke command failed */

	// 400 Feature errors
	siFeatureNotFound = -400,  /**< The feature cannot be found. */
	siFeatureNotImplemented = -401,  /**< The feature is not implemented for this device. */
	siFeatureReadOnly = -402, /**< The feature is read only and cannot be written. */
	siFeatureNotWritable = -403, /**< The feature it not writable at this point of time. */
	siFeatureNotReadable = -404, /**< The feature is not readable at this point of time. */
	siFeatureValueOutOfRange = -405, /**< The value specified for this value is out of range. */
	siFeatureValueNotSet = -406,  /**< The value for this feature is not set. */
	siFeatureEmptyEnumeration = -407,  /**< The feature has its enumeration empty */
	siFeatureMissingFeaturesDefinition = -408,  /**< The profile features definition is missing. */
	siFeatureAlreadyExists = -409, /**< The feature already exists */
	siFeatureInvalidEnumerationString = -410, /**< The enumeration string is invalid. */
	siFeatureGroupNotFound = -411,  /**< The feature Group cannot be found. */
	siFeatureEnumStringNotFound = -412, /**< The string was not found in the enumerated feature. */
	siFeatureMissingType = -413,  /**< The Feature type is missing. */
	siFeatureInvalidType = -414,  /**< The Feature type is invalid. */
	siFeatureMissingName = -415,  /**< The Feature missing name. */
	siFeatureLockTimeout = -416,  /**< The operation on the feature timed out. */

	// 300 sensor profiles errors
	siProfileInvalidDirectory = -300, /**< The (SSP) Specim Sensor Profiles directory is invalid or does not exist. */
	siProfileInvalidDocumentType = -301, /**< An invalid document type has been specified for this profile. */
	siProfileInvalidModuleDefinition = -302, /**< An invalid module definition has been specified for this profile. */
	siProfileInvalidType = -303, /**< An invalid type has been specified for this profile. */
	siProfileInvalidModule = -304, /**< The profile has an invalid module. A module definition cannot be null. */
	siProfileNotOpen = -305, /**< The profile handle is not opened. */
	siProfileMissingName = -306, /**< The name of the profile is missing. */
	siProfileParseError = -307, /**< The profile has some parsing error(s). */
	siProfileNotSet = -308, /**< The profile is not set. */


	// 200 XML error group
	siXmlParseFailed = -200, /**< The XML parsing failed. */
	siXmlStringNotSet = -201, /**< The XML string is not set. */
	siXmlTagNotFound = -202,  /**< The XML tag was not found. */

	// 100 file errors group
	siFileOpenFailed = -100, /**< Could not open the file. */
	siFileCreateFailed = -101, /**< Could not create the file. */
	siFileReadFailed = -102, /**< Could not read the file. */
	siFileNotOpened = -103, /**< File not opened. */
	siFileEOF = -104, /**< End of file. */
	siFileCRC = -105, /**< Cyclic Redundancy Check error. */
	siFileWriteFailed = -106, /**< File write failed. */
	
	siIncompleteCommand = -26, /**< Incomplete command */
	siInvalidCommand = -25, /**< Invalid command */
	siNotAllowed = -24, /**< Not allowed */
	siValueNotSet = -23, /**< Value not set */
	
	siFailedToLoadDriver = -22, /* Failed to load driver.*/
	siUnmappedOsError = -21, /**< Unmapped OS Error. */
	siExceptionOccured = -20, /**< An exception occurred and was handled.*/
	siAbort = -19, /**< Generic abort event */
	siInvalidChecksum = -18, /**< Invalid checksum. */
	siOutOfRangeMin = -17, /**< Value is smaller than its minimum value. */
	siOutOfRangeMax = -16, /**< Value is greater than its maximum value. */
	siInvalidFile = -15, /**< Invalid file. */
	siNotInitialized = -14, /**< Not initialized. */
	siBufferTooSmall = -13, /**< Buffer too small. */
	siOutOfMemory = -12, /**< Out of memory. */
	siNotSupported = -11, /**< Not supported. */
	siUnknownError = -10, /**< Unknown error.*/
	siInvalidParameter = -9, /**< Invalid parameter. */
	siInvalidDeviceType = -8, /**< Invalid device type. */
	siOutOfRange = -7, /**< Out of range. */
	siInvalidPointer = -6, /**< Invalid pointer. */
	siInvalidHandle = -5, /**< Invalid handle. */
	siInvalidType = -4, /**< Invalid type. */
	siNotFound = -3, /**< Not found. */
	siNotImplemented = -2, /**< Not implemented. */
	siFailed = -1, /** Failed. */
	siNoError = 0, /**< No error. */
	siAlreadyInitialized = 1, /**< Already initialized. */
	siAlreadyExists, /**< Already exists. */
	siUnknownWarning, /**< Unknown warning. */
	siIsEmpty,			/**< Trying to access an empty object.*/
	
	// Profiles warnings
	siProfileNoDescription = 300, /**< It is recommended but not mandatory to put a description for the sensor profile. */

	// Feature warnings
	siFeatureNotAvailable = 401,	/**< Feature is not currently available.*/

	// Modules warnings
	siModuleAlreadyRegistered = 600, /**< The module is already registered. */
	siModuleAlreadyLoaded = 601, /**< The module is already loaded */

	// Acquisition warning
	siAcquisitionWaitingForFrames = 700,	/**< Acquisition is waiting for an incoming frame. */

	// Ring buffer warnings
	siRingBufferWaitWriter = 900,		/**< The reader must wait for the writer to continue. */

	// 1200 Camera warnings
	siReadoutModeChangedToIWR = 1200,	/**< The detector readout mode has changed to IWR.*/
	siReadoutModeChangedToITR = 1201,	/**< The detector readout mode has changed to ITR.*/
	siBPRMapLoadingFailed = 1202,		/**< Bad pixel map loading failed. Possibly due to invalid dimensions. */

	// 1500 Motor warnings
	siMotorPlusLimitSwitchReached = 1500,
	siMotorMinusLimitSwitchReached = 1501,


	// Calibration warnings
	siCalibrationMissing = 1600,		/**< Calibration is missing.*/
	

	// 9000 SMP Profile and Group Warnings
	siHostMessageResponseNotFound = 9000,	    /**< HostMessageResponse pointer not found */

	// HOST Warnings
	siHOSTMessageNoHeader = 10000,	/**< HOST Message Has No Header. */
	siHOSTMessagePrivate = 10001,	/**< HOST Message is Private. */
	siHOSTMessageUser = 10002,		/**< HOST Message is User. */
};


/** Returns the error code's string. 
 *	@param _nError siError error code.
 *	@return Error string for the error code given _nError.
 */
const SI_WC* siGetErrorString(int _nError);

#endif
