/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <f32file.h>
#include <apgcli.h>
#include "ShutdownWatcher.h"    // MShutdownObserver

// Forward declarations
	
class CPreviouslyInstalledAppsCache;
class CInstallationFailedAppsCache;

namespace Swi
{
// Modified for S60
class CSilentLauncher;
class CDialogWrapper;   
class CProgramStatus; //For uninstaller
        
/**
* Describes the installation behaviour of any sis files found by the Daemon
*/
class MDaemonInstallBehaviour
    {
    public:
        
        /**
         * Notify plug-in that daemon has complete media processing.
         * Plug-in can start installation.          
         */
        virtual void DoNotifyMediaProcessingComplete() = 0; 
 
        /**
         * Returns ture is plugin is loaded.
         * 
         * @Return TBool ETrue if plug-in is available.          
         */        
        virtual TBool IsPlugin() = 0;

        /**
         * This function does check if plugin is all ready notified that media
         * processing is started. If not it calls MediaProcessingStart.
         * 
         * @Return TBool ETrue if plug-in is available.          
         */            
        virtual void NotifyPlugin() = 0;
          
        /**
         * Reguest plug-in to install given package.
         *  
         * @param aDrive aSisFile Package file.
         */             
        virtual void RequestPluginInstall( TDesC& aSisFile ) = 0;              
    };

/**
* The Daemon SIS installer
*/
class CSisInstaller : public CActive, public MShutdownObserver
    {
    public:

        enum TDSisInstallerState
            {           
#ifdef RD_MULTIPLE_DRIVE                      
            EDSisInstallerStateIdle,
            EDSisInstallerStateInstallerBusy,
            EDSisInstallerStateInstalling,
            EDSisInstallerStateCompleted
#else
            EDSisInstallerStateIdle,
            EDSisInstallerStateInstallerBusy,
            EDSisInstallerStateInstalling     
#endif //RD_MULTIPLE_DRIVE 
            };        

        /**
        * Symbian construction (for member variables)
        */
        static CSisInstaller* NewL(
                MDaemonInstallBehaviour* aDaemonBehaviour,
                CProgramStatus& aMainStatus ); //For uninstaller
		
        /**
        * C++ destructor
        */
        ~CSisInstaller();
		
        /**
        Adds a file to the list of files to be installed
        @param aFileName The full name and path of the file to be installed
        */
        void AddFileToInstallL(const TDesC& aFileName);
		
        /**
        * Install the specified sis file
        */
        void StartInstallingL();
        
#ifdef RD_MULTIPLE_DRIVE 
        /**
        * Returns status of installer.
        */
        TBool IsInstalling();        
#endif                

    private: // From CActive
	
        /**
        * Called by Cancel when a request from this class is outstanding
        */
        void DoCancel();
                
        /**
        * Asynchronous completion request processing
        */
        void RunL();
	
        /**
        * Handles any Leaves from the RunL
        */
        TInt RunError(TInt aError);
        
    private:  // From MShutdownObserver
        /**
        * Called by CShutdownWatcher when the system is closing down
        */
        void NotifyShuttingDown();        

    private:
        /**
        * C++ constructor
        */
        CSisInstaller( MDaemonInstallBehaviour* aDaemonBehaviour );
	
        /**
        * 2nd phase constructor
        */
        void ConstructL( CProgramStatus& aMainStatus ); //Uninstaller
	      
        /**
        * Self request completion
        */
        void CompleteSelf();
        
        /**
        * Completes the installation procedure.
        */
        void InstallationCompleted( TInt aResult );
        
        /**
        * Indicates if this package was ever installed or not.
        */
        TBool NeedsInstallingL( const TDesC& aPackageName );

        /**
        * Indicates if the media is present in the drive.
        */
        TBool IsMediaPresent( TChar aDrive );

        /**
        * Indicates if this package is valid.
        */
        TBool IsValidPackageL(); 
        
        /**
         * Calculate precent value for universal indicator.
         * This precent value show how many packages has been installed.
         */      
        void CalcPercentValue();
        	          
	
    private:
               
        MDaemonInstallBehaviour* iDaemonBehaviour;	
        // The state
        TDSisInstallerState iState;	
        RPointerArray<HBufC> iFilesToInstall;	
        // Sis files and location
        TBuf<KMaxFileName> iSisFile;
        TInt iFileIndex;	
        // Timer and time step
        RTimer iTimer;                
        CSilentLauncher* iInstallLauncher;
        CDialogWrapper* iDialogs;
        TInt iInstallErr;
        RFs iFs;
        RApaLsSession iApaSession;
        // Pointer to the already ever installed checker class
        CPreviouslyInstalledAppsCache* iPreviouslyInstalledAppsCache;
        // Pointer to failed installation cache.
        CInstallationFailedAppsCache* iInstallationFailedAppsCache;
		
#ifdef RD_MULTIPLE_DRIVE 
        // Defines state of Installer 
        TDSisInstallerState iInstallerState;
#endif     		
        // PkgID
        TUid iCurrentPackageId;
        CShutdownWatcher* iShutdownWatcher;
        // For uninstaller
        TInt iGeneralProcessStatus;
        CProgramStatus* iProgramStatus;   
        // For plugin support
        TBool iUpdateCache;
        // File handle for USIF.
        RFile iSisFileHandle;
        TBool iFileOpen;
        //CDialogTimer* iDialogTimer; 
        TReal32 iPercentValue;
    };		
}

