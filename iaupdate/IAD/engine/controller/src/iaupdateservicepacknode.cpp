/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#include <ncdnode.h>
#include <ncdnodeinstall.h>

#include "iaupdateservicepacknode.h"
#include "iaupdatecontrollerimpl.h"
#include "iaupdatecontentoperationmanager.h"
#include "iaupdatenodedetails.h"
#include "iaupdatenodedependencyimpl.h"
#include "iaupdatectrlnodeconsts.h"
#include "iaupdateutils.h"

#include "iaupdatedebug.h"


// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::NewLC
// 
// ---------------------------------------------------------------------------
//
CIAUpdateServicePackNode* CIAUpdateServicePackNode::NewLC( 
    MNcdNode* aNode,
    CIAUpdateController& aController )
    {
    CIAUpdateServicePackNode* self( 
        new( ELeave ) CIAUpdateServicePackNode( aController ) );
    CleanupStack::PushL( self );    
    self->ConstructL( aNode );
    return self;
    }
  
    
// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::NewL
// 
// ---------------------------------------------------------------------------
//    
CIAUpdateServicePackNode* CIAUpdateServicePackNode::NewL( 
    MNcdNode* aNode,
    CIAUpdateController& aController )
    {
    CIAUpdateServicePackNode* self(
        CIAUpdateServicePackNode::NewLC( aNode, aController ) );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::CIAUpdateServicePackNode
// 
// ---------------------------------------------------------------------------
//
CIAUpdateServicePackNode::CIAUpdateServicePackNode( 
    CIAUpdateController& aController ) 
: CIAUpdateNode( aController )
    {
    }


// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateServicePackNode::ConstructL( MNcdNode* aNode )
    {
    CIAUpdateNode::ConstructL( aNode );
    }

    
// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::~CIAUpdateServicePackNode
// 
// ---------------------------------------------------------------------------
//
CIAUpdateServicePackNode::~CIAUpdateServicePackNode()
    {
    }


// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::SetInstallStatusToPurchaseHistoryL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateServicePackNode::SetInstallStatusToPurchaseHistoryL( 
    TInt aErrorCode, TBool aForceVisibleInHistory )
    {
    IAUPDATE_TRACE_2("[IAUPDATE] CIAUpdateServicePackNode::SetInstallStatusToPurchaseHistoryL() begin: %d, %d",
                     aErrorCode, aForceVisibleInHistory);

    // Make sure that error code does not contain any base error value.
    TInt errorCode( 
        CIAUpdateContentOperationManager::CheckErrorCode( 
            aErrorCode ) );

    // Now update the error code with the correct base error value.
    // This base value will be used later to know that the last operation has been
    // install operation. This is service pack specific .
    errorCode += IAUpdateCtrlNodeConsts::KErrBaseServicePackInstall;

    IAUPDATE_TRACE_1("[IAUPDATE] Error code: %d", errorCode);

    SetIdleErrorToPurchaseHistoryL( 
        errorCode, aForceVisibleInHistory );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateServicePackNode::SetInstallStatusToPurchaseHistoryL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::SetIdleErrorToPurchaseHistoryL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateServicePackNode::SetIdleErrorToPurchaseHistoryL( 
        TInt aError, TBool aForceVisibleInHistory )
    {
    IAUPDATE_TRACE_2("[IAUPDATE] CIAUpdateServicePackNode::SetIdleErrorToPurchaseHistoryL() begin: %d, %d",
                     aError, aForceVisibleInHistory);

    TInt newError( aError );

    TInt decodedError( 
        CIAUpdateContentOperationManager::CheckErrorCode( aError ) );

    // If aError already contains the base value, then the given value
    // can be directly saved into the purchase history.
    if ( newError == decodedError )
        {
        // Because error values were same, it means that aError did not contain
        // any base value. So, check if the base value should be included.
        
        // Get the last operation error code from the purchase history.
        TInt lastErrorCode( 
            LastUpdateErrorCodeFromPurchaseHistoryL() );

        // Get the decoded value of the error code.
        // So, it will not contain any base value.
        TInt decodedHistoryError(
            CIAUpdateContentOperationManager::CheckErrorCode( 
                lastErrorCode ) );

        // By subtracting the decoded error from the last purchase history error,
        // we get the error base value. Then, we can add the newly given error value
        // to the base value. Also, newly given error value is decoded before using it.
        newError =
            lastErrorCode - decodedHistoryError 
            + decodedError;
        }

    CIAUpdateNode::SetIdleErrorToPurchaseHistoryL(
        newError, aForceVisibleInHistory );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateServicePackNode::SetIdleErrorToPurchaseHistoryL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::Type
// 
// ---------------------------------------------------------------------------
//
MIAUpdateNode::TPackageType CIAUpdateServicePackNode::Type() const
    {
    return MIAUpdateNode::EPackageTypeServicePack;
    }


// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::IsPurchasedL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateServicePackNode::IsPurchasedL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateServicePackNode::IsPurchasedL()");

    // Notice, this function should not be called before dependency chains
    // are created.
    
    // Notice, that service pack itself is never purchased but its dependency
    // items are. Here we trust that the node dependency chains have already
    // been created. So, we use the dependency nodes to check if this
    // service pack can be thought as purchased. Also, this function trusts
    // that while dependency chains were created also loops were removed.

    TBool purchased( ETrue );

    RPointerArray< CIAUpdateNode > dependencies;
    CleanupClosePushL( dependencies );

    // Accept hidden and visible nodes here.
    // So, if IAD is shown in the UI but it depends on the hidden
    // self updater, the self updater is still included inside the
    // service pack.
    CIAUpdateContentOperationManager::GetOperationNodesL(
        *this, dependencies, ETrue, ETrue );

    for ( TInt i = 0; i < dependencies.Count(); ++i )
        {
        CIAUpdateNode& node( *dependencies[ i ] );
        // Notice, that if the dependency node is a service pack itself
        // then its dependencies are checked like its done in here.
        // Notice! Here we skip self update nodes because at the moment
        // IAD Engine does not support self updates inside service packs.
        // Self update nodes are always shown outside of service pack in 
        // the UI.
        // Because we have the whole hierarchy already in dependencies array,
        // do not call IsPurchased function for other sub service packs.
        if ( MIAUpdateNode::EPackageTypeServicePack != node.Type() 
             && !node.IsSelfUpdate()
             && !node.IsPurchased() )
            {
            IAUPDATE_TRACE("[IAUPDATE] Item not downloaded."); 
            // At least one dependency node was not purchased.
            // So, think service pack as not purchased also.
            purchased = EFalse;
            break;
            }
        }

    CleanupStack::PopAndDestroy( &dependencies );    

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateServicePackNode::IsPurchasedL() end: %d",
                     purchased);

    return purchased;
    }


// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::IsDownloadedL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateServicePackNode::IsDownloadedL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateServicePackNode::IsDownloadedL() begin");

    // Notice, this function should not be called before dependency chains
    // are created.

    // Notice, that service pack itself is never downloaded but its dependency
    // items are. Here we trust that the node dependency chains have already
    // been created. So, we use the dependency nodes to check if this
    // service pack can be thought as downloaded. Also, this function trusts
    // that while dependency chains were created also loops were removed.

    TBool downloaded( ETrue );

    RPointerArray< CIAUpdateNode > dependencies;
    CleanupClosePushL( dependencies );

    // Accept hidden and visible nodes here.
    // So, if IAD is shown in the UI but it depends on the hidden
    // self updater, the self updater is still included inside the
    // service pack.
    CIAUpdateContentOperationManager::GetOperationNodesL(
        *this, dependencies, ETrue, ETrue );

    for ( TInt i = 0; i < dependencies.Count(); ++i )
        {
        CIAUpdateNode& node( *dependencies[ i ] );
        // Notice! Here we skip self update nodes because at the moment
        // IAD Engine does not support self updates inside service packs.
        // Self update nodes are always shown outside of service pack in 
        // the UI.
        // Because we have the whole hierarchy already in dependencies array,
        // do not call IsDownloaded function for other sub service packs.
        if ( MIAUpdateNode::EPackageTypeServicePack != node.Type() 
             && !node.IsSelfUpdate()
             && !node.IsDownloaded() )
            {
            IAUPDATE_TRACE("[IAUPDATE] Item not downloaded."); 
            // At least one dependency node was not downloaded.
            // So, think service pack as not downloaded also.
            downloaded = EFalse;
            break;
            }
        }

    CleanupStack::PopAndDestroy( &dependencies );    

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateServicePackNode::IsDownloadedL() end: %d",
                     downloaded);

    return downloaded;
    }


// ---------------------------------------------------------------------------
// CIAUpdateServicePackNode::IsInstalledL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateServicePackNode::IsInstalledL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateServicePackNode::IsInstalledL() begin");

    // Notice, this function should not be called before dependency chains
    // are created.

    // Notice, that service pack itself is never installed but its dependency
    // items are. While operations are done, the state and the error code
    // will be updated for the service pack in purchase history.
    // With this implementation if the service pack is successfully installed
    // and after that some of its items are uninstalled, the service pack will
    // still be shown in the UI instead of the items.
    // Notice, that here we suppose that if some dependency node is uninstalled,
    // then all its dependants are also been uninstalled by the installer. Therefore,
    // this information will be known also in head service pack, when installation
    // status is checked. 
    // Notice, here we trust that while dependency chains were created also loops 
    // were removed.

    TBool installed( ETrue );

    RPointerArray< CIAUpdateNode > dependencies;
    CleanupClosePushL( dependencies );

    // Accept hidden and visible nodes here.
    // So, if IAD is shown in the UI but it depends on the hidden
    // self updater, the self updater is still included inside the
    // service pack.
    CIAUpdateContentOperationManager::GetOperationNodesL(
        *this, dependencies, ETrue, ETrue );

    for ( TInt i = 0; i < dependencies.Count(); ++i )
        {
        CIAUpdateNode& node( *dependencies[ i ] );
        // Because we have the whole hierarchy already in dependencies array,
        // do not call IsInstalled function for other sub service packs.
        if ( MIAUpdateNode::EPackageTypeServicePack != node.Type()
             && !node.IsSelfUpdate()
             && !node.IsInstalled() )
            {
            IAUPDATE_TRACE("[IAUPDATE] Item not installed."); 

            // If we come here at least one dependency node was not thougth as
            // installed. So, think service pack as not installed also.

            // Notice! Here we skip self update nodes because at the moment
            // IAD Engine does not support self updates inside service packs.
            // Self update nodes are always shown outside of service pack in 
            // the UI. So, if all the nodes inside service pack are self
            // updates, think service pack as installed. Then, seemingly empty
            // service pack itself will be left out of the UI but the
            // self update items would be anyways shown in the UI and they
            // are still available.
            // When service pack has an item that does not require self update,
            // it is not thought as empty any more.
            installed = EFalse;
            break;
            }
        }

    CleanupStack::PopAndDestroy( &dependencies );
    
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateServicePackNode::IsInstalledL() end: %d",
                     installed);

    return installed;
    }


