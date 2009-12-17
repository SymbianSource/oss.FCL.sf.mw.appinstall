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

#ifndef VERSIONREVISOR_H
#define VERSIONREVISOR_H

#include <e32base.h>
#include <f32file.h>

#include "sisregistryentry.h"
#include "uninstaller.h"
#include "programstatus.h"

/**
 *  TVersionRevStubData class for stub data. 
 */
class TVersionRevStubData
    {
    public:
        TUid pkgUID;
        TInt major;
        TInt minor;
        TInt build;
    };

namespace Swi
{

/**
 *  CVersionRevisor class handles version checking for rom updates. 
 *
 *  This class reads stub sis files after firmware update and checks
 *  rom updates from user dirve.
 */
class CVersionRevisor : public CBase
    {
public:
    enum TStartUpReason
        {
        EFirmwareUpdate = 100
        };
 
    /**
    * Two-phased constructor.
    */    
    static CVersionRevisor* NewL( CProgramStatus& aMainStatus );
    
    /**
    * Two-phased constructor.
    */    
    static CVersionRevisor* NewLC( CProgramStatus& aMainStatus );
    
    /**
    * Destructor.
    */    
    ~CVersionRevisor();
    
public: 
    
    /**
      * This function runs all main tasks for VersionRevisor 
      * and starts uninstall if needed. 
      * 
      * @return TInt Error code.    
      */   
    TInt StartProcessL();
    
    /**
      * This function handles self exit for VersionRevisor.
      * 
      */       
    void Exit();
    
private:
    
    /**
      * 2nd phase constructor.      
      * @param aMainStatus - Program status     
      */     
    void ConstructL( CProgramStatus& aMainStatus );
    
    /**
      * This function reads stub sis file's version and UID and adds
      * the UID and version info to array.    
      * 
      * @return TInt Error code  
      */   
    TInt ProcessStubFilesL();
    
    /**
      * This function searches installed rom updates from sis registry
      * and compares version to stub version.    
      *  
      * @return TInt Error code  
      */                       
    TInt ProcessSisRegistryL();
     
    /**
      * This function checks if firmware has change. Function reads version 
      * string from the device and compares that to previous version. Function
      * creates file to private directory in where it save the latest firmware 
      * string.  
      *       
      */     
    void StartUpReasonL();
      
    /**
      * This function reads stub sis files version from sis controller and
      * adds stub info to array.     
      *  
      * @param aStubName Stub's name
      */       
    void AppendStubInfoL( TDesC& aStubName );
    
    /**
      * This function compares rom update's (sis) and stub's version numbers. 
      *      
      * @param aSisVer Sis packages version
      * @param aStubVer Stub sis version
      * @return TInt 
      *     Returns KVerRevSisVersionOlder if sis version is older.
      *     Returns KVerRevSisVersionSame if sis version same as stub version.
      *     Returns KVerRevSisVersionNewer if sis version is newer. 
      */     
    TInt CompareVersions( TVersion aSisVer, TVersion aStubVer );
    
     /**
       * This function checks that all binary fails are found in rom.
       *      
       * @param  aEntry     
       * @return TBool ETure if all binaries are found in rom.        
       */        
    TBool AllRomBinariesFoundL( Swi::RSisRegistryEntry& aEntry );
    
    /**
      * Helper function for StartUpReasonL. Reads firmware string.
      *  
      * @param aFile File containing string.
      * @param aText Buffer for string.        
      */      
    void ReadFileText( RFile& aFile, TDes& aText );
    
    /**
      * Helper function for StartUpReasonL. Writes firmware string.
      * This function is used to store current firmware string to
      * private directory.      
      * 
      * @param aFile File to be writen.
      * @param aText Buffer for string.        
      */      
    void WriteFileText( RFile& aFile, TDes& aText );    
    
    
private:
    
    RFs iFs;  
    // Defines startup reason e.g. firmware update.  
    TInt iStartupReason;        
    // Instance of sis uninstaller AO.
    CSisPkgUninstaller* iSisUninstaller; 
    // Revisor do not own this instance. Parameter defines the current
    // status of swi daemon. This status is updated after install or 
    // uninstall process has finished.
    CProgramStatus* iProgramStatus;     
    // Array containing stub version info.
    RPointerArray<TVersionRevStubData> iStubDataArray;    
    };
    
} // namespace Swi

#endif /*VERSIONREVISOR_H*/

//EOF
