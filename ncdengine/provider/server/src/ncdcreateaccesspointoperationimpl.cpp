/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ncdcreateaccesspointoperationimpl.h"

#include <s32mem.h>
#include <apmstd.h>

#include "catalogsbasemessage.h"
#include "catalogsutils.h"
#include "catalogscontext.h"
#include "catalogsaccesspointmanager.h"
#include "ncdproviderdefines.h"
#include "ncdparserfactory.h"
#include "ncdparser.h"

#include "ncd_cp_query.h"
#include "ncd_cp_queryelement.h"
#include "ncd_cp_queryoption.h"
#include "ncd_cp_detail.h"
#include "ncd_cp_clientconfiguration.h"
#include "ncd_cp_error.h"
#include "ncd_cp_serverdetails.h"
#include "ncd_cp_actionrequest.h"
#include "ncd_cp_queryresponseimpl.h"
#include "ncdparserimpl.h"
#include "ncd_parser_cp_clientconfiguration.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdCreateAccessPointOperation* CNcdCreateAccessPointOperation::NewL( 
    HBufC* aAccessPointData,
    MNcdOperationRemoveHandler& aRemoveHandler, 
    CNcdGeneralManager& aGeneralManager,
    MCatalogsSession& aSession )
    {
    CNcdCreateAccessPointOperation* self = 
        new( ELeave ) CNcdCreateAccessPointOperation( 
            aRemoveHandler, 
            aGeneralManager,
            aSession );
        
    CleanupClosePushL( *self );
    self->ConstructL( aAccessPointData );
    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdCreateAccessPointOperation::~CNcdCreateAccessPointOperation()
    {
    DLTRACEIN(( "" ));    
    delete iParser;
    delete iAccessPointData; 
       
    }


// ---------------------------------------------------------------------------
// Cancel
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperation::Cancel() 
    {    
    DLTRACEIN(( "" ));
    if ( iParser ) 
        {
        iParser->CancelParsing();
        }
    iOperationState = EStateCancelled;
    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperation::ParseError( TInt aErrorCode )
    {
    DLTRACEIN(("error:%d", aErrorCode ));
    
    // Handle error only if not handling an error already
    // (cancellation of parsing may cause an unnecessary call to this function).
    if( iError == KErrNone )
        {
        iError = aErrorCode;
           
        if ( iParser )
            {
            iParser->CancelParsing();
            }
                    
        iOperationState = EStateComplete;     
        RunOperation();
        }
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperation::ParseCompleteL( TInt aError )
    {
    DLTRACEIN(("error: %d", aError ));

    if ( aError == KErrNone ) 
        {        
        ParseAccessPointDataL();
        }
    
    delete iParser;
    iParser = NULL;

    iOperationState = EStateComplete; 
    iError = aError;
        
    RunOperation();
    }


// ---------------------------------------------------------------------------
// RunOperation
// ---------------------------------------------------------------------------
//
TInt CNcdCreateAccessPointOperation::RunOperation()
    {
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));    
    
    if ( !iPendingMessage ) 
        {      
        DLTRACE(("No pending message"));  
        return KErrNotReady;
        }
    
    // One trap to rule them all
    TRAPD( err, HandleStateL() );
    
    HandleError( err );
    DLTRACEOUT(("err: %d", err));
    return err;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperation::HandleStateL() 
    {
    DLTRACEIN((""));
    switch( iOperationState ) 
        {
        case EStateRunning:
            {
            StartParsingL();            
            break;
            }
              
              
        case EStateComplete:
            {
            if ( !iError && !iAccessPoint ) 
                {
                DLERROR(("No accesspoint was created, error KErrNotFound"));
                iError = KErrNotFound;
                }
                
            // Returns false if there was no error
            if ( !HandleError( iError ) ) 
                {
                DLTRACE(("Completing the operation"));
                iProgress.iProgress = iAccessPoint;
                CNcdBaseOperation::CompleteMessage( 
                    iPendingMessage,
                    ENCDOperationMessageCompletionComplete, 
                    iProgress,
                    KErrNone );
                }
            break;
            }
        
        default: 
            {
            DLTRACE(("Default"));
            break;
            }
        }    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdCreateAccessPointOperation::HandleError( TInt aError ) 
    {
    DLTRACEIN(("aError: %d", aError ));
    if ( aError != KErrNone ) 
        {
        DLERROR(("Error %d occurred", aError ));
        iError = aError;
        iOperationState = EStateCancelled;
        Cancel();
        if ( iPendingMessage )
            {
            // ignoring error because operation already failed
            CNcdBaseOperation::CompleteMessage( iPendingMessage,
                ENCDOperationMessageCompletionError, iError );
            }
        return ETrue;
        }    
    return EFalse;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperation::StartParsingL()
    {
    DLTRACEIN((""));
    // create parser
    delete iParser;
    iParser = NULL;
    iParser = NcdParserFactory::CreateParserL( NULL );    
        
    MNcdParserObserverBundle& observers = iParser->Observers();
    observers.SetParserObserver( this );            
    
    
    iSubParser = 
        CNcdConfigurationProtocolClientConfigurationParser::NewL( 
            observers, *static_cast<CNcdParserImpl*>( iParser ), 0 );
            
    iParser->BeginAsyncL( iSubParser ); // ownership is transferred
    
    DLTRACE(("Start parsing the data"));
    iParser->ParseL( *iAccessPointData );
    // Inform parser that no more data will be sent
    iParser->EndL();    
    }
    

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperation::ParseAccessPointDataL() 
    {
    DLTRACEIN((""));
    // Ownership is transferred here
    MNcdConfigurationProtocolClientConfiguration* configuration = 
        iSubParser->ClientConfiguration();

    if ( !configuration ) 
        {
        DLTRACEOUT(("No configuration"));
        return;
        }
        
    CleanupDeletePushL( configuration );
    DASSERT( iPendingMessage );
    
    RArray<TUint32> accesspoints;
    CleanupClosePushL( accesspoints );
    // Parse access point data from client configuration response.
    iGeneralManager.AccessPointManager().ParseAccessPointDataFromClientConfL(
        *configuration, 
        iPendingMessage->Session().Context().FamilyId(),
        ETrue,
        &accesspoints );
        
    iAccessPoint = 0;
    
    if ( accesspoints.Count() ) 
        {
        DLTRACE(("%d accesspoints were created", accesspoints.Count() ));
        iAccessPoint = accesspoints[0];
        DLTRACE(("iAccessPoint: %d", iAccessPoint ));
        }
     
    CleanupStack::PopAndDestroy( 2, configuration ); // accesspoints, aConfiguration    
    }
    

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdCreateAccessPointOperation::CNcdCreateAccessPointOperation(
    MNcdOperationRemoveHandler& aRemoveHandler, 
    CNcdGeneralManager& aGeneralManager,
    MCatalogsSession& aSession )
    :
    CNcdBaseOperation( aGeneralManager, &aRemoveHandler, ECreateAccessPointOperation,
        aSession )    
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperation::ConstructL(
    HBufC* aAccessPointData )
    {
    DLTRACEIN( ( "" ) );

    // Call ConstructL for the base class
    CNcdBaseOperation::ConstructL();

    iAccessPointData = aAccessPointData;

    }
