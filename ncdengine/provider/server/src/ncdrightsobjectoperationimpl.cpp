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


#include "ncdrightsobjectoperationimpl.h"

#include <s32mem.h>
#include <apmstd.h>

#include "catalogsbasemessage.h"
#include "catalogshttpincludes.h"

#include "catalogsutils.h"
#include "catalogscontext.h"
#include "ncdproviderdefines.h"
#include "ncddescriptordownloadsuboperation.h"
#include "ncdhttputils.h"
#include "ncdproviderutils.h"
#include "ncddeviceinteractionfactory.h"
#include "ncdinstallationservice.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdRightsObjectOperation* CNcdRightsObjectOperation::NewL( 
    CNcdGeneralManager& aGeneralManager,
    const TDesC& aDownloadUri,
    const TDesC& aMimeType,
    const TNcdConnectionMethod& aMethod,
    MNcdOperationRemoveHandler& aRemoveHandler, 
    MCatalogsHttpSession& aHttpSession,
    MCatalogsSession& aSession )
    {
    CNcdRightsObjectOperation* self = new( ELeave ) CNcdRightsObjectOperation( 
        aGeneralManager, aRemoveHandler, aHttpSession, aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aMethod, aDownloadUri, aMimeType );
    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdRightsObjectOperation::~CNcdRightsObjectOperation()
    {
    DLTRACEIN( ( "" ) );    
    if ( iDownloadOp ) 
        {
        iDownloadOp->Close();
        iDownloadOp = NULL;
        }
    delete iInstallationService;
    delete iMimeType;
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Download config getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpConfig& CNcdRightsObjectOperation::Config()
    {
    DASSERT( iDownloadOp );
    return iDownloadOp->Config();
    }


// ---------------------------------------------------------------------------
// Cancel
// ---------------------------------------------------------------------------
//
void CNcdRightsObjectOperation::Cancel() 
    {    
    DLTRACEIN(( "" ));
    if ( iDownloadOp ) 
        {
        iDownloadOp->Cancel();
        iDownloadOp->Close();
        iDownloadOp = NULL;
        }
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// RunOperation
// ---------------------------------------------------------------------------
//
TInt CNcdRightsObjectOperation::RunOperation()
    {
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));
    DASSERT( iDownloadOp );
    
    if ( !iPendingMessage ) 
        {      
        DLTRACE(("No pending message"));  
        return KErrNotReady;
        }
    
    // Start download.
    DLINFO(("Starting download op"));
    TInt err = iDownloadOp->Start();

    DLTRACEOUT(("err: %d", err));
    return err;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdRightsObjectOperation::CNcdRightsObjectOperation(    
    CNcdGeneralManager& aGeneralManager,
    MNcdOperationRemoveHandler& aRemoveHandler,
    MCatalogsHttpSession& aHttpSession,
    MCatalogsSession& aSession )
    :
    CNcdBaseOperation( aGeneralManager, &aRemoveHandler, ERightsObjectOperation, aSession ), 
    iHttpSession( aHttpSession )        
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdRightsObjectOperation::ConstructL(
    const TNcdConnectionMethod& aMethod,
    const TDesC& aDownloadUri,
    const TDesC& aMimeType )
    {
    DLTRACEIN( ( "" ) );

    // Call ConstructL for the base class
    CNcdBaseOperation::ConstructL();

    iMimeType = ConvertUnicodeToUtf8L( aMimeType );

    iDownloadOp = CNcdDescriptorDownloadSubOperation::NewL( 
        iGeneralManager, iHttpSession, aDownloadUri, *this, iSession );
                
    iGeneralManager.HttpUtils().ConvertConnectionMethod( 
        aMethod, iConnectionMethod );  
    
    iDownloadOp->Config().SetConnectionMethod( iConnectionMethod );

    iInstallationService = 
        NcdDeviceInteractionFactory::CreateInstallationServiceL();

    DLTRACEOUT(( "" ));
    }
    
void CNcdRightsObjectOperation::Progress( CNcdBaseOperation& /*aOperation*/ )
    {
    DLTRACEIN((""));
    }

void CNcdRightsObjectOperation::QueryReceived(
    CNcdBaseOperation& /*aOperation*/, CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN((""));
    }

void CNcdRightsObjectOperation::OperationComplete( 
    CNcdBaseOperation* aOperation, TInt aError )
    {
    DLTRACEIN(("aOperation=%08x, aError=%d", aOperation, aError));
    (void) aOperation; // suppresses compiler warning

    if( aError == KErrNone )
        {
        // Install the DRM object.
        DLINFO(("Installing rights object (%d bytes data, mime type %S)", 
            iDownloadOp->Body().Size(), iMimeType ));
            
        TRAP( aError, iInstallationService->AppendRightsL( 
            iDownloadOp->Body(), 
            TDataType( *iMimeType ) ) );
        }

    DLINFO(("Completing rights object dl&install operation with %d", aError));
    CompleteMessage( iPendingMessage, 
        aError != KErrNone ? 
            ENCDOperationMessageCompletionError : 
            ENCDOperationMessageCompletionComplete, 
        aError );
    }

