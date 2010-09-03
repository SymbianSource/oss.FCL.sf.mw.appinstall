/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Makes installtion of sis pkgs.
*
*/



#ifndef IAUPDATERINSTALLER_H
#define IAUPDATERINSTALLER_H

#include <e32base.h>
#include <f32file.h>

#include <usif/sif/sif.h>

class CIAUpdaterMngr;
class CIAUpdaterSilentLauncher;
        
/**
 *  This class implements installation for IAUpdater.exe.
 *
 *  @code   
 *  @endcode
 *  @lib 
 *  @since S60 
 */ 
class CIAUpdaterInstaller : public CActive
    {
public:

    enum TDSisInstallerState
        {           
        EDSisInstallerStateIdle,
        EDSisInstallerStateInstallerBusy,
        EDSisInstallerStateInstalling     
        };        


    /**
     * Two-phased constructor.
     */    
    static CIAUpdaterInstaller* NewL( 
        RFs& aFs,    
        CIAUpdaterMngr& aIAUpdater );

	
    /**
     * C++ Destructor
     */
    ~CIAUpdaterInstaller();
	

    /**
     * Adds a file to the list of files to be installed.
     *
     * @since S60 
     * @param aFileName The full path of the file to be installed.     
     */        
    void AddFileToInstallL( const TDesC& aFileName );

	
    /**
     * Install the specified sis file
     *
     * @note This is a synchronous function that uses asynchronous installation.
     * So, this function will return after installation is finished.
     * This uses CActiveSchedulerWait::Start() and CActiveSchedulerWait::AsyncStop().
     * So, be cautious about nested loops. 
     *
     * @since S60      
     */        
    void StartInstallingL();


    /**
     * Cancels the possible on-going operation by calling Cancel.
     * Also, calls CActiveSchedulerWait::AsyncStop if it is required 
     * to release the nested loop.
     *
     * @note User of this object should call this function instead of directly 
     * calling DoCancel because this function can release nested loop waits
     * correctly.
     */
    void CancelOperation();


private: // From CActive
    
    /**
     * Called by Cancel when a request from this class 
     * is outstanding.
     *
     * @since S60 
     */            
    void DoCancel();
            
    /**
     * Asynchronous request processing.
     *
     * @since S60      
     */            
    void RunL();

    /**
     * Handles any leaves from the RunL
     *
     * @since S60 ?S60_version
     * @param aError Erroro code.     
     * @return TInt Return code.
     */            
    TInt RunError( TInt aError );


private:

    /**
     * C++ constructor
     */
    CIAUpdaterInstaller( 
        RFs& aFs,    
        CIAUpdaterMngr& aIAUpdater );

    /**
     * Constructor
     */
    void ConstructL( );

         
    /**
     * Self request completion
     *
     * @since S60 
     */            
    void CompleteSelf();

       
    /**
     * Completes the installation and stops active scheduler.
     *
     * @since S60 
     * @param aResult Installation result
     */            
    void InstallationCompleted( TInt aResult );

    
    /**
     * Checks that given installation pkg is a valis sisx pkg.
     *
     * @since S60 
     * @param aPackageName The full paht of the sis pkg.
     * @return TBool ETrue is file is sisx type. 
     */            
    TBool IsValidPackageL( const TDesC& aPackageName );      
    	          

private: // data

    // File server
    RFs& iFs;
    
    // Updater manager.
    CIAUpdaterMngr& iIAUpdater;  		

    // The state
    TDSisInstallerState iState;    

    // Installer error
    TInt iInstallErr;

    // Sis pkgs index
    TInt iFileIndex;
    
    // Sis file and location
    TBuf<KMaxFileName> iSisFile;    

    // Installation pkgs.
    RPointerArray<HBufC> iFilesToInstall;
   
    // Timer
    RTimer iTimer;
    
    // Silent installation launcher.    
    CIAUpdaterSilentLauncher* iInstallLauncher;

    // CActiveSchedulerWait object can be used as a data member 
    // inside other CBase derived classes.
    CActiveSchedulerWait iWaiter;   
    
    // Installaion results
    Usif::COpaqueNamedParams* iResults;

    };		

#endif //CIAUPDATERINSTALLER_H

//EOF
