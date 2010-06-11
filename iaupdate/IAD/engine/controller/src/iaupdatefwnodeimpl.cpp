/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateFwNode class 
*                member functions.
*
*/



#include <e32cmn.h>
#include <etelmm.h>
#include <ncdnode.h>
#include <ncdnodecontentinfo.h>
#include <ncdpurchasedownloadinfo.h>
#include <ncdutils.h>

#include "iaupdatefwnodeimpl.h"
#include "iaupdatecontrollerimpl.h"
#include "iaupdatefwpurchaseoperation.h"
#include "iaupdatefwnodeobserver.h"
#include "iaupdatenodedetails.h"
#include "iaupdatedebug.h"


// -----------------------------------------------------------------------------
// CIAUpdateFwNode::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
// 
CIAUpdateFwNode* CIAUpdateFwNode::NewLC( 
    MNcdNode* aNode,
    CIAUpdateController& aController,
    MIAUpdateFwNode::TFwUpdateType aFwType )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::NewLC() begin");

    CIAUpdateFwNode* self = 
        new( ELeave ) CIAUpdateFwNode( aController, aFwType );
    CleanupStack::PushL( self );    
    self->ConstructL( aNode );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::NewLC() end");

    return self;
    }

    
// -----------------------------------------------------------------------------
// CIAUpdateFwNode::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//     
CIAUpdateFwNode* CIAUpdateFwNode::NewL( 
    MNcdNode* aNode,
    CIAUpdateController& aController,
    MIAUpdateFwNode::TFwUpdateType aFwType )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::NewL() begin");

    CIAUpdateFwNode* self = 
        CIAUpdateFwNode::NewLC( aNode, aController, aFwType );
    CleanupStack::Pop( self );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::NewL() end");

    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwNode::CIAUpdateFwNode
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateFwNode::CIAUpdateFwNode( CIAUpdateController& aController,
                                  MIAUpdateFwNode::TFwUpdateType aFwType ) 
: CIAUpdateBaseNode( aController ),
  iType( aFwType )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwNode::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateFwNode::ConstructL( MNcdNode* aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::ConstructL() begin");

    // Let the parent handle it all.
    CIAUpdateBaseNode::ConstructL( aNode );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::ConstructL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwNode::~CIAUpdateFwNode
// Destructor
// -----------------------------------------------------------------------------
//    
CIAUpdateFwNode::~CIAUpdateFwNode()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::~CIAUpdateFwNode() begin");

    if ( iPurchaseOperation )
        {
        // Because purchase operation exists, set the cancelling flag on. 
        // Then, observer callbacks are not called when the deletion of 
        // the object cancels the operation. 
        // Also, notice that instead of directly using the error code given 
        // to callbacks, we use the local cancel flag. So, we can handle cases 
        // when operation was able to finish and cancellation did not occur even
        // if it was requested. That would most likely never occur, but better to
        // be safe than sorry,.
        iCancelling = ETrue;
        // Notice, that callback function will delete the iPurchaseOperation.
        // So, do not call delete instead of cancel operation here because then
        // delete would be called twice for the same pointer. Instead, just use 
        // the CancelOperation function call.
        iPurchaseOperation->CancelOperation();
        // There should not be any need to call delete after cancel above. 
        // But, insert it here just in case the operation functionality changes. 
        // It does not hurt to call it here now
        // because pointer should always be NULL already.
        delete iPurchaseOperation;    
        }

    delete iContentUrl;
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::~CIAUpdateFwNode() end");
    }


// ---------------------------------------------------------------------------
// MIAUpdateFwNode functions
// 
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::FwType
// 
// ---------------------------------------------------------------------------
//    
MIAUpdateFwNode::TFwUpdateType CIAUpdateFwNode::FwType() const
    {
    return iType;
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::FwVersion1
// 
// ---------------------------------------------------------------------------
//    
const TDesC& CIAUpdateFwNode::FwVersion1() const
    {
    return Details().FwVersion1();
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::FwVersion2
// 
// ---------------------------------------------------------------------------
//    
const TDesC& CIAUpdateFwNode::FwVersion2() const
    {
    return Details().FwVersion2();
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::FwVersion3
// 
// ---------------------------------------------------------------------------
//    
const TDesC& CIAUpdateFwNode::FwVersion3() const
    {
    return Details().FwVersion3();
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::InitDownloadDataL
// 
// ---------------------------------------------------------------------------
//    
TBool CIAUpdateFwNode::InitDownloadDataL( MIAUpdateFwNodeObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::InitDownloadDataL() begin");
    
    if ( iPurchaseOperation )
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR: KErrInUse, operation already going on.");
        User::Leave( KErrInUse );
        }

    // Create new purchase operation and start it if purchase is necessary.
    IAUPDATE_TRACE("[IAUPDATE] Create new purchase operation");
    CIAUpdateContentOperation* tmpPurchaseOperation(
        CIAUpdateFwPurchaseOperation::NewLC( *this, *this ) );
    TBool operationStarted(
        tmpPurchaseOperation->StartOperationL() );

    if ( operationStarted )
        {
        IAUPDATE_TRACE("[IAUPDATE] Operation started");

        // Ownership will be transferred below to the member variable.
        // So, do not leve the operation into the stack.
        CleanupStack::Pop( tmpPurchaseOperation );

        // Now, that function can not leave, set the member variable values.
        // These values are required when the asynchronous purcahse operation
        // completes and callback functions are called.
        iPurchaseOperation = tmpPurchaseOperation;
        iObserver = &aObserver;        
        }
    else
        {
        // Because operation was not started,
        // delete the operation object.
        CleanupStack::PopAndDestroy( tmpPurchaseOperation );        
        tmpPurchaseOperation = NULL;
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateFwNode::InitDownloadDataL() end: %d",
                     operationStarted);

    return operationStarted;
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::ContentUrl
// 
// ---------------------------------------------------------------------------
//    
const TDesC& CIAUpdateFwNode::ContentUrl() const
    {
    if ( !iContentUrl )
        {
        return KNullDesC();
        }

    return *iContentUrl;
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::NodeType
// 
// ---------------------------------------------------------------------------
//
MIAUpdateAnyNode::TNodeType CIAUpdateFwNode::NodeType() const
    {
    return MIAUpdateAnyNode::ENodeTypeFw;
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::Base
// 
// ---------------------------------------------------------------------------
//
MIAUpdateBaseNode& CIAUpdateFwNode::Base()
    {
    return *this;   
    }


// ---------------------------------------------------------------------------
// MIAUpdateContentOperationObserver functions
// 
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::ContentOperationComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateFwNode::ContentOperationComplete( CIAUpdateBaseNode& /*aNode*/, 
                                                TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateFwNode::ContentOperationComplete() begin: %d",
                     aError);
    
    delete iPurchaseOperation;
    iPurchaseOperation = NULL;

    // Use the temporary variable here for the observer because, in some cases,
    // the observer may delete this object when callback is called and no
    // member variables should be used after that.
    MIAUpdateFwNodeObserver* tmpObserver( iObserver );
    // Do not delete observer because it is not owned here.
    // Just set it to NULL.
    iObserver = NULL;

    if ( !iCancelling )
        {
        IAUPDATE_TRACE("[IAUPDATE] Operation not cancelled.");

        // Because operation was not cancelled, 
        // set the URL if everything went OK.        
        if ( aError == KErrNone )
            {
            // If URL can not be set, then replace KErrNone value of aError
            // with the leave error. So, that value will be given for the observer.
            TRAP( aError, SetContentUrlL() );
            }

        IAUPDATE_TRACE_1("[IAUPDATE] Inform observer: %d", aError);
        tmpObserver->InitDownloadDataComplete( *this, aError );        
        }
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::ContentOperationComplete() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::ContentOperationProgress
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateFwNode::ContentOperationProgress( CIAUpdateBaseNode& /*aNode*/, 
                                                TInt /*aProgress*/, 
                                                TInt /*aMaxProgress*/ )
    {
    
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwNode functions
// 
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateFwNode::SetContentUrlL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateFwNode::SetContentUrlL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::SetContentUrlL() begin");

    // Before trying to set the new values for content URL,
    // delete old values.
    delete iContentUrl;
    iContentUrl = NULL;

    CNcdPurchaseDetails* details( PurchaseDetailsLC() );

    TArray< MNcdPurchaseDownloadInfo* > downloadInfos( 
        details->DownloadInfoL() );
    if ( downloadInfos.Count() == 0 )
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR: KErrNotFound, no download info");
        User::Leave( KErrNotFound );
        }
    else
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Download info count: %d", 
                         downloadInfos.Count());
        // Always use the first download info even if there exists
        // multiple of them.
        MNcdPurchaseDownloadInfo* info( downloadInfos[ 0 ] );
        // Get the content uri and insert it to the member variable.
        iContentUrl = info->ContentUri().AllocL();
        IAUPDATE_TRACE_1("[IAUPDATE] Content uri set: %S",
                         iContentUrl); 
        }    

    CleanupStack::PopAndDestroy( details );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::SetContentUrlL() end");
    }

// ---------------------------------------------------------------------------
// CIAUpdateFwNode::SetNameL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateFwNode::SetNameL( const MNcdNodeMetadata& /*aMetaData*/ )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::SetNameL() begin");
    RTelServer telServer;
    CleanupClosePushL( telServer );
    User::LeaveIfError( telServer.Connect() );
    RTelServer::TPhoneInfo teleinfo;
    User::LeaveIfError( telServer.GetPhoneInfo( 0, teleinfo ) );
    RMobilePhone phone;
    CleanupClosePushL( phone );
    User::LeaveIfError( phone.Open( telServer, teleinfo.iName ) );
    User::LeaveIfError(phone.Initialise()); 
    TUint32 teleidentityCaps;
    phone.GetIdentityCaps( teleidentityCaps );
    RMobilePhone::TMobilePhoneIdentityV1 telid;
    TRequestStatus status;
    phone.GetPhoneId( status, telid );
    User::WaitForRequest( status );
    delete iName;
    iName = NULL;
    if ( status==KErrNone )
        {
        iName = telid.iModel.AllocL(); 
        }
    else
        {
        iName = KNullDesC().AllocL();
        }
    CleanupStack::PopAndDestroy( &phone );
    CleanupStack::PopAndDestroy( &telServer );
     
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwNode::SetNameL() end");
    }
