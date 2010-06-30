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
*     Declares DownloadHandler class for application.
*
*
*/


#ifndef __ADMDOWNLOADHANDLER_H__
#define __ADMDOWNLOADHANDLER_H__

#include <e32base.h>
#include <downloadmgrclient.h>
#include "globals.h"
#include "networkstatusobserver.h"

// Forward declarations
class CADMAppUi;
class CNetworkStatusListener;
class RFileLogger;

class CDownloadHandler : public CBase,
	public MHttpDownloadMgrObserver,
	public MNetworkStatusObserver
	{
public:
	class MDownloadClient
		{
	public:
		virtual void HandleIapChanged( const TUint32 aIAP ) = 0;
		virtual void HandleHttpFetchCompleted( const TDesC& aDlFilename, const TInt aDownloadType, const TInt32 aBytesDownloaded, const TReal32 aAvgDlSpeed, const TInt64 aDlTime ) = 0;
		virtual void HandleHttpFetchInProgress( const TInt32 aDownloaded, const TInt32 aTotalDownloadSize, const TReal32 aAvgDlSpeed ) = 0;
		virtual void HandleHttpFetchFailure( const TDesC& aDlFilename, const TInt aGlobalErrorId, const TInt aErrorId, const TInt aFetchType ) = 0;
		virtual void HandleDownloadDeleting() = 0;
		};

#ifdef USE_LOGFILE
public:
	static CDownloadHandler* NewL(MDownloadClient* aDlClient, RFileLogger& aLogger, const TBool aMasterInstance);
	void Log(TRefByValue<const TDesC16> aFmt, ...);

private:
	CDownloadHandler( MDownloadClient* aDlClient, RFileLogger& aLogger, const TBool aMasterInstance );

private:
	RFileLogger&        iLog; ///< Not Owned

#else
public:
	static CDownloadHandler* NewL(MDownloadClient* aDlClient, const TBool aMasterInstance);
	static CDownloadHandler* NewLC(MDownloadClient* aDlClient, const TBool aMasterInstance);

private:
	CDownloadHandler( MDownloadClient* aDlClient, const TBool aMasterInstance );
#endif

public:
	~CDownloadHandler();

public:
	void HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent );
	void StartDownloadL( const TDesC8& aDepUrl, const TDesC& aFileName, const TDesC& aDownloadPath, const TInt aFetchType );
	void CancelAllDownloads();
	void SetIAP( TUint32 aIAP );

	inline EDownloadError GetDownloadError() const
		{
		return iDownloadError;
		}

	//From MNetworkStatusObserver
	void HandleNetworkStatusChangedL( const TInt aStatus );

private:
	void ConstructL();
	TBool ProcessDlErrors(const TInt32 aGlErrId, const TInt32 aErrId, EDownloadError& aDownloadError);
	TInt64 UpdateDownloadSpeed(const TInt32 aBytesDownloaded);
	void DoHandleHttpFetchFailure(const TDesC& aDlFilename, const TInt aGlobalErrorId, const TInt aErrorId);

private:
	MDownloadClient*    iDlClient;
	CNetworkStatusListener* iNetworkStatusListener;
	TInt                iFetchType;
	TBool               iMasterInstance;
	TBool               iNetworkLost; // ETrue, if used network connection is lost during download
	TBool               iCancelled; // ETrue, if called HandleHttpFailure()
	HBufC8*             iUrl;
	TPtrC8              iPtrToUrl; //to avoid a temporary TPtrC8 being passed by IPC
	HBufC*              iResponseBodyFileName;
	TPtr                iPtrToResponseBodyFileName;
	RHttpDownloadMgr    iDownloadManager;
	TInt32              iIAP;
	TInt                iConnectionAttempt;
	EDownloadError      iDownloadError;
	TInt                iDownloadState;
	TInt                iProgressState;
	TTime               iDlStartTime;
	TReal32             iDlAvgSpeed;
	};

#endif  //__ADMDOWNLOADHANDLER_H__
