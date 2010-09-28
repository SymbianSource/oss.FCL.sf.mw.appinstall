/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Implements  a built-in SIF launcher that integrates the SIF API with AppArc.
* Thanks to this integration,  generic applications may launch installation of
* a common scenario widely used, for example by file and web browsers, when
* the user clicks on a file or link.
*
*/


/**
  This is a software component using @see RApaLsSession::StartDocument.
*/

#include <e32base.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <f32file.h>
#include <usif/sif/sif.h>
#include "usiflog.h"

using namespace Usif;

LOCAL_C void LaunchInstallL();

/** main function called by E32 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();

	__UHEAP_MARK;
	TRAPD(err,LaunchInstallL());
	__UHEAP_MARKEND;

	delete cleanup;
	return err;
	}

LOCAL_C void LaunchInstallL()
    {
	// Get the command line object with data as attached to the process by AppArc.
	CApaCommandLine* commandLine = NULL;
	TInt err = CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine);
	DEBUG_PRINTF2(_L8("SIF Launcher: CApaCommandLine::GetCommandLineFromProcessEnvironment result = %d\n"),err);
	User::LeaveIfError(err);
	
	CleanupStack::PushL(commandLine);
	
	switch (commandLine->Command())
		{
		case EApaCommandOpen:
			{
			// Open the file and use the handle to install it
			RFile fileHandle;
			CleanupClosePushL(fileHandle);
			commandLine->GetFileByHandleL(fileHandle);

			// Establish a connection with the SIF server
			RSoftwareInstall sif;
			err = sif.Connect();
			if (err != KErrNone)
				{
				DEBUG_PRINTF2(_L8("SIF Launcher: RSoftwareInstall::Connect() failed, error = %d\n"),err);
				User::Leave(err);
				}

			CleanupClosePushL(sif);
			TRequestStatus status;
			
			if (fileHandle.SubSessionHandle() != KNullHandle)
				{
				// We've got a working file handle, so use it...
				DEBUG_PRINTF(_L8("SIF Launcher: installation by file handle...\n"));
				sif.Install(fileHandle, status);
				}
			else
				{
				// ...otherwise the file name of a package
				DEBUG_PRINTF(_L8("SIF Launcher: installation by file name...\n"));
				sif.Install(commandLine->DocumentName(), status);
				}
				
			User::WaitForRequest(status);
			DEBUG_PRINTF2(_L8("SIF Launcher: installation finished with err = %d\n"),status.Int());
			CleanupStack::PopAndDestroy(2, &fileHandle);
			break;
			}
			
		/* SIF Launcher doesn't support the following commands:
		- EApaCommandCreate,
		- EApaCommandRun,
		- EApaCommandBackground,
		- EApaCommandViewActivate,
		- EApaCommandRunWithoutViews,
		- EApaCommandBackgroundAndWithoutViews */
		default:
			User::Leave(KErrNotSupported);
			break;
		}

	CleanupStack::PopAndDestroy(commandLine);
	}
