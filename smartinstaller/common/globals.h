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
*     Declaration of the global constants.
*
*
*/

#ifndef GLOBALS_H_
#define GLOBALS_H_

// Constants

// Bootstrap UID
const TUid KUidBootstrap = TUid::Uid(0x2002CCCD);

// Minimum supported version of bootstrap.sis
const TVersion KMinBootstrapSisVersion(0, 9, 00);

// Define the bootstrap version to be used.
// Currently the versions supported are 1, 2, 3 and 4.
const TInt KBootstrapVersion = 4;

// Bootstrap-ADM command line protocol version
const TUint32 KBootstrapAdmProtocolVersion = 15;

// Command line arguments present
const TUint32 KCmdLineProtocolVerBits = 4;
const TUint32 KCmdLineBitProtocolVerMask  = (2 << KCmdLineProtocolVerBits  ) - 1; // KBootstrapAdmProtocolVersion must fit into this
const TUint32 KCmdLineBitBootstrapVersion = 2 << (KCmdLineProtocolVerBits  );
const TUint32 KCmdLineBitWrapperUid       = 2 << (KCmdLineProtocolVerBits+1);
const TUint32 KCmdLineBitIAP              = 2 << (KCmdLineProtocolVerBits+2);
const TUint32 KCmdLineBitADMLaunchControl = 2 << (KCmdLineProtocolVerBits+3);
const TUint32 KCmdLineBitFileDep          = 2 << (KCmdLineProtocolVerBits+4);
const TUint32 KCmdLineBitFileSis          = 2 << (KCmdLineProtocolVerBits+5);
const TInt KCmdLineParamLengths[] = {
		11, 11, 11, 11, 11, 0, 0 ,0
};

/**
 * The maximum amount of real time to be used when trying to get
 * access to installer is KIterationTimer * KAttemptsToAccessInstaller (in us)
 *
 * Default time is 100ms.
 */
const TInt KIterationTimer = 100000; // in microseconds
const TInt KAttemptsToAccessInstaller = 20;

// Number of download retry attempts from the CDownloadHandler::HandleDMgrEventL()
const TInt KDownloadConnectionRetries = 3;

// Number of download restart attempts from the state machine
const TInt KDownloadRestartRetries = 3;

// Maximum length of popup dialog message
const TInt KMaxMsgSize = 128;

// Amount of data to be downloaded before user is prompted when using packet data connection
// 0 = Prompt always for packet data connections, but not for WLAN (except when Ovi Store is running)
const TInt KDownloadPromptThreshold = 0; //5 * 1024 * 1024;

// Cmd line values to indicate the starting behaviour of ADM
const TInt KCmdLineCancelAdm 		= 1;
const TInt KCmdLineCancelAdmNoResume 	= 2;
const TInt KCmdLineLaunchAdm 		= 3;

// Value for invalid IAP ID
const TUint KInvalidIapId = KMaxTUint;

// TODO: make the URL runtime configurable
_LIT8(KDefaultDepServerUrl, "");

enum VersionResult
	{
	EEqualVersion,
	EGreaterFirstVersion,
	EGreaterSecondVersion
	};

enum EDownloadError
	{
	EDlErrNoError = 0,
	EDlErrCancelled,
	EDlErrNetworkFailure,
	EDlErrNotEnoughSpace,
	EDlErrFileNotFound,
	EDlErrDownloadFailure
	};

#endif /* GLOBALS_H_ */
