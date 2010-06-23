/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
*/

#include <e32base.h>
#include "dummy-swi-plugin.h"
#include <ecom/implementationproxy.h>
#include <apgcli.h>
_LIT(KNull,"");

//#define PRINTF_ON

#ifdef PRINTF_ON
_LIT(KMspSwiPlugin,"msp-swi-plugin");
#define PRINTF1(fmt) iConsole->Printf(fmt)
#define PRINTF2(fmt,arg) iConsole->Printf(fmt, arg)
#else
#define PRINTF1(fmt) do{}while(0)
#define PRINTF2(fmt,arg) do{(void)arg;}while(0)
#endif

static const TImplementationProxy ImplementationTable[] =
{
	{{0x10274D09}, TProxyNewLPtr(CSwiDaemonPluginImp::NewL)}
};

EXPORT_C const TImplementationProxy *ImplementationGroupProxy(TInt &aTableCount)
	{
	aTableCount = sizeof(ImplementationTable)/ sizeof(ImplementationTable[0]);
	return ImplementationTable;
	}

CSwiDaemonPlugin *CSwiDaemonPluginImp::NewL()
	{
	CSwiDaemonPluginImp *self = new(ELeave)CSwiDaemonPluginImp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


void CSwiDaemonPluginImp::ConstructL()
	{
#ifdef PRINTF_ON
	iConsole = Console::NewL(KMspSwiPlugin, TSize(KConsFullScreen,KConsFullScreen));
#endif
	User::LeaveIfError(iFs.Connect());
	}

CSwiDaemonPluginImp::~CSwiDaemonPluginImp()
	{
	iFs.Close();
	while(iRFiles.Count())
		{
		iRFiles[0].Close();
		iRFiles.Remove(0);
		}
	delete iConsole;
	iConsole = 0;
	}

void CSwiDaemonPluginImp::MediaProcessingStart( TInt aDrive )
	{
	PRINTF2(_L("CSwiDaemonPluginImp::MediaProcessingStart(%d)\n"), aDrive);
	tagStage(_L("MediaProcessingStart"), KNull);

	// Cleanup old RFile array
	while(iRFiles.Count())
		{
		iRFiles[0].Close();
		iRFiles.Remove(0);
		}
	}


void CSwiDaemonPluginImp::RequestInstall( RFile& aFile )
	{
	RFile file;
	TInt err = file.Duplicate(aFile);
	if(err != KErrNone)
		{
		PRINTF2(_L("CSwiDaemonPluginImp::RequestInstall - Failed to duplicate handle with error %d\n"), err);
		return;
		}
	
	TFileName fileName;
	User::LeaveIfError(file.FullName(fileName));
	PRINTF1(_L("CSwiDaemonPluginImp::RequestInstall(RFile '"));
	PRINTF1(fileName);
	PRINTF1(_L("')\n"));

	tagStage(_L("RequestInstall"), fileName);

	err = iRFiles.Append(file);
	if(err != KErrNone)
		{
		PRINTF2(_L("CSwiDaemonPluginImp::RequestInstall - Append returned %d\n"), err);
		return;
		}
	}

void CSwiDaemonPluginImp::MediaRemoved( TInt aDrive )
	{
	PRINTF2(_L("CSwiDaemonPluginImp::MediaRemoved(%d)\n"), aDrive);
	tagStage(_L("MediaRemoved"), KNull);
	}

void CSwiDaemonPluginImp::MediaProcessingComplete()
	{
	PRINTF1(_L("CSwiDaemonPluginImp::MediaProcessingComplete()\n"));
	tagStage(_L("MediaProcessingComplete"), KNull);
	while(iRFiles.Count())
		{
		TFileName fileName;
		if(iRFiles[0].FullName(fileName) == KErrNone)
			{
			PRINTF1(_L("Installing '"));
			PRINTF1(fileName);
			PRINTF1(_L("'...\n"));
			
#if 0
			TRAPD(err, MediaProcessingCompleteInternalL(iRFiles[0]));
			if(err != KErrNone)
				{
				PRINTF2(_L("MediaProcessingCompleteInternalL failed %d"), err);
				}
			PRINTF1(_L("...Installed\n"));
#else
			PRINTF1(_L("...Skipped (install disabled in dummy-swi-plugin.cpp)\n"));
#endif
			iRFiles[0].Close();
			iRFiles.Remove(0);
			}
		}
	}
	
void CSwiDaemonPluginImp::MediaProcessingCompleteInternalL(RFile &aFile)
	{
	RApaLsSession apaSession;
	User::LeaveIfError(apaSession.Connect());
	TUid appUid;
	TDataType dataType;
	User::LeaveIfError(apaSession.AppForDocument(aFile,appUid,dataType));
	TThreadId threadId;

	User::LeaveIfError(apaSession.StartDocument(aFile, appUid, threadId));
	
	RThread thread;
	User::LeaveIfError(thread.Open(threadId));
	CleanupClosePushL(thread);
					
	// Wait for the installer to terminate before launching another install.
	
	TRequestStatus status;
	thread.Logon(status);
	User::WaitForRequest(status);
	CleanupStack::PopAndDestroy(&thread);
	}


void CSwiDaemonPluginImp::tagStage(const TDesC &stage, const TDesC &fileName)
	{
	TRAPD(err,tagStageL(stage, fileName));
	if(err != KErrNone)
		{
		// Test will fail because we failed to create correct tag files.
		User::Panic(_L("CSwiDaemonPluginImp::tagStage"), 1);
		}
	}


_LIT(KTAGDIR, "\\dummy-swi-plugin\\");

			
void CSwiDaemonPluginImp::tagStageL(const TDesC &stage, const TDesC &fileName)
	{
	if(iCallbackNumber == 0)
		{
		// First run so cleanup old progress tags
		CFileMan *fileMan = CFileMan::NewL(iFs);
		CleanupStack::PushL(fileMan);
		TDriveUnit sysDrive (RFs::GetSystemDrive());
		TBuf<128> tagDir (sysDrive.Name());
		tagDir.Append(KTAGDIR);
		TInt err = fileMan->RmDir(tagDir);
		if(err != KErrNone && err != KErrPathNotFound)
			{
			User::Leave(err);
			}
		
		CleanupStack::PopAndDestroy(fileMan);
		}

	TInt err = iFs.MkDirAll(KTAGDIR);
	if(err == KErrNone)
		{
		// Presumably test code wiped out the tag dir
		iCallbackNumber = 0;
		}
	
	++iCallbackNumber;

	TFileName dest;
	// Make tag dir called nn-stage-filename
	if(fileName.Length())
		{
		TParse parser;
		parser.Set(fileName, NULL, NULL);
		TPtrC p = parser.NameAndExt();
		dest.AppendFormat(_L("%S%03d-%S-%S"), &KTAGDIR, iCallbackNumber, &stage, &p);
		}
	else
		{
		dest.AppendFormat(_L("%S%03d-%S"), &KTAGDIR, iCallbackNumber, &stage);
		}

	RFile file;
	User::LeaveIfError(file.Create(iFs, dest, EFileShareExclusive | EFileWrite));
	file.Close();

	// Make tag dir called nn (used to check if we have had more than X callbacks in tests)
	dest.Zero();
	dest.AppendFormat(_L("%S%03d"), &KTAGDIR, iCallbackNumber);
	User::LeaveIfError(file.Create(iFs, dest, EFileShareExclusive | EFileWrite));
	file.Close();
	}


// End of file

 
