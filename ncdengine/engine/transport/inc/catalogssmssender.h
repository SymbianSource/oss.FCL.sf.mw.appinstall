/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CATALOGSSMSSENDER_H
#define C_CATALOGSSMSSENDER_H

#include <e32base.h>
#include <msvapi.h> // MMsvSessionObserver


class CCatalogsSmsOperation;
class CMsvSession;
class CClientMtmRegistry;
class CSmsClientMtm;

/**
* SMS sender class
*/
class CCatalogsSmsSender : public CActive, public MMsvSessionObserver
    {
    public:

        static CCatalogsSmsSender* NewL( CCatalogsSmsOperation& aObserver );
    
        ~CCatalogsSmsSender();
    
    public:
        /**
        * Start the operation
        *
        * @param aRecipient Message recipient
        * @param aBody Message body
        */  
        void StartL( const TDesC& aRecipient, const TDesC& aBody );
        
        /**
        * Cancels the operation and removes any created messages 
        */
         void CancelOperation();
    
    protected: // From CActive
    
        void RunL();
        
        void DoCancel();
    
        TInt RunError( TInt aError );
    

    protected: // Constructor
    
        CCatalogsSmsSender( CCatalogsSmsOperation& aObserver );

    private: // from MMsvSessionObserver
     
        void HandleSessionEventL( TMsvSessionEvent aEvent, 
            TAny* aArg1, TAny* aArg2, TAny* aArg3 );
    

    private:
    
        void CreateMessageL();
        void SendMessageL();
        void FinishMessageL();
        void DeleteMessageL();
        void Cleanup();
    
    
    private:
    
        enum TCatalogsSmsState 
            {
            ECatalogsSmsStateIdle = 0,
            ECatalogsSmsStateSessionReady,
            ECatalogsSmsStateStarting,
            ECatalogsSmsStateMoving,
            ECatalogsSmsStateSending,
            ECatalogsSmsStateDeletingSchedule,
            ECatalogsSmsStateDeletingMessage,
            ECatalogsSmsStateDone
            };

    private:
    
        CCatalogsSmsOperation& iObserver;
    
        CMsvSession* iSession; 
        CClientMtmRegistry* iMtmRegistry;
        
        HBufC* iRecipient;
        HBufC* iBody;
        
        // This represents an entry in the Message Server index
        TMsvEntry iMsvEntry; 
        CSmsClientMtm* iSmsMtm;
        CMsvEntry* iParentEntry;
        CMsvOperation* iOp;
        CMsvEntrySelection* iSelection;
        
        TCatalogsSmsState iSmsState;
        TBool iCancelled;
        TMsvId iMovedId;
        TMsvSendState iSendState;
    };

#endif // C_CATALOGSSMSSENDER_H