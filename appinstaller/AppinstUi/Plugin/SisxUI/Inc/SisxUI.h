/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CSisxUI class.
*
*                This class implements the ECom SWInstUIPluginAPI interface 
*                for native installation.
*
*/


#ifndef SISXUI_H
#define SISXUI_H

//  INCLUDES
#include <e32base.h>

#include "SWInstUIPluginAPI.h"

namespace SwiUI
{

class CSisxUIOperationWatcher;

/**
* This class implements the ECom SWInstUIPluginAPI interface for sisx 
* installation.
*
* @lib sisxui
* @since 3.0 
*/
class CSisxUI : public CSWInstUIPluginAPI
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSisxUI* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CSisxUI();

    public:  // From base classes

        /**
        * From SWInstUIPlugin, Perform installation.
        * @since 3.0
        */
        void InstallL( RFile& aFile,
                       TInstallReq& aInstallParams, 
                       TBool aIsDRM,
                       TRequestStatus& aStatus );
        
        /**
        * From SWInstUIPlugin, Perform silent installation.
        * @since 3.0
        */
        void SilentInstallL( RFile& aFile,
                             TInstallReq& aInstallParams, 
                             TBool aIsDRM, 
                             TInstallOptions& aOptions,
                             TRequestStatus& aStatus );
        
        /**
        * From SWInstUIPlugin, Perform uninstallation.
        * @since 3.0
        */
        void UninstallL( const TUid& aUid, 
                         const TDesC8& aMIME,
                         TRequestStatus& aStatus );
        
        /**
        * From SWInstUIPlugin, Perform silent uninstallation.
        * @since 3.0
        */
        void SilentUninstallL( const TUid& aUid, 
                               const TDesC8& aMIME, 
                               TUninstallOptions& aOptions,
                               TRequestStatus& aStatus );
        
        /**
        * From SWInstUIPlugin, Indicates if the application shell must be updated.
        * @since 3.0
        */
        TBool IsAppShellUpdate();

        /**
        * Cancel the current operation.
        * @since 3.0
        */
        void Cancel();
        
        /**
        * Informs the plugin about completed operation. Called when all tasks are completed.
        * @since 3.0
        */
        void CompleteL();

        /**
        *
        */
        CErrDetails* GetErrorDetails();

        /**
        * From SWInstUIPlugin, Perform a custom uninstallation.
        * @since 3.0
        */        
        void CustomUninstallL( TOperation aOperation, 
                               const TDesC8& aParams, 
                               TRequestStatus& aStatus );
        
        /**
        * From SWInstUIPlugin, Perform a silent custom uninstallation.
        * @since 3.0
        */      
        void SilentCustomUninstallL( TOperation aOperation,
                                     TUninstallOptions& aOptions,
                                     const TDesC8& aParams, 
                                     TRequestStatus& aStatus );
        
    private:

        /**
        * Constructor.
        */  
        CSisxUI();
         
        /**
        * 2nd phase constructor.
        */
        void ConstructL();


    private: //  Data
            
        TInt iResourceFileOffset;

        CSisxUIOperationWatcher* iWatcher;   
    };
}

#endif      // SISXUI_H   
            
// End of File
