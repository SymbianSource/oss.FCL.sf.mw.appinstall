/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     CDownloadHandler implementation
*
*
*/


#include <aknappui.h>
#include <COEUTILS.H>
#include <wlanerrorcodes.h> //For download error handling
#include <etelpckt.h>       //For download error handling
#include <exterror.h>       //For download error handling
#include <httpdownloadmgrcommon.h> // For download error handling (not in S60 5.0 SDK)

#include "ADM.hrh"
#include "ADMDownloadHandler.h"
#include "ADMAppUi.h"
#include "networkstatuslistener.h"

#include "globals.h"
#include "macros.h"
/*
#ifdef USE_LOGFILE
#include "debug.h"
#undef LOG
#undef LOG2
#undef LOG3
#undef LOG4
#undef LOG5
#undef LOG6
#undef LOG7
#undef LOG8
#undef LOG8_2

#define LOG( aMsg ) { _LIT(KMsg, aMsg); iAppUi.iLog.Write( KMsg ); RDebug::Print( KMsg ); }
#define LOG2( aMsg, aParam1 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1 ); RDebug::Print( KMsg, aParam1 ); }
#define LOG3( aMsg, aParam1, aParam2 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2 ); RDebug::Print( KMsg, aParam1, aParam2 ); }
#define LOG4( aMsg, aParam1, aParam2, aParam3 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3 ); }
#define LOG5( aMsg, aParam1, aParam2, aParam3, aParam4 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4 ); }
#define LOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); }
#define LOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); }
#define LOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); }
#define LOG8_2( aMsg, aParam1 ) { _LIT8(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1 ); RDebug::Print(_L("RDebug _L8() at line %d"), __LINE__); }
#endif
*/
#ifdef USE_LOGFILE
// Logging version
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CDownloadHandler* CDownloadHandler::NewL(MDownloadClient* aDlClient, RFileLogger& aLogger, const TBool aMasterInstance)
	{
	CDownloadHandler* object = new ( ELeave ) CDownloadHandler(aDlClient, aLogger, aMasterInstance);
	CleanupStack::PushL( object );
	object->ConstructL();
	CleanupStack::Pop();
	return object;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CDownloadHandler::CDownloadHandler(MDownloadClient* aDlClient, RFileLogger& aLogger, const TBool aMasterInstance) :
	iLog(aLogger),
	iDlClient(aDlClient),
	iMasterInstance(aMasterInstance),
	iPtrToResponseBodyFileName(0,0)
	{
	}
#else
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CDownloadHandler* CDownloadHandler::NewLC(MDownloadClient* aDlClient, const TBool aMasterInstance)
	{
	CDownloadHandler* object = new ( ELeave ) CDownloadHandler(aDlClient, aMasterInstance);
	CleanupStack::PushL( object );
	object->ConstructL();
	return object;
	}

CDownloadHandler* CDownloadHandler::NewL(MDownloadClient* aDlClient, const TBool aMasterInstance)
	{
	CDownloadHandler* object = CDownloadHandler::NewLC(aDlClient, aMasterInstance);
	CleanupStack::Pop();
	return object;
	}

CDownloadHandler::CDownloadHandler(MDownloadClient* aDlClient, const TBool aMasterInstance) :
	iDlClient(aDlClient),
	iMasterInstance(aMasterInstance),
	iPtrToResponseBodyFileName(0,0)
	{
	}
#endif

// ---------------------------------------------------------------------------
// Default destructor
// ---------------------------------------------------------------------------
//
CDownloadHandler::~CDownloadHandler()
	{
	DELETE_IF_NONNULL( iUrl );
	DELETE_IF_NONNULL( iResponseBodyFileName );

	delete iNetworkStatusListener;
	iDownloadManager.DeleteAll();
	iDownloadManager.Close();
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CDownloadHandler::ConstructL()
	{
//	TBool masterInstance(!(iADMAppUiObj->EikonEnv()->StartedAsServerApp()));
	const TUid uid = {_UID3};
	iDownloadManager.ConnectL(uid , *this, iMasterInstance );
	iNetworkStatusListener = CNetworkStatusListener::NewL( *this );
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CDownloadHandler::SetIAP(TUint32 aIAP)
	{
	iIAP = aIAP;
	if ( iIAP > 0)
		{
		LOG2( "CDownloadHandler::SetIap(): %d", aIAP );
		// Set the download manager to silent mode
		iDownloadManager.SetBoolAttribute( EDlMgrSilentMode, ETrue );
		iDownloadManager.SetIntAttribute( EDlMgrIap, iIAP );
		iNetworkStatusListener->SetIapIdToMonitor(iIAP);
		}
	}

#ifdef USE_LOGFILE
void CDownloadHandler::Log(TRefByValue<const TDesC16> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, list);
	iLog.Write(buf);
	}
#endif

// -----------------------------------------------------------------------------
// Callback for CNetworkStatusObserver
//
// Called when the connection status changes
// -----------------------------------------------------------------------------
//
void CDownloadHandler::HandleNetworkStatusChangedL(const TInt aStatus)
	{
	LOG2( "+ HandleNetworkStatusChangedL(): %d", aStatus );

	// Did we lose the network connection?
	if (aStatus == EConnMonStatusNotAvailable)
		{
		iNetworkLost = ETrue;
		}

	//LOG2( "- HandleNetworkStatusChangedL(): %d", iNetworkLost );
	}

// -----------------------------------------------------------------------------
// Processes download errors and sets aDownloadError
//
// @param aGlErrId Global error ID from download manager.
// @param aErrId Error ID from download manager.
// @param aDownloadError Reference to the download error variable function is
//      going to set in case of known error.
// -----------------------------------------------------------------------------
//
TBool CDownloadHandler::ProcessDlErrors(const TInt32 aGlErrId, const TInt32 aErrId, EDownloadError& aDownloadError)
	{
	if (iFetchType && (aGlErrId || aErrId))
		LOG4( "+ ProcessDlErrors(%d, %d, %d)", aGlErrId, aErrId, aDownloadError );
	TBool requestRestart = EFalse;
	// Process the error codes
	switch (aErrId)
		{
		case KErrNone:
			// everything a-OK!
			break;
		case EConnectionFailed:
			aDownloadError = EDlErrDownloadFailure;
			break;
		case ETransactionFailed:
			// We set the download error to network failure as that's the
			// error propagated to TState::DownloadFailed().
			// The state machine will try to restart the download, if the
			// error code is EDlErrDownloadFailure, but NOT for EDlErrNetworkFailure.
			aDownloadError = EDlErrDownloadFailure;
			requestRestart = ETrue;
			break;
		case EObjectNotFound:
			aDownloadError = EDlErrFileNotFound;
			break;
		default:
			LOG2( "Unhandled ErrId %d", aErrId );
			aDownloadError = EDlErrDownloadFailure;
			break;
		}
	// Process the global error code
	switch (aGlErrId)
		{
		case KErrNone:
			// everything a-OK!
			break;
		case KErrCancel:
		case KErrAbort:
			// The user has cancelled the download / IAP selection box
			aDownloadError = EDlErrCancelled;
			break;
		case KErrDiskFull:
			// Disk full
			aDownloadError = EDlErrNotEnoughSpace;
			break;
		case KErrUnknown:
			// act on KErrUnknown only, if error ID is not set
			if (aErrId == KErrNone)
				{
				aDownloadError = EDlErrDownloadFailure;
				requestRestart = ETrue;
				}
			break;
		case KErrNotFound:
		case KErrBadName:
		case KErrNotSupported:
		case KErrCommsLineFail:
		case KErrTimedOut:
		case KErrCouldNotConnect:
		case KErrDisconnected:
		case KErrGprsServicesNotAllowed:
		case KErrGsmMMNetworkFailure:
		// WLAN network related errors:
		case KErrWlanNetworkNotFound:
		case KErrWlanRoamingFailed:
		case KErrWlanNetworkLost:
#if 0
		case KErrBadName:
		case KErrNotSupported:
		case KErrWlanOpenAuthFailed:
		case KErrWlanSharedKeyAuthRequired:
		case KErrWlanSharedKeyAuthFailed:
		case KErrWlanWpaAuthRequired:
		case KErrWlanIllegalEncryptionKeys:
		case KErrWlanPskModeRequired:
		case KErrWlanEapModeRequired:
		case KErrWlanSimNotInstalled:
		case KErrWlanNotSubscribed:
		case KErrWlanAccessBarred:
		case KErrWlanPasswordExpired:
		case KErrWlanNoDialinPermissions:
		case KErrWlanAccountDisabled:
		case KErrWlanRestrictedLogonHours:
		case KErrWlanServerCertificateExpired:
		case KErrWlanCerficateVerifyFailed:
		case KErrWlanNoUserCertificate:
		case KErrWlanNoCipherSuite:
		case KErrWlanUserRejected:
		case KErrWlanUserCertificateExpired:
		// less fatal, but still needs to fail the download:
		case KErrWlanWpaAuthFailed:
		case KErrWlan802dot1xAuthFailed:
		case KErrWlanIllegalWpaPskKey:
		case KErrWlanEapSimFailed:
		case KErrWlanEapTlsFailed:
		case KErrWlanEapPeapFailed:
		case KErrWlanEapMsChapv2:
		case KErrWlanEapAkaFailed:
		case KErrWlanEapTtlsFailed:
		case KErrWlanLeapFailed:
		case KErrWlanEapGtcFailed:
#endif
			// A fatal network error has occured, don't retry the download
			requestRestart = EFalse;
			aDownloadError = EDlErrNetworkFailure;
			break;
		default:
			if (!requestRestart)
				{
				// We assume all the other error codes to be 'hard' network errors
				LOG2( "Unhandled GlErrId %d", aGlErrId );
				aDownloadError = EDlErrNetworkFailure;
				}
			break;
		}
	if (iFetchType && aDownloadError != ENoError)
		LOG3( "- ProcessDlErrors(): %d, %d", requestRestart, aDownloadError );
	return requestRestart;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CDownloadHandler::HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
	{
	TInt32 glErrId = KErrNone;
	TInt32 errId = ENoError;

	// Get error IDs
	aDownload.GetIntAttribute( EDlAttrGlobalErrorId, glErrId );
	aDownload.GetIntAttribute( EDlAttrErrorId, errId );

#ifdef DO_LOG
	if (iFetchType)
		{
		_LIT(KFmt, ", GlErrId=%6d, ErrId=%d");
		TBuf<64> buf;

		if ( ((glErrId != KErrNone) || (errId != ENoError)) && glErrId != KErrUnknown )
			{
			buf.AppendFormat(KFmt, glErrId, errId);
			}

		if ( ( iDownloadState != aEvent.iDownloadState ) ||
			 ( iProgressState != aEvent.iProgressState ) )
			{
			iDownloadState = aEvent.iDownloadState;
			iProgressState = aEvent.iProgressState;

			LOG5( "DlSt=%5d, PrSt=%5d, L=%d%S", iDownloadState, iProgressState, iNetworkLost, &buf );
			}
		}
#endif

	if (iNetworkLost && !iCancelled)
		{
		TBuf<KMaxPath> fn;
		aDownload.GetStringAttribute( EDlAttrDestFilename, fn );
		iDownloadError = EDlErrNetworkFailure;
		LOG2( "Connection lost! Cancelling '%S' download.", &fn );
		DoHandleHttpFetchFailure(fn, glErrId, errId);
		return;
		}

	switch ( aEvent.iDownloadState )
		{
		case EHttpDlCreated:
			{
			break;
			}
		case EHttpDlInprogress:
			{
			// Already downloaded content size
			TInt32 downloadedSize( 0 );
			// Total download size
			TInt32 dlLength( 0 );

			if ( aEvent.iProgressState == EHttpProgConnected )
				{
				TInt32 iap;
				// Get the selected IAP (ignore any errors from Get())
				iDownloadManager.GetIntAttribute( EDlMgrIap, iap );
				if (iIAP != iap)
					{
					LOG2( "* Using IAP %d", iap );
					iIAP = iap;
					iDlClient->HandleIapChanged(iIAP);
					}
				}

			aDownload.GetIntAttribute( EDlAttrDownloadedSize, downloadedSize );
			aDownload.GetIntAttribute( EDlAttrLength, dlLength );

			UpdateDownloadSpeed(downloadedSize);

			iDlClient->HandleHttpFetchInProgress( downloadedSize, dlLength, iDlAvgSpeed );
			break;
			}
		case EHttpDlPaused:
		case EHttpDlFailed:
// These two fill fold to EHttpDlFailed. We don't want to process failure events twice.
//		case EHttpDlNonPausableNetworkLoss:
//		case EHttpDlMultipleMOFailed:
			{
			TBuf<KMaxPath> buf;

			aDownload.GetStringAttribute( EDlAttrDestFilename, buf );

			// We just monitor the progress state and start download, when
			// content-type is received. Any errors are ignored in this state.
			if (
				// we have to check error codes in EHttpProgNone state, otherwise
				// we'll end up infinite loop, for instance, in AP selection cancellation
				 ( aEvent.iProgressState == EHttpContentTypeReceived ) ||
				 ( ( aEvent.iProgressState == EHttpProgNone ) && ( errId == ENoError) ) )
				{
				// TODO: Should we check glErrId and do Reset() before starting download?
				// need to start the download if not already started
				LOG( "Starting download" );
				aDownload.Start();
				// Start timing the download
				iDlStartTime.UniversalTime();
				// ignore any errors, so we can start the download
				break;
				}
			if ( ProcessDlErrors(glErrId, errId, iDownloadError) )
				{
				if (++iConnectionAttempt < KDownloadConnectionRetries)
					{
					LOG4( "Restarting download due to network failure (%d: %d, %d)", iConnectionAttempt, glErrId, errId );
					// TODO: Do we need to do a Reset() before Start()?
					aDownload.Start();
					// We don't restart the download timer on purpose
					//
					// Clear the error id so it doesn't get caught below
					// as this is not an error situation.
					iDownloadError = EDlErrNoError;
					}
				else
					{
					LOG4( "Too many retries, cancelling download (%d; %d, %d)", iDownloadError, glErrId, errId );
					// iDownloadError has been set properly by ProcessDlErrors()
					}
				}
			if ( iDownloadError != KErrNone )
				{
				if (iFetchType)
					LOG2( "DlErr=%d", iDownloadError );
				// Inform about it
				DoHandleHttpFetchFailure(buf, glErrId, errId);
				// TODO: Just in case: don't delete the download in HandleDMgrEventL
				// That seems to cause problems on different devices.
				// Download failed, delete it
				// aDownload.Delete();
				}
			break;
			}
		case EHttpDlCompleted:
			{
			TBuf<KMaxPath> buf;
			// Download Manager total Length of the download
			TInt32 dlLength( 0 );
			aDownload.GetIntAttribute( EDlAttrLength, dlLength );
			aDownload.GetStringAttribute( EDlAttrDestFilename, buf );
			const TInt64 dlTime = UpdateDownloadSpeed(dlLength);
			iDlClient->HandleHttpFetchCompleted(buf, iFetchType, dlLength, iDlAvgSpeed, dlTime);
			break;
			}
		case EHttpDlDeleting:
			// Signal that we're deleting the download
			iDlClient->HandleDownloadDeleting();
			break;
		default:
			{
			break;
			}
		} // switch
	}

// -----------------------------------------------------------------------------
// Updates the average download speed counter
// @param aBytesDownloaded Bytes downloaded during this download
// @returns Time in microseconds spend in this download
// -----------------------------------------------------------------------------
//
TInt64 CDownloadHandler::UpdateDownloadSpeed(const TInt32 aBytesDownloaded)
	{
	TInt64 fromDlStart = 0;
	if (aBytesDownloaded > 0)
		{
		TTime time;
		time.UniversalTime();

		fromDlStart = time.MicroSecondsFrom(iDlStartTime).Int64();

		// Use ifs to avoid divide-by-zero
		if (fromDlStart > 0)
			{
			iDlAvgSpeed = aBytesDownloaded*1000000.0 / fromDlStart;
			}
		}
	return fromDlStart;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CDownloadHandler::CancelAllDownloads()
	{
	// Remove all downloads
	iDownloadManager.DeleteAll();
	}

// -----------------------------------------------------------------------------
// Handles client callback when download fails
// -----------------------------------------------------------------------------
//
void CDownloadHandler::DoHandleHttpFetchFailure(const TDesC& aDlFilename, const TInt aGlobalErrorId, const TInt aErrorId)
	{
	if (!iCancelled)
		{
		iDlClient->HandleHttpFetchFailure(aDlFilename, aGlobalErrorId, aErrorId, iFetchType);
		iCancelled = ETrue;
		}
	else
		{
		LOG( "DoHandleHttpFetchFailure(): Called, but already cancelled" );
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CDownloadHandler::StartDownloadL(const TDesC8& aDepUrl, const TDesC& aFileName,
								const TDesC& aDownloadPath, const TInt aFetchType)
	{
	__ASSERT_ALWAYS( (aDepUrl.Length() > 0) && (aFileName.Length() > 0) && (aDownloadPath.Length() > 0), User::Leave( KErrArgument) );

	// Cleanup is required in case the API is called
	// multiple times.
	DELETE_IF_NONNULL( iUrl );
	DELETE_IF_NONNULL( iResponseBodyFileName );

	// Set the full Url after appending the filename. +1 for '/'
	iUrl = HBufC8::NewL( aDepUrl.Length() + aFileName.Length() + 1 );
	TPtr8 ptr(iUrl->Des());
	ptr.Copy(aDepUrl);
	// Append forward slash if that's missing
	if (ptr.Right(1)[0] != '/')
		ptr.Append('/');

	ptr.Append(aFileName);

	iDownloadError = EDlErrNoError;
	iCancelled = EFalse;
	iConnectionAttempt = 0;

	// Create a new download
	TBool retried = EFalse;
	TBool isNewDl = ETrue;
	iPtrToUrl.Set(*iUrl);

	// Set the full response body file name
	// based on the actual filename to be downloaded.
	iResponseBodyFileName = HBufC::NewL(aDownloadPath.Length()+ aFileName.Length());
	iPtrToResponseBodyFileName.Set(iResponseBodyFileName->Des());
	iPtrToResponseBodyFileName.Copy(aDownloadPath);
	iPtrToResponseBodyFileName.Append(aFileName);

	ConeUtils::EnsurePathExistsL(iPtrToResponseBodyFileName);

	TBuf<KMaxFileName> buf;
	buf.Copy(ptr);
retry:
	if (aFetchType)
		{
		LOG2( "+ StartDownload(): '%S'", &buf );
		LOG2( "  to '%S'", &iPtrToResponseBodyFileName );
		}

	RHttpDownload& dl = iDownloadManager.CreateDownloadL( iPtrToUrl, isNewDl );

	if (isNewDl)
		{
		// Start download
		dl.SetStringAttribute( EDlAttrDestFilename, iPtrToResponseBodyFileName );
		User::LeaveIfError( dl.Start() );
		// This timer will be reset, when content-type is received, but if it's not
		// we need to have a sane starting time, otherwise dl avg speed calculation
		// does not work properly.
		iDlStartTime.UniversalTime();
		iFetchType = aFetchType;
		iDlAvgSpeed = 0.0;
		}
	else
		{
		LOG( "Download exists, deleting all" );
		// Remove any downloads
		iDownloadManager.DeleteAll();
		if (!retried)
			{
			LOG( "Download exists, retrying.." );
			retried = ETrue;
			// we leave isNewDl set to ETrue, so we would accept any existing download for the same URL
			goto retry;
			}
		LOG( "Download exists, retry failed. Panic!" );
		User::Leave( KErrAlreadyExists );
		}
	}
// EOF
