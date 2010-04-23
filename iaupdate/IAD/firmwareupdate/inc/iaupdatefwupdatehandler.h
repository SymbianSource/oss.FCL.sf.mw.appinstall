/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef IAUPDATEFWUPDATEHANDLER_H
#define IAUPDATEFWUPDATEHANDLER_H

class CIAUpdateFWFotaModel;
class CIAUpdateFWSyncAppEngine;

#include <e32base.h>


class CIAUpdateFWUpdateHandler : public CActive
    {
    public:

        IMPORT_C static CIAUpdateFWUpdateHandler* NewLC();

        IMPORT_C static CIAUpdateFWUpdateHandler* NewL();
    
        virtual ~CIAUpdateFWUpdateHandler();

    public:
      
        IMPORT_C void FirmWareUpdatewithNSU();
        
        IMPORT_C void FirmWareUpdatewithFOTA();
        
        IMPORT_C TBool IsDMSupportAvailableL();

    private:
    
        enum TRequest
            {
            EFOTA =1,
            ENSU
            };
 
        CIAUpdateFWUpdateHandler();
   
        void ConstructL();
   
        void RunL();
    
        void DoCancel();

        void ShowDialogL( TDesC& aText, TDesC& aHeading );
        
        void StartSyncL( const TInt aProfileId = KErrNotFound, const TBool aUseFotaProgressNote = EFalse);
    
    private: // data

        TRequest iRequest;
        CIAUpdateFWFotaModel* iFotaModel;
        CIAUpdateFWSyncAppEngine* iDMEngine;
    };
    
    
#endif  //IAUPDATEFWUPDATEHANDLER_H
