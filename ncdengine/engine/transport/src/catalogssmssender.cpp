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


#include <smscmds.h>
#include <txtrich.h>
#include <smsclnt.h>
#include <smuthdr.h>
#include <mtclreg.h>

#include "catalogssmsoperationimpl.h"
#include "catalogssmssender.h"
#include "catalogsdebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//
CCatalogsSmsSender* CCatalogsSmsSender::NewL( CCatalogsSmsOperation& aObserver )
    {
    CCatalogsSmsSender* self = new( ELeave ) CCatalogsSmsSender( aObserver );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCatalogsSmsSender::~CCatalogsSmsSender()
    {
    DLTRACEIN(( KNullDesC() ));
    CancelOperation();
    Cleanup();
    DLTRACEOUT(( KNullDesC() ));
    }

// ---------------------------------------------------------------------------
// Starts sending the message
// ---------------------------------------------------------------------------
//        
void CCatalogsSmsSender::StartL( const TDesC& aRecipient, 
    const TDesC& aBody )
    {  
    DLTRACEIN(( KNullDesC() ));

    if( iSmsState != ECatalogsSmsStateIdle &&
        iSmsState != ECatalogsSmsStateSessionReady &&
        iSmsState != ECatalogsSmsStateStarting )
        {
        User::Leave( KErrInUse );
        }

    delete iRecipient;
    iRecipient = 0;
    delete iBody;
    iBody = 0;
    
    // Copy recipient and body
    iRecipient = aRecipient.AllocL();
    iBody = aBody.AllocL();
    
    // Open a session to the message server.
    iSession = CMsvSession::OpenAsyncL( *this );
    iSmsState = ECatalogsSmsStateStarting;    
    }


// ---------------------------------------------------------------------------
// Cancel operation
// ---------------------------------------------------------------------------
//        
void CCatalogsSmsSender::CancelOperation()
    {
    if ( !iCancelled ) 
        {
        
        DLTRACEIN(( KNullDesC() ));
        iCancelled = ETrue; 
        // Cancel the current asynchronous operation if any
        if( iSmsState == ECatalogsSmsStateIdle || 
            iSmsState == ECatalogsSmsStateSessionReady ||
            iSmsState == ECatalogsSmsStateStarting ) 
            {
            Cleanup();
            iObserver.HandleSmsSenderEvent( KErrCancel );
            }
        
        else if( iSmsState == ECatalogsSmsStateMoving || 
            iSmsState == ECatalogsSmsStateSending )
            {        
            if ( iOp ) 
                {
                iOp->Cancel();
                }
            
            TRAPD( err, FinishMessageL() );    
            if ( err != KErrNone ) 
                {
                // Add error handling
                }
                
            DLTRACE( ( _L("FinishMessageL failed: %i"), err ) );
            }
        Cancel();            
        }
    }
    
// ---------------------------------------------------------------------------
// Creates the message
// ---------------------------------------------------------------------------
//            
void CCatalogsSmsSender::CreateMessageL()
    {
    DLTRACEIN(( KNullDesC() ));
    // We get a MtmClientRegistry from our session
    // this registry is used to instantiate new mtms.
    
    // This function should be called only once during the life cycle 
    // of an CCatalogsSmsSender object. Thus there is no need to delete
    // member variables before assigning them values.
    iMtmRegistry = CClientMtmRegistry::NewL( *iSession );
    

    // Create a new message entry.

    DLTRACE( ( _L("Creating message entry") ) );
    // message type is SMS
    iMsvEntry.iMtm = KUidMsgTypeSMS;                         
    // this defines the type of the entry: message
    iMsvEntry.iType = KUidMsvMessageEntry;                   
    // ID of local service ( containing the standard folders )
    iMsvEntry.iServiceId = KMsvLocalServiceIndexEntryId;     
    // set the date of the entry to home time
    iMsvEntry.iDate.HomeTime();                              
    // a flag that this message is in preparation
    iMsvEntry.SetInPreparation( ETrue );                     

    //
    // Create the entry in the Drafts folder.
    //
    CMsvEntry* entry =
        CMsvEntry::NewL( *iSession,
                         KMsvDraftEntryIdValue,
                         TMsvSelectionOrdering() );
    CleanupStack::PushL( entry );

    entry->CreateL( iMsvEntry );
    CleanupStack::PopAndDestroy( entry );

    DLTRACE( ( _L("Creating a new MTM handle") ) );
    // Create a new mtm to handle this message ( 
    // in case our own mtm is in use)
    iSmsMtm =
        static_cast<CSmsClientMtm*>( iMtmRegistry->NewMtmL( iMsvEntry.iMtm ) );
    

    iSmsMtm->SwitchCurrentEntryL( iMsvEntry.Id() );

    
    DLTRACE( ( _L("Set body") ) );
    // We get the message body from Mtm and insert a bodytext
    CRichText& mtmBody = iSmsMtm->Body();
    mtmBody.Reset();

    // Set the body text.
    mtmBody.InsertL( 0, *iBody );

    // Set description equal to the message. Without this 9210 sms browser
    // won't show any preview text when browsing messages.
    iMsvEntry.iDescription.Set( *iBody );


    DLTRACE( ( _L("Set recipient") ) );        
    // set iRecipient into the Details of the entry
    iMsvEntry.iDetails.Set( *iRecipient );  // set recipient info in details
    iMsvEntry.SetInPreparation( EFalse );  // set inPreparation to false

    // set the sending state ( immediately )
    iMsvEntry.SetSendingState( KMsvSendStateWaiting ); 
    
    // set time to Home Time  
    iMsvEntry.iDate.HomeTime();                        

    DLTRACE( ( _L("Set SMS setting") ) );
    iSmsMtm->RestoreServiceAndSettingsL();

    // CSmsHeader encapsulates data specific for sms messages,
    // like service center number and options for sending.
    CSmsHeader& header = iSmsMtm->SmsHeader();
    CSmsSettings* sendOptions = CSmsSettings::NewL();

    CleanupStack::PushL( sendOptions );
    
    // restore existing settings
    sendOptions->CopyL( iSmsMtm->ServiceSettings() ); 

    // set send options
    
    // set to be delivered immediately
    sendOptions->SetDelivery( ESmsDeliveryImmediately );      
    header.SetSmsSettingsL( *sendOptions );
    CleanupStack::PopAndDestroy( sendOptions );

    // let's check if there's sc address
    if ( header.Message().ServiceCenterAddress().Length() == 0 )
        {
        DLTRACE( ( _L("No SC address") ) );
        
        // no, there isn't. We assume there is at least one sc number set 
        // and use the default SC number.
        CSmsSettings* serviceSettings = &( iSmsMtm->ServiceSettings() );

        // if number of scaddresses in the list is null

        if ( serviceSettings->ServiceCenterCount() == 0 )
            {
            // No service center address, query from the user should be here
            // if supported
            DLTRACE( ( _L("Definitely no SC address") ) );
            }
        else
            {
            DLTRACE( ( _L("Using default SC") ) );
            // set sc address to default.
            CSmsNumber* sc = CSmsNumber::NewL();
            CleanupStack::PushL( sc );
            sc->SetAddressL( serviceSettings->GetServiceCenter( 
                serviceSettings->DefaultServiceCenter() ).Address() );
                
            header.Message().SetServiceCenterAddressL( sc->Address() );
            CleanupStack::PopAndDestroy( sc );
            }
        }

    DLTRACE( ( _L("Add recipient") ) );
    // Add our recipient to the list, takes in two TDesCs, 
    // first is real address and second is an alias
    // works also without the alias parameter.
    iSmsMtm->AddAddresseeL( *iRecipient, iMsvEntry.iDetails );

    DLTRACE( ( _L("Save message") ) );
    // save message
    CMsvEntry& newEntry = iSmsMtm->Entry();

    // make sure that we are handling the right entry
    newEntry.ChangeL( iMsvEntry ); 
    iSmsMtm->SaveMessageL();     // closes the message

    DLTRACE( ( _L("Start moving the entry to outbox") ) );
    // This moves the message entry to outbox, we'll schedule it 
    // for sending after this.
    TMsvSelectionOrdering sort;
    sort.SetShowInvisibleEntries( ETrue );

    // Take the current parent.
    iParentEntry =
        CMsvEntry::NewL( *iSession, iMsvEntry.Parent(), sort );


    iSmsState = ECatalogsSmsStateMoving;

    //if ( !iCancelled ) 
        {
        
        iOp = iParentEntry->MoveL( iMsvEntry.Id(), KMsvGlobalOutBoxIndexEntryId, 
            iStatus );

        SetActive();
        DASSERT( iStatus.Int() == KRequestPending );
        }
/*
    else 
        {
        DLTRACE( ( _L("Synch Move") ) );
        // Do some hideous Symbian magic.
        CMsvOperationWait* wait = CMsvOperationWait::NewLC();
        wait->Start();

        iOp = iParentEntry->MoveL( iMsvEntry.Id(), KMsvGlobalOutBoxIndexEntryId, 
            wait->iStatus );
        CleanupStack::PopAndDestroy( wait );
        }
  */  
    }


// ---------------------------------------------------------------------------
// Sends the message
// ---------------------------------------------------------------------------
//        
void CCatalogsSmsSender::SendMessageL()
    {
    DLTRACEIN(( KNullDesC() ));
    
    TMsvLocalOperationProgress prog = McliUtils::GetLocalProgressL( *iOp );
    User::LeaveIfError( prog.iError );

    iMovedId = prog.iId;

    // Delete unnecessary objects
    delete iParentEntry;
    iParentEntry = NULL;
    
    delete iOp;
    iOp = NULL;


    DLTRACE( ( _L("Start sending the SMS") ) );
    // We must create an entry selection for message copies 
    // (although now we only have one message in iSelection )
    iSelection = new (ELeave) CMsvEntrySelection();

    iSelection->AppendL( iMovedId ); // add our message to the iSelection

    TBuf8<1> dummyParams;
    
    iSmsState = ECatalogsSmsStateSending;
    //
    // Initiate the send
    
    //if ( !iCancelled ) 
        {
        
    
        iOp = iSmsMtm->InvokeAsyncFunctionL( ESmsMtmCommandSendScheduledCopy,
                                           *iSelection,
                                           dummyParams,
                                           iStatus );

        SetActive();
    
        iObserver.HandleSmsSenderEvent( ECatalogsSmsSending );
        }
    }


// ---------------------------------------------------------------------------
// Finishes the send operation
// ---------------------------------------------------------------------------
//        
void CCatalogsSmsSender::FinishMessageL()
    {
    
    DLTRACEIN(( KNullDesC() ));
    delete iOp;
    iOp = NULL;
    
    // Check state of the SMS    
    iSendState = KMsvSendStateUnknown;

    // Trap here since switching the entry may fail if SMS has been removed
    TRAPD( err,
        {
        // Check if SMS was successfully sent
        iSmsMtm->SwitchCurrentEntryL( iMovedId );
        } );

    iSmsState = ECatalogsSmsStateDeletingSchedule;
    
    if ( err != KErrNone )
        {

        if ( err == KErrNotFound )
            {
            // SMS has been deleted, probably due to a setting 
            // "Don't keep sent SMS:s"
            // consider this case a successful send and continue
            DLTRACE( ( _L("Couldn't find the SMS anymore") ) );
            iSendState = KMsvSendStateSent;
            }
        
        // Skip active scheduler and just execute the next state
        
        RunL();
        }
    else
        {
        DLTRACE( ( _L("Deleting SMS schedule") ) );
        
        CMsvEntry& cEntry = iSmsMtm->Entry();

        iSendState = TMsvSendState( cEntry.Entry().SendingState() );
        DLINFO(("iSendState: %d", iSendState ));
        
        TBuf8<1> dummyParams;
        DLTRACE( ( _L("InvokeAsyncFunctionL") ) );
        //
        // Delete SMS schedule just in case SMS was not 
        // successfully sent
        //
        if ( !iCancelled ) 
            {            
            iOp = iSmsMtm->InvokeAsyncFunctionL( ESmsMtmCommandDeleteSchedule,
                                               *iSelection,
                                               dummyParams,
                                               iStatus );
            
            DLTRACE( ( _L("Activating schedule delete") ) );
            SetActive();
            }
        else 
            {
            // When cancelling, use synchronous delete
            DLTRACE( ( _L("Sync schedule delete") ) );
            CMsvOperationWait* wait = CMsvOperationWait::NewLC();            
            

            iOp = iSmsMtm->InvokeAsyncFunctionL( ESmsMtmCommandDeleteSchedule,
                                               *iSelection,
                                               dummyParams,
                                               wait->iStatus );
                    
            wait->Start();
            CActiveScheduler::Start();
    
            TUint deleteScheduleResult = cEntry.Entry().SendingState();
            DLTRACE( ( _L( "-> SMS schedule deleted, sendingState: %d" ), 
                             deleteScheduleResult ) );
    
            CleanupStack::PopAndDestroy( wait ); // wait       
            DeleteMessageL();     
            }
        }
    }


// ---------------------------------------------------------------------------
// Deletes the sent message
// ---------------------------------------------------------------------------
//                
void CCatalogsSmsSender::DeleteMessageL()
    {
    DLTRACEIN(( KNullDesC() ));
    CMsvEntry& cEntry = iSmsMtm->Entry();
    TUint deleteScheduleResult = cEntry.Entry().SendingState();

    delete iOp;
    iOp = NULL;

    //
    // Delete the SMS
    //
    TMsvSelectionOrdering sort;
    sort.SetShowInvisibleEntries( ETrue );
    
    DLTRACE( ( _L("Taking the current parent") ) );
    // Take the current parent.
    iParentEntry = CMsvEntry::NewL( *iSession, cEntry.Entry().Parent(), 
        sort );
    

    if ( !iCancelled ) 
        {
        // Normally we delete asynchronously
        DLTRACE( ( _L("Deleting the SMS") ) );
        iOp = iParentEntry->DeleteL( iMsvEntry.Id(), iStatus );

        DLTRACE( ( _L("Activating delete") ) );
        iSmsState = ECatalogsSmsStateDeletingMessage;
        
        SetActive();
        }
    else 
        {
        // When cancelling, delete synchronously
        DLTRACE( ( _L("Sync SMS delete" ) ) );
        // Do some hideous Symbian magic.
        CMsvOperationWait* wait = CMsvOperationWait::NewLC();


        iOp = iParentEntry->DeleteL( iMsvEntry.Id(), wait->iStatus );
        wait->Start();
        CActiveScheduler::Start();
        
        CleanupStack::PopAndDestroy( wait );
        
        // cleanup
        Cleanup();
        
        iSmsState = ECatalogsSmsStateDone;
        // Notify observer about cancellation or success
        if( iCancelled ) 
            {
            iObserver.HandleSmsSenderEvent( KErrCancel );
            }
        else
            {                    
            iObserver.HandleSmsSenderEvent( KErrNone );
            }
        DLTRACE( ( _L("Sync SMS delete end" ) ) );        
        
        }
    }


// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CCatalogsSmsSender::RunL()
    {
    DLTRACEIN(( "State: %d, status: %d", iSmsState, iStatus.Int() ));
    if ( iStatus == KErrCancel ) 
        {
        DLTRACE( ( _L("Cancelling" ) ) );        
        return;
        }
   
    switch( iSmsState )
        {
        case ECatalogsSmsStateSessionReady:
            {
            CreateMessageL();
            break;
            }
                
        case ECatalogsSmsStateMoving:
            {
            SendMessageL();
            break;
            }
            
        case ECatalogsSmsStateSending:
            {
            FinishMessageL();
            break;
            }
            
        case ECatalogsSmsStateDeletingSchedule:
            {
            DeleteMessageL();
            break;
            }
            
        case ECatalogsSmsStateDeletingMessage:
            {
            iSmsState = ECatalogsSmsStateDone;
            // cleanup
            Cleanup();
            
            // Notify observer about cancellation or success
            if( iCancelled ) 
                {
                iObserver.HandleSmsSenderEvent( KErrCancel );
                }
            else if( iSendState != KMsvSendStateSent )
                {
                // For some reason the SMS was not sent
                iObserver.HandleSmsSenderEvent( KErrGeneral );
                }
            else
                {                
                iObserver.HandleSmsSenderEvent( KErrNone );
                }
            
            break;
            }
            
        default:
            {
            }
        
        }
    }


// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CCatalogsSmsSender::DoCancel()
    {
    DLTRACEIN(( KNullDesC() ));
    //CancelOperation();
    }
    

// ---------------------------------------------------------------------------
// Error handler
// ---------------------------------------------------------------------------
//
TInt CCatalogsSmsSender::RunError( TInt aError )
    {
    DLTRACEIN(( KNullDesC() ));
    // Check
    if( aError == KErrNotFound && ( 
        iSmsState == ECatalogsSmsStateDeletingSchedule
        || iSmsState == ECatalogsSmsStateDeletingMessage ) )
        {
        iSendState = KMsvSendStateSent;
        }

    if( iCancelled ) 
        {
        iObserver.HandleSmsSenderEvent( KErrCancel );
        }
    else if( iSendState == KMsvSendStateSent )
    // If the message was sent but some error occurred we still notify
    // of a successful sending    
        {
        iObserver.HandleSmsSenderEvent( KErrNone );
        }
    else 
        {    
        iObserver.HandleSmsSenderEvent( KErrGeneral );
        }
        
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsSmsSender::HandleSessionEventL( TMsvSessionEvent aEvent, 
    TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/ )
    {
    DLTRACEIN( ( _L("Event: %i"), aEvent ) );
    if( iCancelled ) 
        {
        return;
        }
        
    switch( aEvent )
        {
        case EMsvServerReady:
            {
            // Make a fake request and complete it in order to continue
            // in RunL
            iSmsState = ECatalogsSmsStateSessionReady;
            CreateMessageL();
            break;
            }
        
        default:
            {
            }
        
        }
    DLTRACEOUT( ( _L("Event: %i"), aEvent ) );
    }



// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CCatalogsSmsSender::CCatalogsSmsSender( CCatalogsSmsOperation& aObserver ) : 
    CActive( EPriorityStandard ), iObserver( aObserver )
    {    
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// Cleanup
// ---------------------------------------------------------------------------
//	
void CCatalogsSmsSender::Cleanup()
    {
    DLTRACEIN(( KNullDesC() ));
    
    if( iOp && iSmsState != ECatalogsSmsStateDone ) 
        {
        DLTRACE( ( _L("Cancelling current operation") ));
        iOp->Cancel();
        }
    delete iOp;
    iOp = NULL;
    
    delete iParentEntry;
    iParentEntry = NULL;
    
    delete iSelection;
    iSelection = NULL;
    
    delete iSmsMtm;
    iSmsMtm = NULL;
    
    delete iBody;
    iBody = NULL;
    
    delete iRecipient;
    iRecipient = NULL;
    
    delete iMtmRegistry;
    iMtmRegistry = NULL;
    
    delete iSession;
    iSession = NULL;
    DLTRACEOUT(( KNullDesC() ));
    }
