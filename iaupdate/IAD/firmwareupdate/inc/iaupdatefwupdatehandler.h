/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
class MIAUpdateFWUpdateObserver;

#include <e32base.h>

NONSHARABLE_CLASS (CIAUpdateFWUpdateHandler) : public CActive
    {
    public:

        IMPORT_C static CIAUpdateFWUpdateHandler* NewLC();

        IMPORT_C static CIAUpdateFWUpdateHandler* NewL();
    
        virtual ~CIAUpdateFWUpdateHandler();

    public:
        
        IMPORT_C void FirmWareUpdatewithFOTA( MIAUpdateFWUpdateObserver* aObserver );
        
        IMPORT_C TBool IsDMSupportAvailableL();

    private:

        CIAUpdateFWUpdateHandler();
   
        void ConstructL();
   
        void RunL();
    
        void DoCancel();

        void ShowDialogL( TDesC& aText, TDesC& aHeading );
        
        void StartSyncL( const TInt aProfileId = KErrNotFound, const TBool aUseFotaProgressNote = EFalse);
    
    private: // data

        CIAUpdateFWFotaModel* iFotaModel;
        CIAUpdateFWSyncAppEngine* iFWSyncAppEngine;
        MIAUpdateFWUpdateObserver* iObserver;
    };
    
    
#endif  //IAUPDATEFWUPDATEHANDLER_H
