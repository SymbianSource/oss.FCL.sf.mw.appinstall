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


#ifndef UNINSTALLER_H
#define UNINSTALLER_H

#include <e32base.h>
#include <f32file.h>
#include <apgcli.h>

#include "programstatus.h"
#include "silentuninstaller.h"
    
namespace Swi
{ 
class CVersionRevisor;
class CDialogWrapper;
/**
 *  CSisPkgUninstaller class handles asyncronous uninstalling for sisx. 
 *
 *  @since S60 3.2
 */
class CSisPkgUninstaller : public CActive
    {
    public:

        // Uninstaller state.
        enum TUninstallerState
            {           
            EUninstallerStateUninstalling,
            EUninstallerStateIdle, 
            EUninstallerStateBusy           
            };        
      
        /**
         * Two-phased constructor.
         */    
        static CSisPkgUninstaller* NewL( 
                CProgramStatus& aMainStatus, 
                CVersionRevisor& aRevisor );
        
        /**
         * C++ destructor
         */
        ~CSisPkgUninstaller();
                
        /**
          * This function starts uninstall process for sisx. Leavs if 
          * there is nothing to uninstall.
          * 
          * @since 3.2      
          */   
        void StartUninstallL(); 
        
        /**
         * This function adds sis pkg UID to uninstall array. Uninstaller 
         * sends UIDs to SWI uninstaller.
         * 
         * @since 3.2 
         * @param aUID Package UID to be added to array.
         */        
        void AddUidToListL( const TUid& aUID );

    private: // From CActive
    
        /**
         * Called by Cancel when a request is outstanding.   
         * 
         * @since 3.2     
         */
        void DoCancel();
                
        /**
         * Asynchronous uninstall request processing. 
         * 
         * @since 3.2        
         */
        void RunL();
    
        /**
         * Handles any Leaves from the RunL.  
         * 
         * @since 3.2 
         * @param aError Error code
         * @return TInt Error code     
         */
        TInt RunError( TInt aError );

    private:
        /**
         * C++ constructor
         */
        CSisPkgUninstaller();
    
        /**
         * 2nd phase constructor
         * 
         * @param aMainStatus SWI Daemon program main status.
         * @param aReviso Pointer to CVersionRevisor instance.
         */
        void ConstructL( CProgramStatus& aMainStatus, CVersionRevisor& aRevisor );
          
        /**
         * Self request completion.
         * 
         * @since 3.2         
         */
        void CompleteSelf();

        /**
         * This is called after all uids are uninstalled.
         * 
         * @since 3.2 
         * @param aResult Result of uninstall process.
         */
        void UninstallationCompleted( TInt /*aResult*/ );
        
        /**
         * Stop running and exit. Uninstaller calls this when all
         * tasks are done. Function calls CVersionRevisor::Exit() for destruction.
         * 
         * @since 3.2 
         */       
        void ExitUninstaller();
        
        /**
          * Calculate precent value for universal indicator.
          * This precent value show how many packages has been uninstalled.
          */              
        void CalcPercentValue();
                                             
    private:
                
        RFs iFs;
        // Internal wait timer. Used for waiting of install process etc.        
        RTimer iTimer;        
        // Array for sis package uids.
        RArray<TUid> iPkgUidArray;                
        RApaLsSession iApaSession; 
        // Uninstaller AO state.
        TUninstallerState iState;
        // Defines SWI Daemon main state (installing, uninstallin, idle).        
        TInt iGeneralProcessStatus;
        // Current array index.
        TInt iUidArrayIndex;
        // Current sis pkg uid.
        TUid iUID;
        // Not own. CProgramStatus instance, which holds Daemon's main status. 
        CProgramStatus* iProgramStatus;
        // Class to controll uninstalling process.
        CSilentUninstaller* iSilentUninstaller;
        // Not own. CVersionRevisor instance, which starts uninstall.
        CVersionRevisor* iRevisor;
        // Uninstaller dialog.        
        CDialogWrapper* iDialogs;
        // Descriptor for mime type.
        HBufC* iMime;  
        // Percent value for universal indicator.
        TReal iPercentValue;
    };      

} //namespace Swi

#endif /*UNINSTALLER_H*/

//EOF

