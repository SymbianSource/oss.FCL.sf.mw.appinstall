/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#include "iaupdateutils.h"

#include "iaupdateversion.h"
#include "iaupdatenode.h"
#include "iaupdatebasenode.h"
#include "iaupdatefwversionfilehandler.h"
#include "iaupdatedebug.h"

#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <swi/sisregistrypackage.h>
#include <sysutil.h>
#include <sysversioninfo.h> 

#include <ncdnode.h>
#include <ncdnodepurchase.h>
#include <ncdnodecontentinfo.h>
#include <ncdpurchaseoption.h>

#include <catalogsutils.h>


//Constants
const TInt KSpaceMarginal( 100 * 1024 );
const TInt KSizeMultiplier( 1 );
const TText KVersionSeparator( '.' );


// -----------------------------------------------------------------------------
// IAUpdateUtils::DesHexToIntL
// 
// -----------------------------------------------------------------------------
//  
EXPORT_C TInt IAUpdateUtils::DesHexToIntL( const TDesC& aDes )
    {
    TLex lex( aDes );
    TInt position = aDes.LocateF( 'x' );
    if ( position != KErrNotFound ) 
        {
        // Hex format is of type '0xABC' or 'xABC'
        // Skip over the x-part.
        lex.Assign( aDes.Mid( position+1 ) );
        }
    TUint value;
    User::LeaveIfError( lex.Val( value, EHex ) );
    return value;
    }    


// -----------------------------------------------------------------------------
// IAUpdateUtils::DesHexToIntL
// 
// -----------------------------------------------------------------------------
//  

EXPORT_C void IAUpdateUtils::DesToVersionL( 
    const TDesC& aVersion, 
    TInt8& aMajor, TInt8& aMinor, TInt16& aBuild )
    {
    // Initialize temporary variables with the original values.
        // These will be replaced with new ones if values are found from the string.
    TInt8 major( aMajor );
    TInt8 minor( aMinor );
    TInt16 build( aBuild );
    
    if ( aVersion != KNullDesC )
        // because version may be omitted (service back nodes), empty strings are skipped  
        {
        TBool majorSet( EFalse );
        TBool minorSet( EFalse );
        TBool buildSet( EFalse );
        
        TLex lex( aVersion );
        lex.Mark();
        for( ;; )
            {
            if( lex.Eos() || lex.Peek() == KVersionSeparator )
                {
                TInt value( 0 );
                TPtrC data( lex.MarkedToken() );
                TLex num;
                num.Assign( data );
                User::LeaveIfError( num.Val( value ) );

                if( !majorSet )
                    {
                    major = value;
                    majorSet = ETrue;
                    }
                else if( !minorSet )
                    {
                    minor = value;
                    minorSet = ETrue;
                    }
                else if( !buildSet )
                    {
                    build = value;
                    buildSet = ETrue;
                    // all values received now, exit loop.
                    break;
                    }
                    
                if( lex.Eos() )
                    {
                    break;
                    }
                else
                    {
                    lex.SkipAndMark( 1 );
                    }
                }
            else 
                {
                lex.Inc();
                }
            }
        }
        
    // Now replace original values with the parsed temporary values.
    aMajor = major;
    aMinor = minor;
    aBuild = build;
    }


// -----------------------------------------------------------------------------
// IAUpdateUtils::AppPackageUidL
// 
// -----------------------------------------------------------------------------
//  
EXPORT_C TUid IAUpdateUtils::AppPackageUidL( const TUid& aUid )
    {
    // This is not actually neede here. But, create it so we can use
    // other version of the AppPackageUidL
    TIAUpdateVersion version;
    TUid appUid = AppPackageUidL( aUid, version );    
    return appUid;
    }


// -----------------------------------------------------------------------------
// IAUpdateUtils::AppPackageUidL
// 
// -----------------------------------------------------------------------------
//  
EXPORT_C TUid IAUpdateUtils::AppPackageUidL( 
    const TUid& aUid, TIAUpdateVersion &aVersion )
    {
    TUid packageUid( TUid::Null() );
    
    Swi::RSisRegistrySession registrySession;
    Swi::RSisRegistryEntry entry;
        
    Swi::CSisRegistryPackage* sisRegistryPackage = NULL;
    TInt err = KErrNone;
    TBool entryFound = EFalse;
    
    User::LeaveIfError( registrySession.Connect() );
    CleanupClosePushL( registrySession );
    
    TRAP( err, sisRegistryPackage = registrySession.SidToPackageL( aUid ) );
        
    if ( err == KErrNone )
        {
        CleanupStack::PushL( sisRegistryPackage );
            
        if ( entry.OpenL( registrySession, *sisRegistryPackage ) == KErrNone )
            {
            entryFound = ETrue;

            // The sid was found from this package
            packageUid = sisRegistryPackage->Uid();
            }
        }
    else if ( entry.Open( registrySession, aUid ) == KErrNone )
        {
        entryFound = ETrue;
        
        // The given uid was already the package UID
        packageUid = aUid;
        }
        
    
    if ( entryFound )
        {
        CleanupClosePushL( entry );        
        
        if ( entry.IsPresentL() )
            {
            aVersion = entry.VersionL();
            
            RPointerArray< Swi::CSisRegistryPackage > augs;
            CleanupResetAndDestroyPushL( augs );

            entry.AugmentationsL( augs );
            
            for ( TInt i = 0; i < augs.Count(); ++i )
                {
                Swi::RSisRegistryEntry augEntry;
                TIAUpdateVersion augVersion;
                
                if ( augEntry.OpenL( registrySession, *augs[i] ) == KErrNone )
                    {
                    CleanupClosePushL( augEntry );
                    
                    if ( augEntry.IsPresentL() )
                        {
                        augVersion = augEntry.VersionL();
                        
                        if( augVersion > aVersion )
                            {
                            aVersion = augEntry.VersionL();
                            }
                        }
                    
                    CleanupStack::PopAndDestroy( &augEntry );
                    }
                }
            
            CleanupStack::PopAndDestroy( &augs );
            }
        else
            {
            entryFound = EFalse;

            packageUid = TUid::Null();            
            }
    	      	
        CleanupStack::PopAndDestroy( &entry );
        }
        
    if ( sisRegistryPackage )
        {
        CleanupStack::PopAndDestroy( sisRegistryPackage );
        }
        
    CleanupStack::PopAndDestroy( &registrySession );

    return packageUid;
    }


// -----------------------------------------------------------------------------
// IAUpdateUtils::IsAppInstalledL
// 
// -----------------------------------------------------------------------------
//  
EXPORT_C TBool IAUpdateUtils::IsAppInstalledL( const TUid& aUid )
    {
    // This is not actually neede here. But, create it so we can use
    // other version of the IsAppInstalledL
    TIAUpdateVersion version;
    TBool isInstalled( IsAppInstalledL( aUid, version ) );
    return isInstalled;
    }
    
    
// -----------------------------------------------------------------------------
// IAUpdateUtils::IsAppInstalledL
// 
// -----------------------------------------------------------------------------
//  
EXPORT_C TBool IAUpdateUtils::IsAppInstalledL( 
    const TUid& aUid, TIAUpdateVersion &aVersion )
    {
    if ( AppPackageUidL( aUid, aVersion ) != TUid::Null() )
        {
        return ETrue;
        }
    else 
        {
        return EFalse;
        }
    }


// -----------------------------------------------------------------------------
// IAUpdateUtils::SpaceAvailableInInternalDrivesL
// 
// -----------------------------------------------------------------------------
//  
EXPORT_C TBool IAUpdateUtils::SpaceAvailableInInternalDrivesL( 
    RPointerArray<MIAUpdateNode>& aNodes )
    {
    TBool enoughSpaceFound = ETrue; 
    // packages are cached in C drive. Needed cache size is a size of the biggest package
    TInt sizeOfBiggest = 0;
    TInt i = 0;
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );  
    TInt64 freeOnC = FreeDiskSpace( fs, EDriveC );
    TInt64 freeOnE = FreeDiskSpace( fs, EDriveE );

    if ( freeOnC >= freeOnE )
        {
        for ( i = 0; i < aNodes.Count(); ++i )
            {
            MIAUpdateNode* node( aNodes[ i ] );
            MIAUpdateBaseNode& baseNode( node->Base() );
            if ( baseNode.ContentSizeL() > sizeOfBiggest )
                {
                sizeOfBiggest = baseNode.ContentSizeL();
                }
            }
        }
  
	TDriveUnit driveUnit( EDriveC );
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, sizeOfBiggest, driveUnit ) ) 
	    { // no space even for package caching
		enoughSpaceFound = EFalse;
	    }
	else
	    {
        TInt sizeNeededInDrive = sizeOfBiggest + KSpaceMarginal; //size of the biggest package is included in C drive
        for ( i = 0; i < aNodes.Count() && enoughSpaceFound; ++i )
            {
            TDriveUnit installedDrive;
            MIAUpdateNode* node( aNodes[ i ] );
            MIAUpdateBaseNode& baseNode( node->Base() );
            if ( !IAUpdateUtils::InstalledDriveL( baseNode.Uid(), installedDrive ) )
                { //let's assume that already installed package does not need extra space
        	    sizeNeededInDrive += ( baseNode.ContentSizeL() * KSizeMultiplier );       
                if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, sizeNeededInDrive, driveUnit ) )
                    { // not enough space in a drive, try the next internal drive 
                    enoughSpaceFound = EFalse;
                    //
                    // other drives than C are not scanned...installation is always to C.                     //
                    //
                    //TBool nextInternalDrive = ETrue;
                    //while ( nextInternalDrive )
                    //    {
            	    //    if ( NextInternalDriveL( fs, driveUnit, driveUnit ) )
            	    //        {
            	    //        sizeNeededInDrive = KSpaceMarginal + 
            	    //               ( baseNode.ContentSizeL() * KSizeMultiplier );
            		//        if ( !SysUtil::DiskSpaceBelowCriticalLevelL( &fs, 
            		//                                                     sizeNeededInDrive, 
            		//                                                     driveUnit ) )
            		//            { 
            		//	        nextInternalDrive = EFalse;
            		//            }
             	    //        }
             	    //    else
             	    //        { //all internal drives scanned, just give up now
             	    //        nextInternalDrive = EFalse;
             	    //        enoughSpaceFound = EFalse;	
             	    //        }    
                    //    }
                    }
        	    }
            }
        }
    CleanupStack::PopAndDestroy( &fs );
    return enoughSpaceFound;
    }


// -----------------------------------------------------------------------------
// IAUpdateUtils::IsInstalledL
// 
// -----------------------------------------------------------------------------
// 
EXPORT_C TBool IAUpdateUtils::IsInstalledL( 
    const TUid& aPUid, const TDesC& aExecutable )
    {
    Swi::RSisRegistrySession registrySession;
    Swi::RSisRegistryEntry entry;
    User::LeaveIfError( registrySession.Connect() );
    CleanupClosePushL( registrySession );
    TInt exeFound = EFalse;
    if ( entry.Open( registrySession, aPUid ) == KErrNone )
        {
        CleanupClosePushL( entry );
    	RPointerArray<HBufC> files; 
        entry.FilesL( files );
    	for( TInt i = 0; !exeFound && i < files.Count(); i++)
    	    {
    	    TFileName fullName = *files[i];	
            TParse parse;
            parse.Set( fullName, NULL, NULL);    
            if ( parse.NameAndExt().CompareF( aExecutable ) == 0 )
    		    {
    			exeFound = ETrue;
    		    }
    	    }
    	files.ResetAndDestroy();    
        CleanupStack::PopAndDestroy( &entry );
        }
    CleanupStack::PopAndDestroy( &registrySession );
    return exeFound;
    }


// ---------------------------------------------------------------------------
// IAUpdateUtils::SilentInstallOptionsL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C SwiUI::TInstallOptions IAUpdateUtils::SilentInstallOptionsL( 
    const TUid& aUid, TInt aSize )
    {
    SwiUI::TInstallOptions options;

    // Upgrades are allowed        
    options.iUpgrade = SwiUI::EPolicyAllowed;

    // Install all if optional packets exist.
    options.iOptionalItems = SwiUI::EPolicyAllowed;

    // Prevent online cert revocation check.
    options.iOCSP = SwiUI::EPolicyNotAllowed;
    
    // See iOCSP setting above
    options.iIgnoreOCSPWarnings = SwiUI::EPolicyAllowed;

    // Do not allow installation of uncertified packages.
    options.iUntrusted = SwiUI::EPolicyNotAllowed;

    // If filetexts are included in SIS package. Then, show them.
    options.iPackageInfo = SwiUI::EPolicyUserConfirm;
    
    // Automatically grant user capabilities.
    // See also iUntrusted above.
    options.iCapabilities = SwiUI::EPolicyAllowed;

    // Open application will be closed.
    options.iKillApp = SwiUI::EPolicyAllowed;
    
    // Files can be overwritten.
    options.iOverwrite = SwiUI::EPolicyAllowed;
    
    // This only affects Java applications.
    options.iDownload = SwiUI::EPolicyAllowed;
    
    // Where to save.
    TDriveUnit driveUnit = IAUpdateUtils::DriveToInstallL( aUid, aSize );
    TDriveName driveName = driveUnit.Name();
    options.iDrive = driveName[0];
    
    // Choose the phone language.
    options.iLang = User::Language();
    
    // If language is asked, then use the current phone language.
    options.iUsePhoneLang = ETrue;
    
    // Does not affect SISX. This is for Java.
    options.iUpgradeData = SwiUI::EPolicyAllowed;

    return options;
    }


// -----------------------------------------------------------------------------
// IAUpdateUtils::InstalledDriveL
// 
// -----------------------------------------------------------------------------
// 
TBool IAUpdateUtils::InstalledDriveL( 
    const TUid& aUid, TDriveUnit& aLocationDrive )
    {
    TBool installed = EFalse;
	Swi::RSisRegistrySession registrySession;
	User::LeaveIfError( registrySession.Connect() );
    CleanupClosePushL( registrySession );
    Swi::RSisRegistryEntry entry;
    TInt ret = entry.Open( registrySession, aUid );
    if ( ( ret != KErrNone ) && ( ret != KErrNotFound ) )
        {
    	User::LeaveIfError( ret );
        }
    if ( ret == KErrNone )
        {
    	CleanupClosePushL( entry );  
        if ( ( !entry.IsInRomL() ) && ( entry.IsPresentL() ) )
            { //only interested in a drive available for installation just now
            installed = ETrue;
    	    TUint drivesMask = entry.InstalledDrivesL();
            if( drivesMask )
                {
                // Select the highest drive as location drive. That's the case when 
                // all installation is not in same drive
                TInt drive = EDriveA;
                while( drivesMask >>= 1 )
                    {
                    drive++;
                    }
                aLocationDrive = drive;
                }
            else
                {
                // No installed files, select C: as location drive
                aLocationDrive = EDriveC;
                }
            }
        CleanupStack::PopAndDestroy( &entry );
        }
    else
        {
        entry.Close();	
        }
    
    CleanupStack::PopAndDestroy( &registrySession ); 
	return installed;
    }


// -----------------------------------------------------------------------------
// IAUpdateUtils::NextInternalDriveL
// 
// -----------------------------------------------------------------------------
// 

TBool IAUpdateUtils::NextInternalDriveL( 
    RFs& aFs, 
    TDriveUnit aCurrentDrive, 
    TDriveUnit& aNextDrive )
   {
   TBool nextInternalDrive = EFalse;
   
   TInt driveCount = 0; 
   TDriveList driveList;
   User::LeaveIfError( DriveInfo::GetUserVisibleDrives( aFs,
                                                        driveList, 
                                                        driveCount) ); 

   TUint driveStatus = 0;
   for ( TInt j = aCurrentDrive + 1; j < KMaxDrives && !nextInternalDrive; j++ )
       {
       if ( driveList[j] )
           {
    	   User::LeaveIfError( DriveInfo::GetDriveStatus( aFs, 
    	                                                  j, 
    	                                                  driveStatus ) );
    	   if ( driveStatus & DriveInfo::EDriveInternal )
    	       {
    	       nextInternalDrive = ETrue;
    	       aNextDrive = j;
    	       }
	       }
       }
        	   
   return nextInternalDrive;
   }


// -----------------------------------------------------------------------------
// IAUpdateUtils::DriveToInstallL
// 
// -----------------------------------------------------------------------------
//   
TDriveUnit IAUpdateUtils::DriveToInstallL( const TUid& /*aUid*/, TInt /*aSize*/ )
    {
    
	/*TDriveUnit preferredDriveUnit;
	TDriveUnit targetDriveUnit( EDriveC );
	//preferred drive is same as a drive of previous installation
	if ( !InstalledDriveL( aUid, preferredDriveUnit ) )
	    {
		preferredDriveUnit = BiggestInternalDriveL();
	    }
	    
	if ( !InternalDriveWithSpaceL( aSize * KSizeMultiplier + KSpaceMarginal, 
	                               preferredDriveUnit, 
	                               targetDriveUnit ) )
	    { //try again without space marginal
		if ( !InternalDriveWithSpaceL( aSize * KSizeMultiplier, preferredDriveUnit, targetDriveUnit ) )
		    { //no space with estimated size, let's try to preferred drive
			targetDriveUnit = preferredDriveUnit;
		    }
	    }*/
	    
	// installation drive is alvays "C"
    TDriveUnit targetDriveUnit( EDriveC );
    return targetDriveUnit;
    }


// -----------------------------------------------------------------------------
// IAUpdateUtils::BiggestInternalDriveL
// 
// -----------------------------------------------------------------------------
//
TDriveUnit IAUpdateUtils::BiggestInternalDriveL()
    {
    RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs ); 
	TInt driveCount = 0; 
	TDriveList driveList;
	User::LeaveIfError( DriveInfo::GetUserVisibleDrives( 
                                         fs, 
                                         driveList, 
                                         driveCount) ); 
    TUint driveStatus = 0;
    TDriveUnit driveUnit( EDriveC );
    TInt size( 0 );
	for ( TInt index(0); index < KMaxDrives; index++ )
    	{
    	if ( driveList[index] )
    	    {
    	  	User::LeaveIfError( DriveInfo::GetDriveStatus( fs, 
    	                                                   index, 
    	                                                   driveStatus ) );
    	    if ( driveStatus & DriveInfo::EDriveInternal )
    	        {
    	        TVolumeInfo volumeInfo;
    	        User::LeaveIfError( fs.Volume( volumeInfo, index ) );
    	        if ( volumeInfo.iSize > size )
    	            {
    	        	driveUnit = index;
    	        	size = volumeInfo.iSize;
    	            }
     	        }
    	    }	
	    }
	CleanupStack::PopAndDestroy( &fs );
	return driveUnit;    
    }


// -----------------------------------------------------------------------------
// IAUpdateUtils::InternalDriveWithSpaceL
// 
// -----------------------------------------------------------------------------
//   
TBool IAUpdateUtils::InternalDriveWithSpaceL( 
    TInt aSize, 
    TDriveUnit aPreferredDriveUnit, 
    TDriveUnit& aTargetDriveUnit )
    {
	TBool enoughSpaceFound = EFalse;
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs ); 
	//check preferred drive first, that's default drive
	aTargetDriveUnit = aPreferredDriveUnit;
	if ( !SysUtil::DiskSpaceBelowCriticalLevelL( &fs, aSize, aPreferredDriveUnit ) )
        {
		enoughSpaceFound = ETrue;
	    }
	else
	    { //go through other internal drives
	    TInt driveCount = 0; 
	    TDriveList driveList;
	    User::LeaveIfError( DriveInfo::GetUserVisibleDrives( 
                                         fs, 
                                         driveList, 
                                         driveCount) ); 
        TUint driveStatus = 0;
	    for ( TInt index(0); index < KMaxDrives && !enoughSpaceFound; index++ )
    	    {
    	    if ( driveList[index]  && aPreferredDriveUnit != index )
    	        {
    	    	User::LeaveIfError( DriveInfo::GetDriveStatus( fs, 
    	                                                       index, 
    	                                                       driveStatus ) );
    	        if ( driveStatus & DriveInfo::EDriveInternal )
    	            {
    	        	if ( !SysUtil::DiskSpaceBelowCriticalLevelL( &fs, 
    	        	                                             aSize, 
    	        	                                             index ) )
    	        	    {
    	        		aTargetDriveUnit = index;
    	        		enoughSpaceFound = ETrue;
    	        	    }
    	            }
    	        }
    	    }	
	    }
	
	CleanupStack::PopAndDestroy( &fs );
    return enoughSpaceFound;
    }

// -----------------------------------------------------------------------------
// IAUpdateUtils::SaveCurrentFwVersionIfNeededL
// 
// -----------------------------------------------------------------------------
// 
void IAUpdateUtils::SaveCurrentFwVersionIfNeededL()
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateUtils::SaveCurrentFwVersionIfNeededL() begin");
    TBuf<KSysVersionInfoTextLength> info;
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    TInt ret = SysVersionInfo::GetVersionInfo(SysVersionInfo::EFWVersion,
                info, fs);
    CleanupStack::PopAndDestroy( &fs ); 
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateUtils::SaveCurrentFwVersionIfNeededL() ret: %d", ret );
    if ( ret != KErrNotSupported )
        {
        User::LeaveIfError( ret );
        CIAUpdateFwVersionFileHandler* fwVersionFileHandler = CIAUpdateFwVersionFileHandler::NewLC();
        if ( *fwVersionFileHandler->FwVersionL() != info )
            {
            IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateUtils::SaveCurrentFwVersionIfNeededL() Current firmware version in a phone: %S", &info );
            fwVersionFileHandler->SetFwVersionL( info );
            IAUPDATE_TRACE("[IAUPDATE] IAUpdateUtils::SaveCurrentFwVersionIfNeededL() firmware version stored");
            }
        CleanupStack::PopAndDestroy( fwVersionFileHandler );
        }
    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateUtils::SaveCurrentFwVersionIfNeededL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateUtils::IsFirmwareChangedL()
// 
// -----------------------------------------------------------------------------
// 
TBool IAUpdateUtils::IsFirmwareChangedL() 
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateUtils::IsFirmwareChangedL() begin");
    TBool fwChanged = EFalse;
    TBuf<KSysVersionInfoTextLength> info;
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    TInt ret = SysVersionInfo::GetVersionInfo( SysVersionInfo::EFWVersion,
                    info, fs );
    CleanupStack::PopAndDestroy( &fs ); 
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateUtils::IsFirmwareChangedL() ret: %d", ret );
    if ( ret != KErrNotSupported )
        {
        User::LeaveIfError( ret );
        IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateUtils::IsFirmwareChangedL() Current firmware version in a phone: %S", &info );
    
        CIAUpdateFwVersionFileHandler* fwVersionFileHandler = CIAUpdateFwVersionFileHandler::NewLC();
        if ( *fwVersionFileHandler->FwVersionL() != info && *fwVersionFileHandler->FwVersionL() != KNullDesC )
            {
            fwChanged = ETrue;
            }
        CleanupStack::PopAndDestroy( fwVersionFileHandler );
        }
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateUtils::IsFirmwareChangedL() fwChanged: %d", fwChanged );
    return( fwChanged );
    }
    
 
// ---------------------------------------------------------------------------
// IAUpdateUtils::FreeDiskSpace
// ---------------------------------------------------------------------------
// 
TInt64 IAUpdateUtils::FreeDiskSpace( RFs& aFs, TInt aDriveNumber )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateUtils::FreeDiskSpace() begin");
    TInt64 freeSpace = 0;
    TVolumeInfo volumeInfo;
    
    if ( aFs.Volume( volumeInfo, aDriveNumber ) == KErrNone)
        {
        freeSpace = volumeInfo.iFree;
        }
    
    IAUPDATE_TRACE_2("[IAUPDATE]IAUpdateUtils::FreeDiskSpace() drive: %d free space: %d kB", 
                    aDriveNumber, freeSpace / 1024 );
    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateUtils::FreeDiskSpace() end");
    return freeSpace;
    }  
    


