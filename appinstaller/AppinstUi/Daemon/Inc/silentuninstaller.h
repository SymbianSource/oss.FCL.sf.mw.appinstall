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


#ifndef SILENTUNINSTALLER_H
#define SILENTUNINSTALLER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <SWInstApi.h>
#include <usif/sif/sif.h>
#include <usif/sif/sifcommon.h>

#include "sisregistrysession.h"

namespace Swi
{
/**
 *  CSilentUninstaller class handles uninstalling of the single sisx file. 
 */
class CSilentUninstaller : public CBase
    {
    public:  
        
        /**
         * Two-phased constructor.
         * 
         * @param aFs File server session.
         */
        static CSilentUninstaller* NewL( RFs& aFs );
        
        /**
         * Destructor.
         */
        virtual ~CSilentUninstaller();

    public:  

        /**
         * This function requests silent uninstalling from the 
         * SWI installer.
         *          
         * @param aUid Package UID
         * @param aReqStatus Request status
         * @param aMIME MIME type descriptor.
         */
        void UninstallL(         
                TUid& aUid, 
                TRequestStatus& aReqStatus, 
                TDesC& aMIME  );

        /**
         * Cancel the current installation.        
         */
        void Cancel();        
        
    private:

        /**
         * Constructor.
         * 
         * @param aFs File server 
         */  
        CSilentUninstaller( RFs& aFs );
         
        /**
         * 2nd phase constructor.
         */
        void ConstructL();

    private: //  Data
        
        // SW installer framework
        Usif::RSoftwareInstall iSWInstallerFW;
        // Install parameters
        Usif::COpaqueNamedParams* iSifOptions;
        // Result parameters like error codes.
        Usif::COpaqueNamedParams* iSifResults;
        // SisRegister for native install packages.
        RSisRegistrySession iRegistrySession;
        // File server
        RFs& iFs;
        // Defines need of connection to install server.
        TBool iConnected;
    };
}

#endif /*SILENTUNINSTALLER_H*/
            
// End of File
