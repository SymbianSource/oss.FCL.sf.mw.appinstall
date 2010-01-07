/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CSisxUIOperationWatcher class 
*
*                This class handles the asynchronous install / unsinstall 
*                requests.
*
*/


#ifndef SISXUIOPERATIONWATCHER_H
#define SISXUIOPERATIONWATCHER_H

//  INCLUDES
#include <e32base.h>
#include <SWInstStartupTaskParam.h>
#include <SWInstLogTaskParam.h>

#include "SisxUIHandler.h"
#include "CUICancelTimer.h"

// FORWARD DECLARATIONS
namespace ContentAccess
{
class CManager;
class TAgent;
}

namespace Swi 
{ 
class CInstallPrefs; 
class CAsyncLauncher;
class CSisRegistryPackage;
}

namespace SwiUI
{

// FORWARD DECLARATIONS

class CTaskManager;
class CTask;
class CSisxUIAppInfo;

// CLASS DECLARATION

/**
* This class handles the asynchronous install / unsinstall requests.
*
* @lib sisxui.lib
* @since 3.0
*/
class CSisxUIOperationWatcher : public CActive, 
                                public MSisxUICancelObserver,
                                public CommonUI::MCUICancellable
    {
    public:  // Constructors and destructor
     
        /**
        * Two-phased constructor.
        */
        static CSisxUIOperationWatcher* NewL();

        /**
        * Destructor.
        */
        virtual ~CSisxUIOperationWatcher();

    public: // New functions

        /**
        * Handle install request.
        * @since 3.0
        */
        void InstallL( RFile& aFile, 
                       const TDesC8& aMIME, 
                       TBool aIsDRM,
                       TRequestStatus& aRequestStatus );

        /**
        * Handle silent install request.
        * @since 3.0
        */
        void SilentInstallL( RFile& aFile, 
                             const TDesC8& aMIME, 
                             TBool aIsDRM,
                             TInstallOptions& aOptions, 
                             TRequestStatus& aRequestStatus );
        
        /**
        * Handle uninstall request.
        * @since 3.0
        */
        void UninstallL( const TUid& aUid, TRequestStatus& aRequestStatus );

        /**
        * Handle silent uninstall request.
        * @since 3.0
        */
        void SilentUninstallL( const TUid& aUid, 
                               TUninstallOptions& aOptions, 
                               TRequestStatus& aRequestStatus );
        /**
        * Uninstalls only a specific augmentation (sispatch)
        * @since 3.0
        * @param aUid - Uid of the package.
        * @param aIndex - Index of the augmentation.
        */
        void UninstallAugmentationL( const TUid& aUid, TInt aIndex, TRequestStatus& aRequestStatus ); 

        /**
        * Uninstalls only a specific augmentation (sispatch) - silently.
        * @since 3.0
        * @param aUid - Uid of the package.
        * @param aOptions - Options for the silent operation.
        * @param aIndex - Index of the augmentation.
        */
        void SilentUninstallAugmentationL( const TUid& aUid, 
                                           TUninstallOptions& aOptions,
                                           TInt aIndex, 
                                           TRequestStatus& aRequestStatus );         

        /**
        * Informs the UI, about final completion.
        * @since 3.0
        */
        void FinalCompleteL();  

    public: // Functions from base classes

        /**
        * From CActive, Override.
        * @since 3.0
        */
        void Cancel();

        /**
        * From CActive, Called by framework when request is finished.
        * @since 3.0
        */
        void RunL();

        /**
        * From CActive, Called by framework when request is cancelled.
        * @since 3.0
        */        
        void DoCancel();  

        /**
        * From CActive, Handles a leave occurring in the request completion 
        * event handler RunL().
        * @since 3.0
        */
        TInt RunError( TInt aError );

        /**
        * From MSisxUICancelObserver, Called when user has cancelled the operation.
        * @since 3.0
        */  
        void UserCancelL( TBool aCancelImmediately );   

        /**
        * From MCUICancellable, Indicates if a modal dialog is currently shown.
        */
        TBool IsShowingDialog();

        /**
        * From MCUICancellable, Cancels the installation engine.
        */       
        void CancelEngine();

        /**
        * From MCUICancellable, Called when nothing else has worked.
        */               
        void ForceCancel();         

        /**
        * From MCUICancellable, Called to indicate that the cancelling progress 
        * has been started.
        */  
        void StartedCancellingL();         

    private:

        /**
        * Completes the pending request.
        * @param aResult - Result of the request
        */
        void CompleteRequest( TInt aResult );
        
    private:

        /**
        * C++ default constructor.
        * @param aRequestStatus - Status of the request.
        */
        CSisxUIOperationWatcher();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Add files to startup list.
        * @since 3.0
        */
        void AddStartupItemsL( RArray<TStartupTaskParam>& aParams );
        
        /**
        * Remove files from the startup list.
        * @since 3.0
        */
        void RemoveStartupItemsL( RArray<TStartupTaskParam>& aParams );

        /**
        * Checks if the given uid has installed a startup file. If it finds one
        * it removes it and creates a task to remove the items as well.
        * @since 3.0
        */
        void RemoveStartupFileL( TUid aUid );        

        /**
        * Handles the log entry addition.
        * @since 3.0
        * @param aAppInfo - Package info.
        * @param aAction - Action of the log.
        * @param aIsStartup - Indicates if the startup list was modified.
        */
        void HandleLogL( const CSisxUIAppInfo& aAppInfo, 
                         const TLogTaskAction& aAction, 
                         TBool aIsStartup );   

        /**
        * Helper to handle the case of pip file.
        * @since 3.0
        * @param aPipFileHandle - Handle to pip file.
        * @param aSisFileHandle - On return contains handle to sis file.
        */
        void HandlePipFileL( RFile& aPipFileHandle, RFile& aSisFileHandle );        

        /**
        * Helper to read ocsp values from central repository and prepare the ui.
        * @since 3.0
        * @param aUIHandler - UI handler.
        * @param aPrefs - Engine install preferences.
        */
        void PrepareOCSPL( CSisxUIHandler* aUIHandler, Swi::CInstallPrefs* aPrefs );

        /**
        * Helper to get the packages that were installed in this session.
        * @since 3.0
        * @param aPackages - On return contains the installed packages.
        */
        void GetInstalledPackagesL( RPointerArray<CSisxUIAppInfo>& aPackages );
        
        /**
        * Helper to handle startup list addition.
        * @since 3.0
        * @param aAppInfo - Installed application.
        */
        void HandleStartupAdditionL( const CSisxUIAppInfo& aAppInfo ); 

        /**
        * Prepares the installation.
        * @since 3.0
        */
        void PrepareInstallL();

        /**
        * Completes the installation
        * @since 3.0
        */
        void CompleteInstallationL();

        /**
        * Completes the uninstallation
        * @since 3.0
        */
        void CompleteUninstallationL();  

        /**
        * Performs the uninstallation of augmentation.
        * @since 3.0
        * @param aUid - Uid of the package.
        * @param aIndex - Index of the augmentation.
        */
        void DoUninstallAugmentationL( TUid aUid, TInt aIndex ); 

        /**
        * Notifies that uninstallation is about to start.
        * @since 3.0
        * @param aUid - Uid of the package to be uninstalled.
        */
        void NotifyStartUninstallL( TUid aUid );
        
        /**
        * Notifies that uninstallation has been completed.
        * @since 3.0
        */
        void NotifyEndUninstall();   

        /**
        * Handles searching correct manager for DRM registration
        * @since 3.0
        * @param aAgent - content access agent
        * @return Manager - Manages files and content access agents
        */        
        ContentAccess::CManager* GetCafDataL( ContentAccess::TAgent& aAgent );
        
        /**
         * Register PIP package content to DRM DCF server.
         * @since 5.0
         * @param aUID - Sis package UID       
         */                        
         void RegisterContentToDCF( TUid aUid );
  
         /**
          * Search files which should be register to DCF server.
          * @since 5.0
          * @param aSourceArray - Array conteining file paths.
          * @param aTargetArray - Array for files which should be register.     
          */                      
         void SearchFilesForDCFL( 
             RPointerArray<HBufC>& aSourceArray,
             RPointerArray<HBufC>& aTargetArray );        
        
                
    private: // Data

        Swi::CAsyncLauncher* iLauncher;        
        CSisxUIHandler* iUIHandler;
        CTaskManager* iTaskManager;        

        TRequestStatus* iRequestStatus;
        RFile iSourceFile;
        HBufC8* iMIME;        

        TUid iUninstalledUid;

        // Pip file related
        RFile iTempFile;  // Points to extracted sis file
        RFs iRfs;        
        TBool iIsPipFile;
        TFileName iTempDir;        

        RPointerArray<HBufC> iInstalledFiles;
        RPointerArray<Swi::CSisRegistryPackage> iAugmentations;        

        TBool iStartupModified; 
        TBool iPreparing;  
        TBool iCancelOngoing;   
        TBool iIsUserCancel;        
        TBool iIsAugmentation;        

        TBool iIsSilent;
        TBool iDoSilentOcsp;        
        
        CommonUI::CCUICancelTimer* iCanceller;
        RArray<TInt> iSupportedLanguages;
        TBool iDeletionOngoing;
    };
}

#endif      // SISXUIOPERATIONWATCHER_H   
            
// End of File
