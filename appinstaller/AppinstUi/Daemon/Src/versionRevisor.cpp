/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include <sysutil.h>
#include <f32file.h>

#include "sisregistrysession.h"
#include "sisregistryentry.h"
#include "sisregistrypackage.h"
#include "filesisdataprovider.h"
#include "siscontroller.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "sisversion.h"

#include "versionRevisor.h"
#include "uninstaller.h"
#include "SWInstDebug.h"

_LIT( KVerRevStubSISDir, "z:\\system\\install\\" );
_LIT( KVerRevVersionFile, "c:\\private\\10202DCE\\firmware_version.txt" );
_LIT( KVerRevPrivatePath, "c:\\private\\10202DCE\\" );
_LIT( KVerRevNewLinePattern, "\\n" );
_LIT( KVerRevNewLine, "\n" );
_LIT( KVerRevSysPath, "\\sys\\bin\\" );
_LIT( KVerRevRomSysPath, "z:\\sys\\bin\\" );
const TInt KVerRevStrLength = KSysUtilVersionTextLength;  
const TUint KVerRevDriveC = 4; //0x04 = C drive.

const TInt KVerRevSisVersionOlder = 200;
const TInt KVerRevSisVersionSame = 300;
const TInt KVerRevSisVersionNewer = 400;

namespace Swi
    {
    // -----------------------------------------------------------------------
    // Two-phased constructor
    // -----------------------------------------------------------------------
    //    
    CVersionRevisor* CVersionRevisor::NewL( CProgramStatus& aMainStatus )
        {
        CVersionRevisor* self = NewLC( aMainStatus );
        CleanupStack::Pop( self );
        return self;
        }
    
    // ------------------------------------------------------------------------
    // Two-phased constructor
    // ------------------------------------------------------------------------
    //    
    CVersionRevisor* CVersionRevisor::NewLC( CProgramStatus& aMainStatus )
        {
        CVersionRevisor* self = new (ELeave) CVersionRevisor;
        CleanupStack::PushL( self );
        self->ConstructL( aMainStatus );
        return self;    
        }
      
    // -----------------------------------------------------------------------
    // 2nd constructor
    // -----------------------------------------------------------------------
    //    
    void CVersionRevisor::ConstructL( CProgramStatus& aMainStatus )
        {
        FLOG( _L("[VersionRev] ConstructL start") );
        iSisUninstaller = NULL;
        iStartupReason = KErrNone; 
        // Note revisor do not own this instance.
        iProgramStatus = &aMainStatus;         
        User::LeaveIfError( iFs.Connect() );
        // Get system startup reason.
        TRAP_IGNORE( StartUpReasonL() );         
        FLOG( _L("[VersionRev] ConstructL end") );
        }
    
    // -----------------------------------------------------------------------
    // c++ destructor
    // -----------------------------------------------------------------------
    //    
    CVersionRevisor::~CVersionRevisor()
        {
        FLOG( _L("[VersionRev] ~CVersionRevisor start") );         
        delete iSisUninstaller;
        iSisUninstaller = NULL;         
        iStubDataArray.ResetAndDestroy();               
        iFs.Close(); 
        FLOG( _L("[VersionRev] ~CVersionRevisor end") ); 
        }
     
    // -----------------------------------------------------------------------
    // Starts uninstall process.
    // -----------------------------------------------------------------------
    //    
    TInt CVersionRevisor::StartProcessL()
        {  
        FLOG( _L("[VersionRev] StartProcessL start") );        
        TInt err = KErrNone;
        
//#ifdef _DEBUG        
//FLOG( _L("[VersionRev] FOR TESTING: STARTUP REASON = Firmwareupdate ! ! !") );          
//iStartupReason = EFirmwareUpdate;
//#endif

        // If firmware update is done, start scan process.
         if( iStartupReason == EFirmwareUpdate )
              {                             
              iSisUninstaller = 
                  CSisPkgUninstaller::NewL( *iProgramStatus, *this );  
              
              TRAP( err, ProcessStubFilesL() );
              FLOG_1( _L("[VersionRev] Process stub TRAP error = %d"), err );
               
              TRAP( err, ProcessSisRegistryL() ); 
              FLOG_1( _L("[VersionRev] Process sis TRAP error = %d"), err );
               
              iSisUninstaller->StartUninstallL();                           
              }        
         else
             {
             FLOG( _L("[VersionRev] StartProcessL: No firmware update") );
             // Ok, no need to start process. Leave so main will call delete. 
             User::Leave( KErrAbort );
             }
                                                     
        FLOG( _L("[VersionRev] StartProcessL end") );
        return KErrNone;
        }
   
    // -----------------------------------------------------------------------
    // Reads stub sis files.
    // -----------------------------------------------------------------------
    //    
    TInt CVersionRevisor::ProcessStubFilesL()
        {  
        FLOG( _L("[VersionRev] ProcessStubFilesL start") );
        CDir* dir;
        
        TInt err = iFs.GetDir( 
                KVerRevStubSISDir, 
                KEntryAttNormal, 
                ESortNone, 
                dir );
        
        if ( err == KErrNone )
            {
            CleanupStack::PushL( dir );
            
            TInt count = dir->Count();
            FLOG_1( _L("[VersionRev] Stub count = %d"), count );

            for ( TInt index = 0; index < count; index++ )
                {
                FLOG_1( _L("[VersionRev] Stub index = %d"), index );                                               
                TFileName stubName;                 
                stubName.Append( KVerRevStubSISDir );
                stubName.Append( (*dir)[index].iName );                
                FLOG_1( _L("[VersionRev] Stub name: %S\n "), 
                        &(*dir)[index].iName );                
                // Function needs to be trapped because SisController leavs 
                // if some corrupted file is in dir etc.
                TRAP( err, AppendStubInfoL( stubName ) );                            
                }                       
            CleanupStack::PopAndDestroy( dir ); 
            }
        
        FLOG( _L("[VersionRev] ProcessStubFilesL end") );         
        return err;
        }
    
    // -----------------------------------------------------------------------
    // Checks rom updates from sis registry and adds pkg UID to array for
    // uninstaller process.
    //
    // Pre contitons for uninstall is: 
    // 1. sis version is older or same as rom STUB version.
    // 2. sis has files only in C drvie.
    // 3. if sis version is same as stub version, all binary files must be
    // present in rom (z:\sys\bin).
    // Rom STUB <- SA       = uninstalled
    // Rom STUB <- SP       = no uninstall, returned drive is Z   (0x04)
    // Rom STUB <- PU       = no uninstall, returned drive is Z+C (0x2000004)
    // Rom STUB <- SA <- SP = SA is uninstalled, SP is not (n-gage) 
    // Rom STUB <- SA <- PU = uninstalled
    // Rom STUB <- SA + emped. sis = SA is uninstalled, empedded pkg is not.
    // -----------------------------------------------------------------------
    //    
    TInt CVersionRevisor::ProcessSisRegistryL()
        {
        FLOG( _L("[VersionRev] ProcessSisRegistryL start") );        
        TInt stubArrayCount = iStubDataArray.Count();
        FLOG_1( _L("[VersionRev] Stub array count = %d"), stubArrayCount );
        
        if ( stubArrayCount )
            {
            Swi::RSisRegistrySession sisRegSession;                                        
            User::LeaveIfError( sisRegSession.Connect() );                
            CleanupClosePushL( sisRegSession );
                               
            Swi::RSisRegistryEntry entry;                                    
            TInt err = KErrNone;
                         
            // Search correct sis pagace and open entry to it.
            for ( TInt stubIndex = 0; stubIndex < stubArrayCount; stubIndex++ )
                {                  
                TUid stubUID = iStubDataArray[stubIndex]->pkgUID;                 
                FLOG_1( _L("[VersionRev] Stub UID: 0x%x \n "), stubUID.iUid );                                                  
                                
                TBool isInstalled = sisRegSession.IsInstalledL( stubUID );
                                                                             
                if ( isInstalled )
                    {                                
                    err = entry.Open( sisRegSession, stubUID );                     
                    
                    if ( !err )
                        {
                        CleanupClosePushL( entry );                                                                                                                                                                                                                                                                                      
                                                
                        TUint drives = entry.InstalledDrivesL();
                        FLOG_1( _L("[VersionRev] Installe drive: 0x%x \n "), 
                                drives );                                                                                               
                                                
                        if ( drives == KVerRevDriveC )
                            { 
                            FLOG( _L("[VersionRev] SIS is installed to C:") );                                                        
                            FLOG( _L("[VersionRev] Check SIS version") );
                                                        
                            TVersion stubVersion(
                                    iStubDataArray[stubIndex]->major,
                                    iStubDataArray[stubIndex]->minor,
                                    iStubDataArray[stubIndex]->build );
                                                     
                            // Check sis and stub version.              
                            TInt ver = CompareVersions( 
                                   entry.VersionL(), 
                                   stubVersion ); 
                            FLOG_1( _L("[VersionRev] CompareVer.= %d "), ver );                                                                                           
                            TBool removePkg = EFalse;
                            
                            if ( ver == KVerRevSisVersionSame )
                                {
                                // If version is same check that all binary 
                                // files in rom is found. If files are missing
                                // do not remove sis pkg.
                                FLOG( _L("[VersionRev] Sis version same"));
                                removePkg = AllRomBinariesFoundL( entry );
                                }
                            if ( ver == KVerRevSisVersionOlder )
                                {
                                FLOG( _L("[VersionRev] Sis version older"));
                                removePkg = ETrue;
                                } 
                            
                            // Add uid to uninstall array if removePkg is true.
                            if ( removePkg )
                                {                                                                   
                                iSisUninstaller->AddUidToListL( stubUID );
                                FLOG_1( _L("[VersionRev] Uninst. UID:0x%x"), 
                                          stubUID.iUid );                                  
                                } 
                            }//if
                        CleanupStack::PopAndDestroy(); //entry                         
                        }//if                                                                   
                    } //if                                                                          
                } //for  
            CleanupStack::PopAndDestroy(); //sisRegSession
            } //if
          
        return KErrNone;
        }

    
    // -----------------------------------------------------------------------
    // This function reads firmware string and checks if firmware update has
    // been done. This function does create file to private directory and
    // saves current firmware version string to it.
    // -----------------------------------------------------------------------
    //    
    void CVersionRevisor::StartUpReasonL()
        {          
        FLOG( _L("[VersionRev] StartUpReasonL") );                           
        iStartupReason = KErrNotFound;                               
        HBufC* currentVersionString = HBufC::NewLC( KVerRevStrLength*2 );
        TPtr currentStringPtr = currentVersionString->Des();
        
        // Get current firmware string from sysutil. 
        // Firmware string is saved to Z:\\resource\\versions\\sw.txt
        SysUtil::GetSWVersion( currentStringPtr );
        FLOG_1( _L("[VersionRev] Firmware version = %S "), 
                &currentStringPtr );                 
        TUint result;
        // If private directory is not found create it. 
        TInt err = iFs.Att( KVerRevPrivatePath, result );
        
        if ( ! err == KErrNone && result & KEntryAttDir )
            {                 
            iFs.CreatePrivatePath( EDriveC ); 
            FLOG( _L("[VersionRev] StartUpReasonL: Create private folder") );
            }
               
        RFile file; 
        TFindFile find( iFs );                      
        // Check if version file is found in Daemon private director.
        err = find.FindByPath( KVerRevVersionFile(), &KVerRevPrivatePath() );
        FLOG_1( _L("[VersionRev] StartUpReasonL: Find file = %d"), err );  
        
        if ( err )
            {
            // If file is not found this is first boot or user memory (C:) has 
            // been formatted. No need to start version checking but let's save
            // the current firmware version to Daemon private folder.
            err = file.Create( 
                    iFs, 
                    KVerRevVersionFile, 
                    EFileWrite|EFileRead|EFileStreamText );
            
            FLOG_1( _L("[VersionRev] file create err = %d"), err ); 
            User::LeaveIfError( err );
            
            // Write current firmware version strign to file. 
            WriteFileText( file, currentStringPtr );
            
            file.Flush();
            file.Close();                        
            }
        else
            {
            
            HBufC* previousVersionString = HBufC::NewLC( KVerRevStrLength*2 );
            TPtr previousStringPtr = previousVersionString->Des();            
            
            // Open firmware version file from private directory.
            err = file.Open( 
                    iFs, 
                    KVerRevVersionFile, 
                    EFileWrite|EFileRead|EFileStreamText );
            
            FLOG_1( _L("[VersionRev] file open err = %d"), err ); 
            User::LeaveIfError( err );            
            
            // Read previous firmware string from the file. 
            ReadFileText( file, previousStringPtr );
            FLOG_1( _L("[VersionRev] Previous sw version: %S"), 
                    &previousStringPtr ); 
                        
            file.Close();
            
            if ( previousStringPtr.Length() != 0 )
                {                                                                             
                // Compare firmware strings.
                err = previousVersionString->Compare( currentStringPtr );
                FLOG_1( _L("[VersionRev] String compare = %d"), err ); 
                
                if ( err == KErrNone )
                    {  
                    // Strings are identical, no firmware update.
                    iStartupReason = KErrNone;
                    FLOG( _L("[VersionRev] iStartupReason = KErrNone") );                 
                    }
                else
                    {
                    iStartupReason = EFirmwareUpdate;
                    FLOG( _L("[VersionRev] iStartupReason = EFirmwareUpdate") );                     
                    file.Replace(                    
                        iFs, 
                        KVerRevVersionFile, 
                        EFileWrite|EFileStreamText );                                        
                    // Ok we have firmware update. Let's write new firmware 
                    // string to file and start version checking.
                    WriteFileText( file, currentStringPtr );                                                                                 
                    file.Flush();
                    file.Close();  
                    }
                }            
            CleanupStack::PopAndDestroy(); //previousVersionString  
            }                                                                   
        CleanupStack::PopAndDestroy( ); //currentVersionString                  
        }
     
    
    // -----------------------------------------------------------------------
    // Help function to read firmware version string from file.
    // -----------------------------------------------------------------------
    //  
    void CVersionRevisor::ReadFileText( RFile& aFile, TDes& aText )
        {                        
        TFileText fileText;
        fileText.Set( aFile );
        fileText.Read( aText );
         
         // Replace new-line patterns with real ones.
         TInt position = aText.Find( KVerRevNewLinePattern );
         while ( position != KErrNotFound )
             {
             // err is a position
             aText.Replace( 
                     position, 
                     KVerRevNewLinePattern().Length(), 
                     KVerRevNewLine );
             
             position = aText.Find( KVerRevNewLinePattern );
             }                        
        }

    
    // -----------------------------------------------------------------------
    // Help function to write firmware version string from file.
    // -----------------------------------------------------------------------
    //  
    void CVersionRevisor::WriteFileText( RFile& aFile, TDes& aText )
        { 
        // Replace real new-line marker with pattern. This makes 
        // reading operation easy.
        TInt position = aText.Find( KVerRevNewLine );                
        while ( position != KErrNotFound )
            {                
            aText.Replace( 
                    position, 
                    KVerRevNewLine().Length(), 
                    KVerRevNewLinePattern );
             
             position = aText.Find( KVerRevNewLine );
             }
                     
        TFileText fileText;
        fileText.Set( aFile );
        fileText.Write( aText );                       
        }
        
    
    // -----------------------------------------------------------------------
    // This function reads stub sis files version and uid and adds the info
    // in array.
    // -----------------------------------------------------------------------
    //  
    void CVersionRevisor::AppendStubInfoL( TDesC& aStubName )
        {                                             
        CFileSisDataProvider* provider = 
            CFileSisDataProvider::NewLC( iFs, aStubName );
                
        Sis::CController* stubData = Sis::CController::NewLC( *provider );
                
        const Sis::CVersion& version = stubData->Info().Version();
                
        TVersionRevStubData* stubPkg = new( ELeave ) TVersionRevStubData();
        CleanupStack::PushL( stubPkg );        
        stubPkg->pkgUID = stubData->Info().Uid().Uid();
        stubPkg->major = version.Major();
        stubPkg->minor = version.Minor();
        stubPkg->build = version.Build();
        
        iStubDataArray.AppendL( stubPkg );
        
        CleanupStack::Pop( stubPkg );
        
 #ifdef _DEBUG  
        RDebug::Print( _L("[VersionRev] Add Stub UID: 0x%x Ver: %d.%d.%d\n "),                  
                  stubData->Info().Uid().Uid(),
                  version.Major(),
                  version.Minor(),
                  version.Build() );        
 #endif
                    
        CleanupStack::PopAndDestroy( 2, provider );
        provider = NULL;
        stubData = NULL;
        stubPkg = NULL;        
        }

    // -----------------------------------------------------------------------    
    // This function compares sis and stub versions. 
    // -----------------------------------------------------------------------
    //   
    TInt CVersionRevisor::CompareVersions( 
        TVersion aSisVer, 
        TVersion aStubVer )
        { 
        FLOG( _L("[VersionRev] CompareVersions()") );
                        
        // Compare major version
        if ( aSisVer.iMajor > aStubVer.iMajor  )
            {
            FLOG( _L("[VersionRev] STUB major version older ") );
            return KVerRevSisVersionNewer;
            }
        // If same major version, check minor version.
        else if ( aSisVer.iMajor == aStubVer.iMajor )
            {                           
            if ( aSisVer.iMinor > aStubVer.iMinor )
                {
                FLOG( _L("[VersionRev] STUB minor version older ") );
                return KVerRevSisVersionNewer;
                }
            // If same minor version, check build version.
            else if ( aSisVer.iMinor == aStubVer.iMinor )
                {
                if ( aSisVer.iBuild > aStubVer.iBuild  )
                    {
                    FLOG( _L("[VersionRev] STUB build version older ") );
                    return KVerRevSisVersionNewer;                        
                    }  
                else if ( aSisVer.iBuild == aStubVer.iBuild  )
                    {
                    // Sis build version is same as stub.
                    FLOG( _L("[VersionRev] Build version same ") );
                    return KVerRevSisVersionSame;                        
                    }                                  
                else // build
                    {
                    // Sis build version is older.
                    FLOG( _L("[VersionRev] SIS build version OLDER !") );
                    return KVerRevSisVersionOlder;
                    }
                }
            else // minor
                {
                // Sis minor version is older.
                FLOG( _L("[VersionRev] SIS minor version OLDER !") );
                return KVerRevSisVersionOlder;
                }            
            }        
        else //major
            {
            // Sis major version is older.
            FLOG( _L("[VersionRev] SIS major version OLDER !") );
            return KVerRevSisVersionOlder;
            }            
        }
  
    // -----------------------------------------------------------------------    
    // This function check that all sis pkg's binary files are found from rom. 
    // -----------------------------------------------------------------------
    //     
    TBool CVersionRevisor::AllRomBinariesFoundL( Swi::RSisRegistryEntry& aEntry )
        {
        FLOG( _L("[VersionRev] AllRomBinariesFoundL") );
        TBool allBinaryFilesFound = EFalse;
        TInt err = KErrNone;
        RPointerArray<HBufC> sisFilesArray;
        RPointerArray<HBufC> binaryFilesArray;
                
        // Get installed files from this entry.
        aEntry.FilesL( sisFilesArray );
        TInt arrayCount = sisFilesArray.Count();   
        FLOG_1( _L("[VersionRev] Sis files count = %d"), arrayCount );
        
        if ( arrayCount )
            {                                                   
            for ( TInt index = 0; index < arrayCount; index++ )
                {
                // Get file path
                HBufC* tempFilePath = sisFilesArray[index]->AllocL();
                CleanupStack::PushL( tempFilePath );
                FLOG_1( _L("[VersionRev] File path: %S \n"), tempFilePath );                
                
                // Check if file is installed to sys\bin folder.
                // Search \sys\bin string from the descriptor's data.
                err = tempFilePath->Find( KVerRevSysPath );
                FLOG_1( _L("[VersionRev] Sys path found = %d"), err );
                
                if ( err != KErrNotFound )
                    {
                    // If binary file, add to array.
                    binaryFilesArray.Append( tempFilePath );
                    CleanupStack::Pop(); //tempFilePath
                    }
                else
                    {
                    // Delete temp descriptor.
                    CleanupStack::PopAndDestroy( tempFilePath );
                    tempFilePath = NULL;
                    }                
                }
            
            // Count how meny binary files are found.
            TInt binaryCount = binaryFilesArray.Count();
            FLOG_1( _L("[VersionRev] Binary files count = %d"), binaryCount );
            if ( binaryCount )
                {
                TInt foundInRomCount = 0;
                TFindFile find( iFs );
                TParse stringParser;                
                TFileName binaryFileNameAndExt;
                
                for ( TInt index = 0; index < binaryCount; index++ )
                     {
                     // Get binary file path.
                     HBufC* binaryPathBuf = binaryFilesArray[index];
                     // Parse file name and extension.                                                  
                     stringParser.Set( *binaryPathBuf ,NULL, NULL );                                                                                  
                     binaryFileNameAndExt.Copy( stringParser.NameAndExt() );                                          
                     FLOG_1( _L("[VersionRev] Search file: %S \n"), 
                             &binaryFileNameAndExt );   
                                                             
                     // Search file in z:\sys\bin.
                     // Note path must contain drive letter.
                     err = find.FindByPath( 
                             binaryFileNameAndExt, 
                             &KVerRevRomSysPath() ); 
                     FLOG_1( _L("[VersionRev] Found in rom = %d"), err );
                     
                     if ( err == KErrNone )
                         {
                         // File found in rom. Increase counter.
                         foundInRomCount++;
                         }                                                  
                     } //for
                
                // Check that all binary files are found in rom.
                if ( binaryCount == foundInRomCount )
                    {
                    allBinaryFilesFound = ETrue;
                    FLOG( _L("[VersionRev] All rom file found !") );
                    }                
                } //if  
            
            // Free all objects and array.
            binaryFilesArray.ResetAndDestroy();
            }
        // Free all objects and array.
        sisFilesArray.ResetAndDestroy();
        
        return allBinaryFilesFound;
        }
    
    
    // -----------------------------------------------------------------------
    // This function handles VersionRevisors self exit.
    // -----------------------------------------------------------------------
    //      
     void CVersionRevisor::Exit()
         {
         FLOG( _L("[VersionRev] Exit() ") );         
         // Ok all is done. Free all resources and exit.
         delete this;
         }

    } // namespace Swi

//EOF
