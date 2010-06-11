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
* Description:   This module contains the implementation of CIAUpdateBaseNode class 
*                member functions.
*
*/



#include <ncdprovider.h>
#include <ncdnode.h>
#include <ncdnodemetadata.h>
#include <ncdnodepurchase.h>
#include <ncdnodecontentinfo.h>
#include <ncdpurchasehistory.h>
#include <ncdutils.h>
#include <catalogsutils.h>

#include "iaupdatebasenodeimpl.h"
#include "iaupdatenodedetails.h"
#include "iaupdatecontrollerimpl.h"
#include "iaupdatecontentoperationmanager.h"
#include "iaupdateutils.h"
#include "iaupdatectrlnodeconsts.h"
#include "iaupdateprotocolconsts.h"
#include "iaupdatedebug.h"





// -----------------------------------------------------------------------------
// CIAUpdateBaseNode::CIAUpdateBaseNode
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateBaseNode::CIAUpdateBaseNode( CIAUpdateController& aController ) 
: CBase(),
  iController( aController )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateBaseNode::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateBaseNode::ConstructL( MNcdNode* aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::ConstructL() begin");

    if ( !aNode )
        {
        // Do not accept NULL node.
        User::Leave( KErrArgument );
        }

    // Here we initialize metadata, details and contentinfo.
    // The metadata, its details and content info should remain same even if
    // update related operations are done. So, they can be created here and there
    // is no need to update that information during the lifetime of this node.
    // If the node is updated from the net server, then this whole node should be 
    // recreated. Or, these object should be updated in correct places.

    MNcdNodeMetadata* metaData = aNode->QueryInterfaceLC< MNcdNodeMetadata >();
    if ( metaData )
        {
        // Also update details information from the metadata.
        iMetaNamespace = metaData->Namespace().AllocL();
        iMetaId = metaData->Id().AllocL();
        SetNameL( *metaData );
        iDescription = metaData->Description().AllocL();
        IAUPDATE_TRACE_1("[IAUPDATE] description: %S", iDescription );
        // Notice, that ownership of metadata is not transferred here.
        iDetails = CIAUpdateNodeDetails::NewL( metaData );
        CleanupStack::PopAndDestroy( metaData );
        metaData = NULL;        
        }
    else
        {
        // Just leave, because important information is missing.
        // We should always have metadata when this node is created.
        User::Leave( KErrNotFound );        
        }

    // Get information from content info.
    // These we have to set here because we can not just pass this information
    // as it is given from the engine and we also take the ownership of the final
    // result.    
    MNcdNodeContentInfo* contentInfo = aNode->QueryInterfaceLC< MNcdNodeContentInfo >();
    if ( contentInfo )
        {
        iMime = contentInfo->MimeType().AllocL();
        iVersion.InternalizeL( contentInfo->Version() );

        //Initialize iUid or iIdentifier according to the mime-type
        iUid = KNullUid;

        if ( iMime->Compare( IAUpdateProtocolConsts::KMimeWidget ) == 0 )
            {
            iIdentifier = contentInfo->Identifier().AllocL();
            }
        else
           {
           iUid = contentInfo->Uid();
           iIdentifier = KNullDesC().AllocL();
           }

        // Release content info.
        // So, we can later check if it still exists and then get 
        // new one if it exists.
        CleanupStack::PopAndDestroy( contentInfo );
        contentInfo = NULL;
        }
    else
        {
        // Just leave, because important information is missing like
        // content UID.
        User::Leave( KErrNotFound );
        }

    // Not, that this function can not leave any more it is safe to take the
    // ownership of the node. Now, the user that called this function can safely
    // pop the node from the cleanup stack.
    iNode = aNode;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::ConstructL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateBaseNode::~CIAUpdateBaseNode
// Destructor
// -----------------------------------------------------------------------------
//    
CIAUpdateBaseNode::~CIAUpdateBaseNode()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::~CIAUpdateBaseNode() begin");

    if ( iNode )
        {
    	iNode->Release();
        }

    delete iMetaNamespace;    
    delete iMetaId;
    delete iName;    
    delete iDescription;
    delete iMime;
	delete iIdentifier;
    delete iDetails;
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::~CIAUpdateBaseNode() end");
    }



// ---------------------------------------------------------------------------
// MIAUpdateNode functions
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::MetaNamespace
// 
// ---------------------------------------------------------------------------
//
const TDesC& CIAUpdateBaseNode::MetaNamespace() const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::MetaNamespace() = %S", 
                     iMetaNamespace);
    return *iMetaNamespace;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::MetaId
// 
// ---------------------------------------------------------------------------
//
const TDesC& CIAUpdateBaseNode::MetaId() const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::MetaId() = %S", iMetaId);
    return *iMetaId;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Uid
// 
// ---------------------------------------------------------------------------
//
const TUid& CIAUpdateBaseNode::Uid() const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::Uid() = %x", iUid);
    return iUid;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Identifier
// 
// ---------------------------------------------------------------------------
//
const TDesC& CIAUpdateBaseNode::Identifier() const
    {
    return *iIdentifier;
    }
    
// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Name
// 
// ---------------------------------------------------------------------------
//
const TDesC &CIAUpdateBaseNode::Name() const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::Name() = %S", iName);
    return *iName;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Description
// 
// ---------------------------------------------------------------------------
//
const TDesC& CIAUpdateBaseNode::Description() const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::Description() = %S", 
                     iDescription);
    return *iDescription;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::ContentSize
// 
// ---------------------------------------------------------------------------
//
TInt CIAUpdateBaseNode::ContentSizeL() const
    {
    // Notice, here we give the content size through the interface.
    // See OwnContentSizeL for the size that has been gotten from the server.
    // It will be given as a default, if child classes do not implement this.
    return OwnContentSizeL();
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Version
// 
// ---------------------------------------------------------------------------
//
const TIAUpdateVersion& CIAUpdateBaseNode::Version() const
    {
    return iVersion;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Importance
// 
// ---------------------------------------------------------------------------
//    
MIAUpdateBaseNode::TImportance CIAUpdateBaseNode::Importance() const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::Importance() = %d", 
                     Details().Importance());
    return Details().Importance();
    }

void CIAUpdateBaseNode::SetImportance( MIAUpdateBaseNode::TImportance aImportance )
    {
    Details().SetImportance( aImportance );
    }
// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::SearchCriteria
// 
// ---------------------------------------------------------------------------
//
const TDesC& CIAUpdateBaseNode::SearchCriteria() const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::SearchCriteria() = %S", 
                     &Details().SearchCriteria());
    return Details().SearchCriteria();
    }

// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::RebootAfterInstall
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateBaseNode::RebootAfterInstall() const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::RebootAfterInstall() = %d", 
                         Details().RebootAfterInstall());
    return Details().RebootAfterInstall();
    }

// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Hidden
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateBaseNode::Hidden() const
    {
    TBool hidden( EFalse );
    if ( iForcedHidden
         || Details().Importance() == MIAUpdateBaseNode::EHidden )
        {
        hidden = ETrue;
        }
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::Hidden() = %d", hidden);
    return hidden;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::SetSelected
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateBaseNode::SetSelected( TBool aSelected )
    {
    iSelected = aSelected;
    }

    
// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::IsSelected
// 
// ---------------------------------------------------------------------------
//
    
TBool CIAUpdateBaseNode::IsSelected() const
    {
    return iSelected;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::LastUpdateErrorCodeL()
// 
// ---------------------------------------------------------------------------
// 
TInt CIAUpdateBaseNode::LastUpdateErrorCodeL() const
    {
    return CIAUpdateContentOperationManager::CheckErrorCode( 
            LastUpdateErrorCodeFromPurchaseHistoryL() );
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::SetIdleCancelToPurchaseHistoryL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateBaseNode::SetIdleCancelToPurchaseHistoryL( 
    TBool aForceVisibleInHistory )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::SetIdleCancelToPurchaseHistoryL() begin");

    SetIdleErrorToPurchaseHistoryL( KErrCancel, aForceVisibleInHistory );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::SetIdleCancelToPurchaseHistoryL() end");  
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::SetInstallStatusToPurchaseHistoryL
// 
// ---------------------------------------------------------------------------
//    
void CIAUpdateBaseNode::SetInstallStatusToPurchaseHistoryL( 
    TInt aErrorCode, TBool aForceVisibleInHistory )
    {
    IAUPDATE_TRACE_2("[IAUPDATE] CIAUpdateBaseNode::SetInstallStatusToPurchaseHistoryL(): %d, %d",
                     aErrorCode, aForceVisibleInHistory);
    (void)aErrorCode;
    (void)aForceVisibleInHistory;
    // Nothing to do here because install status is automatically set into the
    // purchase history when normal nodes are used. 
    // Child classes may have their own implementation here.
    return;
    }

// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Mime
// 
// ---------------------------------------------------------------------------
// 
const TDesC& CIAUpdateBaseNode::Mime() const
    {
    return *iMime;
    }



// ---------------------------------------------------------------------------
// Public functions
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::SetIdleErrorToPurchaseHistoryL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateBaseNode::SetIdleErrorToPurchaseHistoryL( 
    TInt aError, TBool aForceVisibleInHistory )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::SetIdleErrorToPurchaseHistoryL() begin");

    // Update the current node information to the purchase history.

    // Create a new purchase detail.
    CNcdPurchaseDetails* details( PurchaseDetailsLC() );

    // Set the current universal time. 
    // So, that item is the newest in the purhcase history list.
    details->SetLastUniversalOperationTime();

    // The operation was cancelled.
    details->SetLastOperationErrorCode( aError );

    // Because item should be forced as visible in history,
    // remove the hidden postfix if necessary.
    if ( aForceVisibleInHistory )
        {
        IAUPDATE_TRACE("[IAUPDATE] Change purchase history MIME to visible");
        TInt postfixIndex(
            iMime->Match( IAUpdateCtrlNodeConsts::KMimeHiddenPostfixPattern() ) );
        if ( postfixIndex != KErrNotFound )
            {
            IAUPDATE_TRACE("[IAUPDATE] Hidden postfix found. Omit it.");
            details->SetAttributeL( 
                MNcdPurchaseDetails::EPurchaseAttributeContentMimeType, 
                    iMime->Left( postfixIndex ) );            
            }
        }

    // Get the purchase history and save the purchase detail information there.
    MNcdPurchaseHistory* history( iController.ProviderL().PurchaseHistoryL() );
    CleanupReleasePushL( *history );
    
    history->SavePurchaseL( *details );
    
    CleanupStack::PopAndDestroy( history );
    
    CleanupStack::PopAndDestroy( details );    

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::SetIdleErrorToPurchaseHistoryL() end");      
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Equals
// 
// ---------------------------------------------------------------------------
// 
TBool CIAUpdateBaseNode::Equals( const CIAUpdateBaseNode& aNode ) const
    {
    TBool equals = EFalse;
    if ( iMime->Compare( IAUpdateProtocolConsts::KMimeWidget ) == 0 )
        {
        if ( aNode.MetaNamespace() == MetaNamespace()
                && aNode.MetaId() == MetaId()
             || ( aNode.Identifier() == Identifier() 
                && aNode.Version() == Version() ) )
            {
            equals = ETrue;
            }
        }
    else
        {
        if ( aNode.MetaNamespace() == MetaNamespace()
                && aNode.MetaId() == MetaId()
             || ( aNode.Uid() == Uid()
                && aNode.Version() == Version() 
                && aNode.iMime->Match( IAUpdateProtocolConsts::KMimeServicePackPattern()) == KErrNotFound ) ) 
            {
            equals = ETrue;
            }
        }
    return equals;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::ForceHidden
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateBaseNode::ForceHidden( TBool aHidden )
    {
    iForcedHidden = aHidden;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::OwnContentSizeL()
// 
// ---------------------------------------------------------------------------
//     
TInt CIAUpdateBaseNode::OwnContentSizeL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::OwnContentSizeL() begin");

    // Set the default size to zero.
    // We should always have content info and get the correct value 
    // from there. But, if content info is not available, then
    // zero is only thing we can give here.
    TInt retSize( 0 );

    // Get information from content info.
    MNcdNodeContentInfo* contentInfo( 
        iNode->QueryInterfaceL< MNcdNodeContentInfo >() );
    if ( contentInfo )
        {
        IAUPDATE_TRACE("[IAUPDATE] Content info interface found");
        // Content info existed. So, get the size info.
        retSize = contentInfo->Size();
        // Release content info.
        // So, we can later check if it still exists and then get 
        // new one if it exists.
        contentInfo->Release();
        contentInfo = NULL;
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::OwnContentSizeL() end: %d",
                     retSize);

    return retSize;
    }

 
// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Details
// 
// ---------------------------------------------------------------------------
//
CIAUpdateNodeDetails& CIAUpdateBaseNode::Details() const
    {
    return *iDetails;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Controller
// 
// ---------------------------------------------------------------------------
//
CIAUpdateController& CIAUpdateBaseNode::Controller() const
    {
    return iController;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::Node
// 
// ---------------------------------------------------------------------------
//
MNcdNode& CIAUpdateBaseNode::Node() const
    {
    return *iNode;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::LastUpdateErrorCodeFromPurchaseHistoryL
// 
// ---------------------------------------------------------------------------
//
TInt CIAUpdateBaseNode::LastUpdateErrorCodeFromPurchaseHistoryL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::LastUpdateErrorCodeFromPurchaseHistoryL() begin");
    
    TInt errorCode( KErrNone );
    
    // Get the error code of the last operation.
    // Purchase history provides a good place to get this.

    // Create filter. So, we will get
    // all the purchase history items.
    CNcdPurchaseHistoryFilter* filter =
        CNcdPurchaseHistoryFilter::NewLC();
    filter->SetNamespaceL( MetaNamespace() );
    filter->SetEntityIdL( MetaId() );

    // Add family uid to the filter
    RArray< TUid > uids;
    CleanupClosePushL( uids );
    uids.AppendL( iController.FamilyUid() );
    filter->SetClientUids( uids.Array() );
    CleanupStack::PopAndDestroy( &uids );

    MNcdPurchaseHistory* history( iController.ProviderL().PurchaseHistoryL() );
    CleanupReleasePushL( *history );
    
    // Get the ids. So, we can next get all the corresponding
    // details.
    RArray< TUint > ids = history->PurchaseIdsL( *filter );
    // Temporarily remove history from cleanup stack
    CleanupStack::Pop( history );
    CleanupStack::PopAndDestroy( filter );
    CleanupReleasePushL( *history );
    CleanupClosePushL( ids );
    
    if ( ids.Count() > 0 )
        {
        CNcdPurchaseDetails* details( 
            history->PurchaseDetailsL( ids[ 0 ] , EFalse ) );
        if ( details )
            {
            errorCode = details->LastOperationErrorCode();    
            delete details;
            details = NULL;
            }
        }
    
    CleanupStack::PopAndDestroy( &ids );
    CleanupStack::PopAndDestroy( history );
    
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", errorCode );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::LastUpdateErrorCodeFromPurchaseHistoryL() end");

    return errorCode;    
    }


// ---------------------------------------------------------------------------
// Protected functions
// 
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::PurchaseDetailsLC
// 
// ---------------------------------------------------------------------------
//
CNcdPurchaseDetails* CIAUpdateBaseNode::PurchaseDetailsLC()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::IdleCancelPurchaseDetailsLC begin");

    // This will contain the correct details object.
    CNcdPurchaseDetails* details( NULL );

    // Create filter. So, we will get
    // all the purchase history items.
    CNcdPurchaseHistoryFilter* filter =
        CNcdPurchaseHistoryFilter::NewLC();
    filter->SetNamespaceL( MetaNamespace() );
    filter->SetEntityIdL( MetaId() );
    
    // Add family uid to the filter
    RArray< TUid > uids;
    CleanupClosePushL( uids );
    uids.AppendL( iController.FamilyUid() );
    filter->SetClientUids( uids.Array() );
    CleanupStack::PopAndDestroy( &uids );

    MNcdPurchaseHistory* history( iController.ProviderL().PurchaseHistoryL() );
    CleanupReleasePushL( *history );
    
    // Get the ids. So, we can next get all the corresponding
    // details.
    RArray< TUint > ids = history->PurchaseIdsL( *filter );
    // Temporarily remove history from cleanup stack
    CleanupStack::Pop( history );
    CleanupStack::PopAndDestroy( filter );
    CleanupReleasePushL( *history );
    CleanupClosePushL( ids );
    
    if ( ids.Count() > 0 )
        {
        // If purchase details exist, then use the most up-to-date one.
        // This may be a previous idle cancel or if the update flow has
        // proceeded for this node, then this is a real purchase detail.
        // In either case, this details will be updated with the cancel
        // error code and new time information.
        details = 
            history->PurchaseDetailsL( ids[ 0 ], EFalse );
        }
    
    CleanupStack::PopAndDestroy( &ids );
    CleanupStack::PopAndDestroy( history );
    
    if ( !details )
        {
        // There was not corresponding purchase details in the history.
        // So, create new.
        details = CNcdPurchaseDetails::NewLC();

        // Set the known details into the newly created object.
        
        // Set identifier information here.
        details->SetNamespaceL( MetaNamespace() );
        details->SetEntityIdL( MetaId() );
        // The client uid means here the family uid.
        details->SetClientUid( iController.FamilyUid() );
            
        // This information we already have available in this class.
        details->SetItemNameL( Name() );
        details->SetDescriptionL( Description() );
        details->SetVersionL( Version().Name() );

        // Notice, let the purchase time be zero because the purchase operation
        // has not been done to the node content yet.
        const TInt KDefaultPurchaseTime( 0 );
        details->SetPurchaseTime( KDefaultPurchaseTime );
        }
    else
        {
        // The details was created but not inserted into
        // the cleanup stack above. Insert it into the cleanupstack now.
        CleanupStack::PushL( details );
        }

    // MIME type checking.
    // Make sure that the purchase history also contains the correct MIME.
    // In certain cases the MIME type depends on the node state or settings
    // and the purchase history may contain old value that has been gotten
    // from the server.
    // By setting it here, we can be sure that if the purchase details is
    // later saved into the history, the correct value will then be used.
    details->SetAttributeL( 
        MNcdPurchaseDetails::EPurchaseAttributeContentMimeType, MimeL() );


    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::IdleCancelPurchaseDetailsLC end");

    return details;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::MimeL
// 
// ---------------------------------------------------------------------------
//
const TDesC& CIAUpdateBaseNode::MimeL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::MimeL begin");

    TInt postfixIndex(
        iMime->Match( IAUpdateCtrlNodeConsts::KMimeHiddenPostfixPattern() ) );

	IAUPDATE_TRACE_1("[IAUPDATE] postfixIndex: %d", postfixIndex);

    if ( Hidden() )
        {
    	IAUPDATE_TRACE("Set as hidden");

        // Node should be set as hidden.
        if ( postfixIndex == KErrNotFound )
            {
        	IAUPDATE_TRACE("[IAUPDATE] Was not hidden");
            // The node is not already set as hidden.
            // So, set it now.
            // Expand the buffer size, so hidden postfix can be added.
            iMime = 
                iMime->ReAllocL(
                    iMime->Length()
                    + IAUpdateCtrlNodeConsts::KMimeHiddenPostfix().Length() );

            // Append the postfix into the mime variable.
            TPtr ptrMime( iMime->Des() );
            ptrMime.Append( IAUpdateCtrlNodeConsts::KMimeHiddenPostfix() );
            }        
        }
    else
        {
    	IAUPDATE_TRACE("Unset hidden");

        // Node should be set as not a hidden.
        if ( postfixIndex != KErrNotFound )
            {
            IAUPDATE_TRACE("[IAUPDATE] Was hidden");
            // The node was actually hidden.
            // So, set the correct new value now.
            // Notice, that this sets the value into the purchase history and into
            // the member variable.
            HBufC* mime( iMime->Left( postfixIndex ).AllocL() );
            delete iMime;
            iMime = mime;
            }
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateBaseNode::MimeL end: %S", 
                     iMime);

    return *iMime;
    }

// ---------------------------------------------------------------------------
// CIAUpdateBaseNode::SetNameL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateBaseNode::SetNameL( const MNcdNodeMetadata& aMetaData )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::SetNameL begin");
    delete iName;
    iName = NULL;
    iName = aMetaData.Name().AllocL();
    IAUPDATE_TRACE_1("[IAUPDATE] name: %S", iName );
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateBaseNode::SetNameL end");
    }

