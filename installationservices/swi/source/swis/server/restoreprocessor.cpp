/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "restoreprocessor.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sts/sts.h>
#include "swtypereginfo.h"
#include "installswtypehelper.h"
#include "cleanuputils.h"
#else
#include "integrityservices.h"
#endif

#include "application.h"
#include "hashcontainer.h"
#include "secutils.h"
#include "sisregistryentry.h"
#include "sisregistrypackage.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "sistruststatus.h"
#include "log.h"
#include "securitycheckutil.h"
#include "sidcache.h"
#include <f32file.h>
#include "userselections.h"
#include "sissupportedlanguages.h"

using namespace Swi; 


//
// CRestoreProcessor
//

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
_LIT(KApparcRegDir, "\\private\\10003a3f\\import\\apps\\");
#endif

CRestoreProcessor::CRestoreProcessor(const CPlan& aPlan, const TDesC8& aControllerBuffer,
	CSecurityManager& aSecurityManager,	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession, RArray<TAppUpdateInfo>& aAppInfo,
#else
	CIntegrityServices& aIntegrityServices,
#endif
	const RPointerArray<CRestoreController::CSisCertificateVerifier>& aVerifiers, RSwiObserverSession& aObserver)
	: CActive(CActive::EPriorityStandard),
	  iVerifiers(aVerifiers),
	  iSecurityManager(aSecurityManager),
	  iControllerBuffer(aControllerBuffer),	  
	  iPlan(aPlan),	  
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	  iStsSession(aStsSession),
	  iRegistrySession(aRegistrySession),
	  iAppInfo(aAppInfo),
#else
	  iIntegrityServices(aIntegrityServices),
#endif
	  iSystemDriveChar(RFs::GetSystemDriveChar()),
	  iObserver(aObserver)
	{
	CActiveScheduler::Add(this);	
	}
		
CRestoreProcessor* CRestoreProcessor::NewL(const CPlan& aPlan, const TDesC8& aControllerBuffer, CSecurityManager& aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession, RArray<TAppUpdateInfo>& aAppInfo,
#else
	CIntegrityServices& aIntegrityServices,
#endif
	const RPointerArray<CRestoreController::CSisCertificateVerifier>& aVerifiers,
	RArray<TUid>& aSids, RSwiObserverSession& aObserver)
	{
	CRestoreProcessor* self = CRestoreProcessor::NewLC(aPlan, aControllerBuffer, aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			aStsSession, aRegistrySession,aAppInfo,
#else
			aIntegrityServices, 
#endif
			aVerifiers, aSids, aObserver);
	CleanupStack::Pop(self);
	return self;
	}
		
CRestoreProcessor* CRestoreProcessor::NewLC(const CPlan& aPlan, const TDesC8& aControllerBuffer, CSecurityManager& aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession, RArray<TAppUpdateInfo>& aAppInfo,
#else
	CIntegrityServices& aIntegrityServices,
#endif
	const RPointerArray<CRestoreController::CSisCertificateVerifier>& aVerifiers,
	RArray<TUid>& aSids, RSwiObserverSession& aObserver)
	{
	CRestoreProcessor* self = new (ELeave) CRestoreProcessor(aPlan, aControllerBuffer, aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		aStsSession, aRegistrySession,aAppInfo,
#else
		aIntegrityServices, 
#endif
		aVerifiers, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSids);
	return self;
	}

void CRestoreProcessor::InstallFileL(const CSisRegistryFileDescription& aRegistryFileDescription)
	{	
	TParsePtrC parse(aRegistryFileDescription.Target());
	
	TFileName tempFileName;
	_LIT(KTemporaryFileFormat, "%c:%Stfile-%d-%d-%d-");
	TUint driveCh(iSystemDriveChar);
	tempFileName.Format(KTemporaryFileFormat, driveCh, &KSysInstallTempPath,
		iApplication->ControllerL().Info().Uid().Uid().iUid, iApplication->AbsoluteDataIndex(), aRegistryFileDescription.Index());

	// Append a hex representation of a hash of the filename. We need the
	// temporary filename to be effectively unique, but the filename itself
	// may be too long.
	// Use the entire path minus the drive letter because the drive letter can change.
	// For consistency, the target name must be the name from controller not the target
	// after the path substitution code.
	HBufC* hashBuf = SecUtils::HexHashL(parse.FullName().Right(parse.FullName().Length() - 1));
	tempFileName.Append(*hashBuf);
	delete hashBuf;
	
	DEBUG_PRINTF3(_L("Restore - Installing File '%S' from '%S'"), &aRegistryFileDescription.Target(), &tempFileName);

  	TEntry entry;
  	TInt err = iFs.Entry(tempFileName, entry);
  	if (err == KErrNone)
		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		TRAP(err, iStsSession.RemoveL(aRegistryFileDescription.Target()));
#else
  		TRAP(err, iIntegrityServices.RemoveL(aRegistryFileDescription.Target()));
#endif
  		if (err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
  			{
			User::Leave(err);
			}
		
		TInt err = iFs.MkDirAll(aRegistryFileDescription.Target());
  		if (err != KErrNone && err != KErrAlreadyExists)
  			{
  			User::LeaveIfError(err);
  			}
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
  		iStsSession.RegisterNewL(aRegistryFileDescription.Target());
#else
  		iIntegrityServices.AddL(aRegistryFileDescription.Target());
#endif
  		User::LeaveIfError(iFileMan->Move(tempFileName, aRegistryFileDescription.Target()));
		}
else
 		{
 		DEBUG_PRINTF3(_L("Cannot find '%S' assuming SBE will restore public data err = %d"), &tempFileName, err);
		} 

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Parse the file if it carries software type registration info
	if (InstallSoftwareTypeHelper::IsValidSwRegFileL(aRegistryFileDescription.Target(),
													 iApplication->ControllerL().Info().Uid().Uid().iUid))
		{
		InstallSoftwareTypeHelper::ParseRegFileL(iFs,
												 aRegistryFileDescription.Target(),
												 iSoftwareTypeRegInfoArray);
		}
#endif

	}

void CRestoreProcessor::ConstructL(RArray<TUid>& aSids)
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	iFileMan = CFileMan::NewL(iFs);
	for (TInt i = 0; i < aSids.Count(); ++i)
		{
		iSids.AppendL(aSids[i]);
		}
	}

void CRestoreProcessor::ExtractHashL(const CSisRegistryFileDescription& aFileToProcess)
	{			
	TBuf<32> hashPath;	
	TUint driveCh(iSystemDriveChar); // can't pass TChar to Format
	hashPath.Format(KHashPathFormat, driveCh, &KHashPath);	
	TParse hashFileName;
	hashFileName.Set(hashPath, &aFileToProcess.Target(), NULL);

	TEntry hashEntry;
	TInt err = iFs.Entry(hashFileName.FullName(), hashEntry);

	if (err == KErrNone)
		{
		// Hash file exists, need to remove the old one.
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iStsSession.RemoveL(hashFileName.FullName());
#else
		iIntegrityServices.RemoveL(hashFileName.FullName());
#endif
		}
	
	err = iFs.MkDirAll(hashFileName.DriveAndPath());
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		} 

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iStsSession.RegisterNewL(hashFileName.FullName());
#else
	iIntegrityServices.AddL(hashFileName.FullName());
#endif

	DEBUG_PRINTF2(_L("Restore - Extracting hash to file '%S'"),
		&hashFileName.FullName());

	RFile file;
	User::LeaveIfError(file.Create(iFs, hashFileName.FullName(),
		EFileWrite | EFileShareExclusive | EFileStream));
		
	CleanupClosePushL(file);
	const TDesC8& hashData = aFileToProcess.Hash().Data();
	User::LeaveIfError(file.Write(hashData));
	CleanupStack::PopAndDestroy(&file);
	}
			
TBool CRestoreProcessor::DoStateInitializeL()
	{
	iCurrent = 0;
	return ETrue;
	}


TBool CRestoreProcessor::DoStateInstallFilesL()
	{
	if (iCurrent < iApplication->FilesToAdd().Count())
		{
		if(0 == iCurrent)
			{//Write package header before logging the first install event
			CObservationHeader *header = CObservationHeader::NewLC(iApplication->ControllerL().Info().Uid().Uid(), 
						static_cast<TPackageType>(iApplication->ControllerL().Info().InstallType()), Swi::EOpInstall);
			iObserver.AddHeaderL(*header);
			CleanupStack::PopAndDestroy(header);
			}
			
		//Get file description	
		CSisRegistryFileDescription* regFileDes = iApplication->FilesToAdd()[iCurrent++];

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 		
		// Add apparc registerd files in list.
		AddApparcFilesInListL(regFileDes->Target(), *iApplication);
#endif
		
		//Complete actual file installation
		InstallFileL(*regFileDes);
		TUint8 fileFlag(EFileAdded);
		if(regFileDes->Sid().iUid)
			{//if the file has a SID, it can be exe or dll
			if(SecUtils::IsExeL(regFileDes->Target()))
				{//Set file exe flag.
				fileFlag |= Swi::EFileExe;
				}
			else if(SecUtils::IsDllL(regFileDes->Target()))
				{//Set file dll flag.
				fileFlag |= Swi::EFileDll;
				}
			}
				
		//Write the name of the file into the observation file
		CObservationData *event = CObservationData::NewLC(regFileDes->Target(),regFileDes->Sid(),fileFlag);	
		iObserver.AddEventL(*event);
		CleanupStack::PopAndDestroy(event);
		SwitchState(ECurrentState);
		return EFalse;
		}
	else
		{
		iCurrent = 0;
		return ETrue;
		}
	}
	
TBool CRestoreProcessor::DoStateUpdateRegistryL()
	{
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
    // Taking the previous controller's affected apps
    RArray<TAppUpdateInfo>& affectedApps = iAppInfo;        
    const CApplication& application = *iApplication;   // current application
	//if there are reg files in the package or if its an upgrade (in case of SA (with app) over SA(with no app))
    if(iApparcRegFilesForParsing.Count() != 0 || application.IsUpgrade())
        {
        //Create the list of Application Uids which are affected by the Restore                   
        Usif::TComponentId componentId = 0;
        RArray<Usif::TComponentId> componentIds;
        CleanupClosePushL(componentIds);
        RArray<TUid> existingAppUids;
        CleanupClosePushL(existingAppUids);
        TAppUpdateInfo existingAppInfo;     
        TUid packageUid = application.ControllerL().Info().Uid().Uid();
        
        if(application.IsUpgrade())
            {           
            //Get all componentIds
            iRegistrySession.GetComponentIdsForUidL(packageUid, componentIds);            
            TInt count = componentIds.Count();
            if(0 == count)
                {
                DEBUG_PRINTF(_L("ComponentIDs not found for the base package"));
                User::Leave(KErrNotFound);
                }
                                
            //SA over SA
            if(application.ControllerL().Info().InstallType() == Sis::EInstInstallation )
                {        
                //Get the compid for base package
                componentId = iRegistrySession.GetComponentIdForUidL(packageUid);              
                      
                TInt index = componentIds.Find(componentId);
             
                //Exclude the Base SA compId from the list 
                componentIds.Remove(index);
             
                //Get the apps for Base SA compId and mark them as to be deleted
                existingAppUids.Reset();
                TRAPD(err,iRegistrySession.GetAppUidsForComponentL(componentId, existingAppUids);)  
                //If Base Package does not contain any app then GetAppUidsForComponentL will return KErrNotFound, ignore the error else leave
                if (KErrNone != err && KErrNotFound != err)
                    {
                    User::Leave(err);
                    }
                
                for(TInt i = 0 ; i < existingAppUids.Count(); ++i)
                    {
                    existingAppInfo = TAppUpdateInfo(existingAppUids[i], EAppUninstalled);    
                    affectedApps.AppendL(existingAppInfo);
                    }                
                //Get the apps for Remaining CompIds(SP's) and mark them as to be upgraded               
                for(TInt i = 0 ; i < componentIds.Count(); ++i)
                    {
                    existingAppUids.Reset();             
                    //If there are no apps within the components (SP's) then it will return KErrNotFound
                    TRAP(err, iRegistrySession.GetAppUidsForComponentL(componentIds[i], existingAppUids);) 
                    if (KErrNone != err && KErrNotFound != err)
                        {
                        User::Leave(err);
                        }
                    
                    for(TInt k = 0 ; k < existingAppUids.Count(); ++k)
                        {
                        existingAppInfo = TAppUpdateInfo(existingAppUids[i], EAppInstalled);    
                        affectedApps.AppendL(existingAppInfo);
                        }
                    }                                
                }
                
            //SP over SP
            if(application.ControllerL().Info().InstallType() == Sis::EInstAugmentation)
                {
                componentId = iRegistrySession.GetComponentIdForPackageL(application.PackageL().Name(), application.PackageL().Vendor());                
                //Get the apps for Base SP compId and mark them as to be deleted
                existingAppUids.Reset();
                TRAPD(err, iRegistrySession.GetAppUidsForComponentL(componentId, existingAppUids);) 
                if (KErrNone != err && KErrNotFound != err)
                    {
                    User::Leave(err);
                    }
                
                for(TInt k = 0 ; k < existingAppUids.Count(); ++k)
                   {
                   // Search for the app in the existing set of affected apps, if already present mark them as UnInstalled else add a new entry
                   TInt index = FindAppEntry(affectedApps, existingAppUids[k]);
                   if (KErrNotFound != index)
                       {
                       affectedApps[index].iAction = EAppUninstalled;
                       }
                   else
                       {
                       existingAppInfo = TAppUpdateInfo(existingAppUids[k], EAppUninstalled);    
                       affectedApps.AppendL(existingAppInfo);
                       }                   
                   }                                  
                }        
            }                   
        CleanupStack::PopAndDestroy(2, &componentIds);   
        }       
#endif
	
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	RSisRegistryWritableSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
#endif
		
	Swi::RSisRegistryEntry entry;
	TBool entryInROM(EFalse);	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TInt err = entry.Open(iRegistrySession, iApplication->ControllerL().Info().Uid().Uid());
#else
	TInt err = entry.Open(session, iApplication->ControllerL().Info().Uid().Uid());
#endif
	if (err == KErrNone)
		{
		CleanupClosePushL(entry);
		entryInROM = entry.IsInRomL();
		CleanupStack::PopAndDestroy(&entry);
		}

	if (iApplication->IsUpgrade() || iApplication->IsPartialUpgrade() || (iApplication->IsInstall() && entryInROM)) 	
 		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		if (iSoftwareTypeRegInfoArray.Count() > 0)
			{
			iRegistrySession.UpdateEntryL(*iApplication, iControllerBuffer, iSoftwareTypeRegInfoArray, iStsSession.TransactionIdL());
			}
		else
			{
			iRegistrySession.UpdateEntryL(*iApplication, iControllerBuffer, iStsSession.TransactionIdL());
			}
		TInt count = iApparcRegFileData.Count();
        for (int i = 0; i < count; i++)
            {
            iRegistrySession.UpdateEntryL(*iApplication, *iApparcRegFileData[i], iApparcRegFilesForParsing[i]->GetSisRegistryPackage());
            }
#else
 		session.UpdateEntryL(*iApplication, iControllerBuffer, iIntegrityServices.TransactionId());
#endif
 		}
 	else
 		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		if (iSoftwareTypeRegInfoArray.Count() > 0)
			{
			iRegistrySession.AddEntryL(*iApplication, iControllerBuffer, iSoftwareTypeRegInfoArray, iStsSession.TransactionIdL());
			}
		else
			{
			iRegistrySession.AddEntryL(*iApplication, iControllerBuffer, iStsSession.TransactionIdL());
			}
		TInt count = iApparcRegFileData.Count();
        for (int i = 0; i < count; i++)
            {
            iRegistrySession.AddEntryL(*iApparcRegFileData[i], iApparcRegFilesForParsing[i]->GetSisRegistryPackage());
            }
#else
 		session.AddEntryL(*iApplication, iControllerBuffer, iIntegrityServices.TransactionId());
#endif
 		}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Registration of MIME types of the software types being installed to AppArc
	InstallSoftwareTypeHelper::RegisterMimeTypesL(iSoftwareTypeRegInfoArray);
	
	//if there are reg files in the package or if its an upgrade (in case of SA (with app) over SA(with no app))
	if(iApparcRegFilesForParsing.Count() != 0 || application.IsUpgrade())
	        {
	        //Create the list of Application Uids which are affected by the Restore                            
	        RArray<Usif::TComponentId> componentIds;
	        CleanupClosePushL(componentIds);
	        RArray<TUid> newAppUids;    
	        CleanupClosePushL(newAppUids);
	        TAppUpdateInfo existingAppInfo, newAppInfo;     
	        TUid packageUid = application.ControllerL().Info().Uid().Uid();
	        //Get all componentIds for the application
	        componentIds.Reset();
	        iRegistrySession.GetComponentIdsForUidL(packageUid, componentIds);
	        TInt count = componentIds.Count();
	        
	        //Get the apps for All CompIds               
	        for(TInt i = 0 ; i < count; i++)
	            {
	            newAppUids.Reset();                    
	            TRAPD(err,iRegistrySession.GetAppUidsForComponentL(componentIds[i], newAppUids))
	            if (KErrNone != err && KErrNotFound != err)
	                {
	                User::Leave(err);
	                }
	            
	            for(TInt i = 0 ; i < newAppUids.Count(); ++i)
	                {	               	                
	                TInt index = 0;		
	                // Search for the app in the existing set of affected apps, if already present mark them as UnInstalled else add a new entry
	                index = FindAppEntry(affectedApps, newAppUids[i]);
	                if(index != KErrNotFound)
	                    {
	                    affectedApps[index].iAction = EAppInstalled;
	                    }
	                else
	                    {
	                    existingAppInfo = TAppUpdateInfo(newAppUids[i], EAppInstalled);
	                    affectedApps.AppendL(existingAppInfo);
	                    }
	                }  		           
	            }
	        
	        //Compare the new affected apps with the existing affected apps and update the existing affected apps if alredy present or 
            //add to the list if it is a new app.
            RArray<TAppUpdateInfo> existingAffectedApps;
            CleanupClosePushL(existingAffectedApps);
            const_cast<CPlan&>(iPlan).GetAffectedApps(existingAffectedApps);
            TInt appCount = affectedApps.Count();
            for(TInt k = 0; k < appCount ; ++k)
                {
                TInt count = existingAffectedApps.Count();
                TUid appUid = affectedApps[k].iAppUid;
                for(TInt index = 0; index < count ; ++index)
                   {
                   if(appUid == existingAffectedApps[index].iAppUid)
                       {           
                       existingAffectedApps.Remove(index);                                    
                       }
                   }
                existingAffectedApps.AppendL(affectedApps[k]);
                }
	        const_cast<CPlan&>(iPlan).ResetAffectedApps();
	        const_cast<CPlan&>(iPlan).SetAffectedApps(affectedApps);
	        
	        CleanupStack::PopAndDestroy(3, &componentIds);
	        }
	
#else
	CleanupStack::PopAndDestroy(&session);
#endif
	return ETrue;
	}
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
TInt CRestoreProcessor::FindAppEntry(RArray<TAppUpdateInfo>& aAffectedApps, TUid& aNewAppUid)
    {
    TInt count = aAffectedApps.Count();
    for(TInt index = 0; index < count ; index++)
        {
        if(aAffectedApps[index].iAppUid == aNewAppUid)
            {           
            return index;
            }
        }
    return KErrNotFound;
    }
#endif


TBool CRestoreProcessor::DoStateProcessFilesL()
	{
	if (iCurrent < iApplication->FilesToAdd().Count())
		{
		
		// determine the temporary file name from the registry description
		// and controller
		CSisRegistryFileDescription* aFileToProcess = iApplication->FilesToAdd()[iCurrent++];
		TParsePtrC parse(aFileToProcess->Target());
		
		_LIT(KTemporaryFileFormat, "%c:%Stfile-%d-%d-%d-");
		TFileName tempFileName;		
		TUint driveCh(iSystemDriveChar); // Can't pass TChar to Format.
		tempFileName.Format(KTemporaryFileFormat, driveCh, &KSysInstallTempPath, 
			iApplication->ControllerL().Info().Uid().Uid().iUid, iApplication->AbsoluteDataIndex(), aFileToProcess->Index());
		
		// Append a hex representation of a hash of the filename. We need the
		// temporary filename to be effectively unique, but the filename itself
		// may be too long.
		// Use the entire path minus the drive letter because the drive letter can change.
		// For consistency, the target name must be the name from controller not the target
		// after the path substitution code.		
		HBufC* hashBuf = SecUtils::HexHashL(parse.FullName().Right(parse.FullName().Length() - 1));
		tempFileName.Append(*hashBuf);
		delete hashBuf;

		//if the file is an exe or dll, which in restore machine
		//is more or less guarantee'd, then we need to add the hash
		// to /sys/hash
		
  		TEntry entry;
  		TInt err = iFs.Entry(tempFileName, entry);
  		if (err == KErrNone)
  			{
  			// Process the file if the temporary actually exists
  			TInt err = SecurityCheckUtil::ProcessFileL(*iApplication, iFs, iSids,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
  				iStsSession, 
#else
  				iIntegrityServices, 
#endif
				*aFileToProcess, tempFileName);
  			if (err != KErrNone)
  				{
  				User::Leave(KErrCorrupt);
  				}
  				
  			// extract a hash file for this, if the target is /sys/bin	
  			if (parse.Path().CompareF(KBinPath) == 0)
  				{
  				ExtractHashL(*aFileToProcess);
  				}
  			}  			
		SwitchState(ECurrentState);
		return EFalse;		
		}
	else 
		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK                
        // Find out all the regisration resource files associated with this package UID and add to the list of 
        // files to be processed later for parsing      
        TRAPD(err, AddAppArcRegResourceFilesL());
        if ( err != KErrNotFound && err != KErrNone)
            {
            User::Leave(err);
            }
#endif
		iCurrent = 0;
		return ETrue;		
		}		
	}
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CRestoreProcessor::AddAppArcRegResourceFilesL()
    {
    RSisRegistryEntry entry;
    TInt err = KErrNone;
    if (!iApplication->IsUninstall())
        err = entry.Open(iRegistrySession, iApplication->ControllerL().Info().Uid().Uid());
    else
        return;
    
    User::LeaveIfError(err);
    CleanupClosePushL(entry);
    AddAppArcRegResourceFilesForRegEntryL(entry);
    
    RPointerArray<CSisRegistryPackage> augmentationArray;
    CleanupResetAndDestroyPushL(augmentationArray);
    entry.AugmentationsL(augmentationArray);
    CleanupStack::Pop(&augmentationArray);
    CleanupStack::PopAndDestroy(&entry);    
        
    CleanupResetAndDestroyPushL(augmentationArray);
    TInt count = augmentationArray.Count();
    for ( TInt i=0; i < count; ++i)
        {
        TInt err = entry.OpenL(iRegistrySession,*augmentationArray[i]);
        User::LeaveIfError(err);
        CleanupClosePushL(entry);
        AddAppArcRegResourceFilesForRegEntryL(entry);
        CleanupStack::PopAndDestroy(&entry);
        }
    
    CleanupStack::PopAndDestroy(&augmentationArray);
    }

void CRestoreProcessor::AddAppArcRegResourceFilesForRegEntryL(RSisRegistryEntry& aEntry)
    {
    RPointerArray<HBufC> filesArray;
    CleanupResetAndDestroyPushL(filesArray);
    aEntry.FilesL(filesArray);    
    TInt count = filesArray.Count();
    CSisRegistryPackage *regPkg = aEntry.PackageL();
    CleanupStack::PushL(regPkg);
    
    for (TInt i=0; i<count; ++i)
        {
        if (FileIsApparcReg(*filesArray[i]))
            {
            CAppRegFileData *tmpAppRegFileData = CAppRegFileData::NewLC(*filesArray[i],*regPkg);
            iApparcRegFilesForParsing.AppendL(tmpAppRegFileData);
            CleanupStack::Pop(tmpAppRegFileData);
            }
        }
    CleanupStack::PopAndDestroy(2,&filesArray);
    }
#endif


TBool CRestoreProcessor::DoStateVerifyPathsL() 
	{
	if (iCurrent < iApplication->FilesToAdd().Count())
		{		
		CSisRegistryFileDescription* fileToProcess = iApplication->FilesToAdd()[iCurrent++];
		
		SecurityCheckUtil::TProtectedDirectoryCheckError dummyErrorCode;
		// sis file signed by Su Cert are allowed to restore files in private dir 
		// without corresponding executable in the package.
		TBool pathValid = SecurityCheckUtil::CheckProtectedDirectoriesL(
				fileToProcess->Target(),
				fileToProcess->Operation(),
				iSids,
				dummyErrorCode);

		if (!pathValid && !iApplication->IsInstallSuCertBased())
			{
			User::Leave(KErrAccessDenied);
			}	
			
		SwitchState(ECurrentState);
		return EFalse;		
		}
	else
		{
		iCurrent = 0;
		return ETrue;
		}
	}
	
void CRestoreProcessor::RunL()
	{
	DEBUG_PRINTF3(_L8("Restore - Restore processing state machine, State: %d, Status: %d"),
		iState, iStatus.Int());
	
	// Leave if there has been an error
	User::LeaveIfError(iStatus.Int());

	switch(iState)
		{	
	case EInitialize:
		if (DoStateInitializeL())
			{
			SwitchState(EProcessFiles);
			}
		break;

	case EProcessFiles:
		if (DoStateProcessFilesL())
			{
			SwitchState(EVerifyPaths);
			}
		break;
		
	case EVerifyPaths:
		if (DoStateVerifyPathsL())
			{
			SwitchState(EInstallFiles);
			}
		break;

	case EInstallFiles:
		if (DoStateInstallFilesL())
			{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK           
            SwitchState(EParseApplicationRegistrationFiles);
#else
            SwitchState(EUpdateRegistry);
#endif          
            }
        break;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    case EParseApplicationRegistrationFiles:
        if (DoParseApplicationRegistrationFilesL())
            {
			SwitchState(EUpdateRegistry);
			}
		break;
#endif
	case EUpdateRegistry:
		if (DoStateUpdateRegistryL())
			{
			SwitchState(EFinished);
			}
		break;
	
	case EFinished:
		DoStateFinishedL();
		break;
		
	default:
		User::Leave(KErrGeneral);
		break;
		}
	}

void CRestoreProcessor::SwitchState(TProcessingState aNextState)
	{
	if (aNextState!=ECurrentState)
		{
		iState=aNextState;
		}
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

	
void CRestoreProcessor::DoStateFinishedL()
	{
	User::RequestComplete(iClientStatus, KErrNone);
	}

TInt CRestoreProcessor::RunError(TInt aError)
	{
	
	User::RequestComplete(iClientStatus, aError);
	return KErrNone;
	
	}
	
void CRestoreProcessor::ProcessApplicationL(const CApplication& aApplication, TRequestStatus& aClientStatus)
	{
	iApplication = &aApplication;
	iClientStatus = &aClientStatus;
	aClientStatus = KRequestPending;
	
	iState = EInitialize;
	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CRestoreProcessor::ProcessPlanL(TRequestStatus& aClientStatus)
	{
	ProcessApplicationL(iPlan.ApplicationL(), aClientStatus);
	}
	
void CRestoreProcessor::DoCancel()
	{
	}

CRestoreProcessor::~CRestoreProcessor()
	{
	delete iFileMan;
	iFs.Close();
	iSids.Close();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    iSoftwareTypeRegInfoArray.Close();  
    iApparcRegFilesForParsing.ResetAndDestroy();
    iApparcRegFileData.ResetAndDestroy();
    delete iAppRegExtractor;   
    iAppInfo.Close();
#endif
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

TBool CRestoreProcessor::ParseRegistrationResourceFileL(const TDesC& aTargetFileName)
    {
    DEBUG_PRINTF2(_L("Restore Processor - ParseRegistrationResourceFileL - Parsing '%S' registration resource file"), &aTargetFileName);
 
    if ( NULL == iAppRegExtractor )
        {
        DEBUG_PRINTF(_L("Restore Processor - ParseRegistrationResourceFileL - Creating CAppRegExtractor for async parsing of registration resource file"));             
        iAppRegExtractor = CAppRegExtractor::NewL(iFs, iApparcRegFileData);                   
        }
    iAppRegExtractor->ExtractAppRegInfoSizeL(aTargetFileName, iStatus);
    return EFalse;
    }

TBool CRestoreProcessor::DoParseApplicationRegistrationFilesL()
    {
    if (iCurrent == 0)
        DEBUG_PRINTF2(_L("Restore Processor - DoParseApplicationRegistrationFilesL - Number of Application registration resource files to be parsed %d"), iApparcRegFilesForParsing.Count());
    
    if (iAppRegExtractor != NULL)
        {
        if (iAppRegExtractor->GetErrorCode() == KErrCorrupt)
            {
            delete iApparcRegFilesForParsing[--iCurrent];
            iApparcRegFilesForParsing.Remove(iCurrent);
            }
        }
    
    if (iCurrent < iApparcRegFilesForParsing.Count())
        { 
        TDesC& fileDescription = (iApparcRegFilesForParsing[iCurrent++]->GetAppRegFile());   
        // Continue processing the next file if a registration resource file is not found(in case of SA over SA)
        TRAPD(err,ParseRegistrationResourceFileL(fileDescription));        
        if(KErrNotFound == err || KErrPathNotFound == err)
            {               
            delete iApparcRegFilesForParsing[--iCurrent];
            iApparcRegFilesForParsing.Remove(iCurrent); 
            TRequestStatus* status = &iStatus;
            User::RequestComplete(status, KErrNone);     
            SetActive();
            }
        else if(KErrNone != err )
            {
            User::Leave(err);
            }
        else
            {
            SetActive();
            }
        return EFalse;
        }
    else
        {
        iCurrent = 0;
        return ETrue;
        }
    }

TBool CRestoreProcessor::FileIsApparcReg(const TDesC& aFilename) const
    {
    TParsePtrC filename(aFilename);
    return filename.Path().CompareF(KApparcRegDir) == 0;
    }
	
void CRestoreProcessor::AddApparcFilesInListL(const TDesC& aTargetFileName, const CApplication& aApplication)
    {    
    if (FileIsApparcReg(aTargetFileName))
        {
        // we're installing a reg file so add it to our list for parsing it and 
        // populating SCR in EParseApplicationRegistrationFiles state of CProcessor     
        TInt index = UserSelectedLanguageIndexL(aApplication);
        CSisRegistryPackage *regPkg = CSisRegistryPackage::NewLC(aApplication.ControllerL().Info().Uid().Uid(),\
                aApplication.ControllerL().Info().Names()[index]->Data(),\
                aApplication.ControllerL().Info().UniqueVendorName().Data());
        CAppRegFileData *appRegData =CAppRegFileData::NewLC(aTargetFileName,*regPkg);
        iApparcRegFilesForParsing.AppendL(appRegData);
        CleanupStack::Pop(appRegData);  
        CleanupStack::PopAndDestroy(regPkg);
        }
    }

TInt CRestoreProcessor::UserSelectedLanguageIndexL(const CApplication& aApplication) const
// used to find out which is the index of the selected language, 
// based on the language selection. This will be used for the relevant package and vendor names
    {
    TLanguage language = aApplication.UserSelections().Language();
    
    TInt index = KErrNotFound;
    for (TInt i = 0; i < aApplication.ControllerL().SupportedLanguages().Count(); i++)
        {
        if (aApplication.ControllerL().SupportedLanguages()[i] == language)
            {
            index = i;
            break;  
            }
        }
    User::LeaveIfError(index);
    return index;
    }   
#endif

