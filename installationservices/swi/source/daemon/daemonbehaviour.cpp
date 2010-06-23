/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "daemonbehaviour.h"
#include "swispubsubdefs.h"
#include "recsisx.h"
#include "sisregistrywritablesession.h"
#include "sisregistryentry.h"
#include "filesisdataprovider.h"
#include "swi/sisparser.h"
#include "swi/siscontents.h"
#include "swi/siscontroller.h"
#include "sisinfo.h"
#include "cleanuputils.h"
#include "sisregistrypackage.h"
#include "log.h"

void ScanDirectoryL(TInt aDrive);

using namespace Swi;

CDaemonBehaviour* CDaemonBehaviour::NewL()
	{
	CDaemonBehaviour* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
CDaemonBehaviour* CDaemonBehaviour::NewLC()
	{
	CDaemonBehaviour* self = new (ELeave) CDaemonBehaviour;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
		
void CDaemonBehaviour::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	User::LeaveIfError(iApaSession.Connect());
	iSisInstaller = CSisInstaller::NewL(this);
	TRAP_IGNORE(iSwiDaemonPlugin = CSwiDaemonPlugin::NewL());
	}
	
CDaemonBehaviour::~CDaemonBehaviour()
	{
	if(iSwiDaemonPlugin)
		{
		delete iSwiDaemonPlugin;
		REComSession::FinalClose();
		}
	delete iSisInstaller;
	iFs.Close();	
	iApaSession.Close();
	}
		
	// from MDaemonBehaviour
TBool CDaemonBehaviour::StartupL()
	{
	// Return state of Startup
	return ETrue;
	}

void CDaemonBehaviour::MediaChangeL(TInt aDrive, TChangeType aChangeType)
	{
	DEBUG_PRINTF3(_L8("SWI Daemon - Media change detected. Drive: %d, Change Type: %d."),
		aDrive, aChangeType);
	
	RSisRegistryWritableSession registrySession;
	
	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);
		
	if (aChangeType==EMediaInserted)
		{
		// notify IAR
		registrySession.AddDriveL(aDrive);

		// Scan directory on the card and run pre-installed through SWIS
		// nb. will leave if no SIS files to install.
		ProcessPreinstalledFilesL(aDrive);
		}
	else if (aChangeType==EMediaRemoved)
		{
		// Cancel all requests for install
		iSisInstaller->Reset();

		// Notify plugin
		if(iSwiDaemonPlugin)
			{
			iSwiDaemonPlugin->MediaRemoved(aDrive);
			}

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		// notify IAR
		registrySession.RemoveDriveL(aDrive);
#endif
		}
	CleanupStack::PopAndDestroy(&registrySession);
	}
	
void CDaemonBehaviour::ProcessPreinstalledFilesL(TInt aDrive)
	{
	_LIT(KDaemonPrivatePath,":\\private\\10202dce\\");
		
	iSisInstaller->Reset();
	ProcessPreinstalledFilesL(aDrive, KDaemonPrivatePath);
	iStartNotified = EFalse;
	iDrive = aDrive;
	iSisInstaller->StartInstallingL();
	}
		
void CDaemonBehaviour::ProcessPreinstalledFilesL(TInt aDrive, const TDesC& aDirectory)
	{
	TPath preInstalledPath;
	TChar drive;
	RFs::DriveToChar(aDrive, drive);
	preInstalledPath.Append(drive);
	preInstalledPath.Append(aDirectory);

	CDir* dir = NULL;
	TInt err = iFs.GetDir(preInstalledPath, KEntryAttNormal, ESortByName, dir);	
	if (err != KErrNone && err != KErrPathNotFound)
			{
			User::Leave(err);
			}
	if(dir)
		{
		// dir will only exist if GetDir succeeded
		CleanupStack::PushL(dir);
		for(TInt i = 0; i < dir->Count(); i++)
			{
			const TEntry &entry = (*dir)[i];
			if(!entry.IsDir())
				{
				TFileName fileName(preInstalledPath);
				fileName.Append(entry.iName);
				iSisInstaller->AddFileToInstallL(fileName);
				}
			}
		CleanupStack::PopAndDestroy(dir);
		}
	}


// From MDaemonInstallBehaviour

MDaemonInstallBehaviour::TSisInstallState CDaemonBehaviour::VerifyAppCacheListL()
	{
	MDaemonInstallBehaviour::TSisInstallState returnState = MDaemonInstallBehaviour::EStateVerifyAppCacheList;
		
	// What is really desired is notification that the Application Architecture cached List
	// is valid, but as this cannot be done simply.
	// Is is simulated by a call to two member functions.
				
	TApaAppInfo appInfo;
	User::LeaveIfError(iApaSession.GetAllApps());
	TInt err(iApaSession.GetNextApp(appInfo)); // Call member function that can report an EAppListInvalid
		
	if (err == RApaLsSession::EAppListInvalid)
		{
		returnState = MDaemonInstallBehaviour::EStateVerifyAppCacheList;
		}
	else if (err == KErrNone) // cached list of apps is ready
		{
		returnState = MDaemonInstallBehaviour::EStateVerifySwisProperty;
		}	
	else
		{
		User::Leave(err);
		}
	return returnState;
	}

MDaemonInstallBehaviour::TSisInstallState CDaemonBehaviour::VerifySwisPropertyL()
	{
	MDaemonInstallBehaviour::TSisInstallState returnState = MDaemonInstallBehaviour::EStateVerifySwisProperty;
	TInt swisState;
	TInt err(RProperty::Get(KUidSystemCategory,KUidSoftwareInstallKey,swisState));
		
	if (err == KErrNotFound)
		{
		returnState = (MDaemonInstallBehaviour::EStateInstall);
		}
	else if (err == KErrNone)
		{
		if (swisState == ESwisNone)
			{
			// SWIS is idle
			returnState = (MDaemonInstallBehaviour::EStateInstall);
			}
		else
			{
			// SWIS is performing an operation
			returnState = (MDaemonInstallBehaviour::EStateVerifySwisIdle);
			}
		}
	else
		{
		User::Leave(err);
		}
	return returnState;
	}

MDaemonInstallBehaviour::TSisInstallState CDaemonBehaviour::VerifySwisIdleL()
	{
	MDaemonInstallBehaviour::TSisInstallState returnState = MDaemonInstallBehaviour::EStateVerifySwisIdle;
	TInt swisState;
			
	// Obtain the state of the software installer
	User::LeaveIfError(RProperty::Get(KUidSystemCategory,KUidSoftwareInstallKey,swisState));

	if (swisState == ESwisNone) // Swis is idle
		{
		returnState = (MDaemonInstallBehaviour::EStateInstall);
		}
	else // Software installer is currently installing/uinstalling
		{
		returnState = (MDaemonInstallBehaviour::EStateVerifySwisIdle);
		}
	return returnState;
	}
	
void CDaemonBehaviour::ReadSymbianHeaderL(RFile& aFile, TUid& aUid1, TUid& aUid2, TUid& aUid3)
	{
	
	TInt uidLen =  sizeof(TInt32);
		
	TPckg<TInt32> uid1(aUid1.iUid);
	User::LeaveIfError(aFile.Read(uid1, uidLen));
	if (uid1.Length() != uidLen)
		{
		User::Leave(KErrUnderflow);
		}
			
	TPckg<TInt32> uid2(aUid2.iUid);
	User::LeaveIfError(aFile.Read(uid2, uidLen));
	if (uid1.Length() != uidLen)
		{
		User::Leave(KErrUnderflow);
		}
			
	TPckg<TInt32> uid3(aUid3.iUid);
	User::LeaveIfError(aFile.Read(uid3, uidLen));
	if (uid1.Length() != uidLen)
		{
		User::Leave(KErrUnderflow);
		}
	
	}

void CDaemonBehaviour::DoInstallRequestL(const TDesC& aFileName)
	{
	DEBUG_PRINTF2(_L("SWI Daemon - Processing presinstalled SIS file '%S'"),
		&aFileName);
		
	TUid appUid;
	TDataType dataType;
		
	User::LeaveIfError(iFs.ShareProtected());
		
	// have to pass the file using the file handle
	// otherwise SisHelper won't be able to read the 
	// daemon private directory
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFileName, EFileRead | EFileStream | EFileShareReadersOnly));
	CleanupClosePushL(file);
	User::LeaveIfError(iApaSession.AppForDocument(file,appUid,dataType));
	
	// Start installing the file only if it is a sisx file
	if (dataType.Des8() == KDataTypeSisx)
		{

		// Check if this is already installed, by extracting the package UID and
		// asking the registry.
			
		TUid uid1, uid2, uid3;
		ReadSymbianHeaderL(file, uid1, uid2, uid3);
			
			
		// UID 3 will be the package UID, query the registry about it.
		RSisRegistrySession session;
		User::LeaveIfError(session.Connect());
		CleanupClosePushL(session);
			
		// temporary work around while we decide what IsInstalled should be policed by.
			
		RSisRegistryEntry entry;
		TInt entryOpenResult = entry.Open(session, uid3);
		CleanupClosePushL(entry);

		// Get the types and other details from the stub
		CFileSisDataProvider* fileProvider = CFileSisDataProvider::NewLC(iFs, aFileName, EFileRead | EFileShareReadersOnly); 
		TInt64 pos( 0 );                            
   		fileProvider->Seek( ESeekStart, pos ); 
		Swi::Sis::CContents* content = Swi::Sis::Parser::ContentsL(*fileProvider);
		CleanupStack::PushL( content );
		HBufC8* controllerdata = content->ReadControllerL(); 
		CleanupStack::PushL( controllerdata ); 
		TPtrProvider provider(controllerdata->Des());
		Sis::CController *siscontroller = Swi::Sis::CController::NewLC(provider);
		
		switch(siscontroller->Info().InstallType())
			{
				/* For SA/PA, If UID is found in registry, Don't install otherwise Install */
				case EInstInstallation:
				case EInstPreInstalledApp:
					{
					if(entryOpenResult != KErrNone)
						{
						DoInstallL(file,appUid);
						}
					
					break;
					}
				/*
				 For PP/SP, If patch name is found already in registry, Don't install 
		 		 otherwise Install 
		 		 */
				case EInstPreInstalledPatch:
				case EInstAugmentation:
					{
					TBool shouldInstall = ETrue;
					const RPointerArray<CString>& names = siscontroller->Info().Names();
					RPointerArray<CSisRegistryPackage> augmentations;
					
					// If Base package is not available then don't do anything and break the case.
					if (entryOpenResult != KErrNone)
						{
						break;
						}

					//Get the Augmentation and Install if augmentation doesn't match 
					CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(augmentations);
					entry.AugmentationsL(augmentations);	
					for (TInt i = 0; i < augmentations.Count(); ++i)
						{
						for (TInt j =0; j < names.Count(); ++j)
							{
							if ((uid3  == augmentations[i]->Uid()) &&
								(augmentations[i]->Name() == names[j]->Data()))
								{
								shouldInstall=EFalse;
								break;
								}
							}
						}

					if(shouldInstall)	
						{
						DoInstallL(file,appUid);
						}
					
					CleanupStack::PopAndDestroy(&augmentations);
					break;
					}
			
				default: 
					/* 
					If SA/PA/SP/PP is not the case we can't handle because
					PU has never been supported for SwiDaemon according to FS.
					*/
					ASSERT(EFalse);
				
			}

		CleanupStack::PopAndDestroy(6, &session);
		
		} // End of SISX file condition
		
	CleanupStack::PopAndDestroy(&file); // close
	}

void CDaemonBehaviour::DoNotifyMediaProcessingComplete()
	{
	if(iSwiDaemonPlugin && iStartNotified)
		{
		iSwiDaemonPlugin->MediaProcessingComplete();
		}
	}

void CDaemonBehaviour::DoInstallL(RFile &file, const TUid &appUid)
	{
					
	// Rewind file
	TInt pos = 0;
	file.Seek(ESeekStart, pos);
				
	if(iSwiDaemonPlugin)
		{
		DEBUG_PRINTF(_L8("SWI Daemon - Using external plugin to process install"));
					
		if(!iStartNotified)
			{
			iSwiDaemonPlugin->MediaProcessingStart(iDrive);
			iStartNotified = ETrue;
			}
		iSwiDaemonPlugin->RequestInstall(file);
		}
		
	else
		{
		DEBUG_PRINTF(_L8("SWI Daemon - Using internal logic to process install"));
			
		TThreadId threadId;
		User::LeaveIfError(iApaSession.StartDocument(file,appUid,threadId));
					
		RThread thread;
		User::LeaveIfError(thread.Open(threadId));
		CleanupClosePushL(thread);
						
		// Wait for the installer to terminate before launching another install.
						
		TRequestStatus status;
		thread.Logon(status);
		User::WaitForRequest(status);
		CleanupStack::PopAndDestroy(&thread);
		}
	}	
