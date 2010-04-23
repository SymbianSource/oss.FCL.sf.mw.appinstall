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
* Description:    CIAUpdateHistoryItem
*
*/




#include <ncdpurchasedetails.h>
#include <ncdprovider.h>
#include <ncdnode.h>
#include <ncdnodeinstall.h>
#include <ncdnodedownload.h>

#include "iaupdatehistoryitemimpl.h"
#include "iaupdatecontentoperationmanager.h"


CIAUpdateHistoryItem* CIAUpdateHistoryItem::NewL( MNcdPurchaseDetails* aDetails,
                                                  MNcdProvider& aProvider )
    {
    CIAUpdateHistoryItem* self = 
        CIAUpdateHistoryItem::NewLC( aDetails, aProvider );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateHistoryItem* CIAUpdateHistoryItem::NewLC( MNcdPurchaseDetails* aDetails,
                                                   MNcdProvider& aProvider )
    {    
    CIAUpdateHistoryItem* self = 
        new( ELeave ) CIAUpdateHistoryItem( aProvider );
    CleanupStack::PushL( self );
    self->ConstructL( aDetails );
    return self;
    }


CIAUpdateHistoryItem::CIAUpdateHistoryItem( MNcdProvider& aProvider )
: CBase(),
  iProvider( aProvider )
    {
    
    }
 
    
void CIAUpdateHistoryItem::ConstructL( MNcdPurchaseDetails* aDetails )
    {
    if ( !aDetails )
        {
        User::Leave( KErrArgument );
        }

    // The details is set here instead of in the constructor, because now
    // the details object ownership can be safely taken. If the ownership would be
    // transferred in constructor, a memory leak could happen if new( ELeave )
    // function leaves.
    iDetails = aDetails;    
    }
 
    
CIAUpdateHistoryItem::~CIAUpdateHistoryItem()
    {
    delete iDetails;
    }


MNcdPurchaseDetails& CIAUpdateHistoryItem::Details() const
    {
    return *iDetails;
    }


MNcdProvider& CIAUpdateHistoryItem::Provider() const
    {
    return iProvider;
    }


const TDesC& CIAUpdateHistoryItem::Name() const
    {
    return Details().ItemName();
    }
    
const TDesC& CIAUpdateHistoryItem::Version() const
    {
    return Details().Version();
    }

TTime CIAUpdateHistoryItem::LastOperationTime() const
    {
    return Details().LastOperationTime();
    }


TInt CIAUpdateHistoryItem::LastOperationErrorCode() const
    {
    // Make sure that error code does not contain possible
    // base error code value.
    return 
        CIAUpdateContentOperationManager::CheckErrorCode( 
            Details().LastOperationErrorCode() );
    }


CIAUpdateHistoryItem::TIAHistoryItemState CIAUpdateHistoryItem::StateL() const
    {
    // Set the preliminary state for the node.
    // Purchase state is the initial state of the purchase details.
    TIAHistoryItemState state( EPurchased );

    // Notice, when node content has been installed, its purchase details state
    // will remain as EStateInstalled even if the content is uninstalled.
    // Also notice, that an error may occur during an operation or an error code 
    // may be set by NCD API user because operations have been cancelled. 
    // Thus, purchase details can not be used here for all the cases because 
    // in some situations the update item content may have been uninstalled 
    // and after that the operation can be cancelled. After that, purhcase details 
    // still give EStateInstalled, but the error code may be set by NCD API user. So, 
    // use purchase details for error code but use the node interfaces to get the 
    // correct state information.
    

    // First check if the node is a service pack because then we need to 
    // handle it differently.
    CIAUpdateContentOperationManager::TContentOperationType operationType(
        CIAUpdateContentOperationManager::ServicePackOperationType( Details() ) );
    if ( operationType != CIAUpdateContentOperationManager::ENoContentOperation )
        {
        // Because we come here, it means that the node is a service pack.
        // Now check what was the latest operation.
        switch ( operationType )
            {
            case CIAUpdateContentOperationManager::EPurchaseOperation:
                state = EPurchased;
                break;

            case CIAUpdateContentOperationManager::EDownloadOperation:
                if ( LastOperationErrorCode() == KErrNone )
                    {
                    // Because error code describe success in operation.
                    state = EDownloaded;                    
                    }
                else
                    {
                    // Error code says that download has not been success.
                    // So, only purchase operation has been fully done.
                    state = EPurchased;
                    }
                break;

            case CIAUpdateContentOperationManager::EInstallOperation:
                if ( LastOperationErrorCode() == KErrNone )
                    {
                    // Because error code describe success in operation.
                    state = EInstalled;                    
                    }
                else
                    {
                    // Error code says that install has not been success.
                    // So, only download operation has been fully done.
                    state = EDownloaded;
                    }
                break;
            
            default:
                User::Leave( KErrNotSupported );
                break;
            }

        return state;
        }


    // Get the node from the provider. The node can be gotten by using the
    // purchase details.
    MNcdNode* node( Provider().NodeL( Details() ) );
    if ( !node )
        {
        // Node should always be found if its purchase details can be found
        // from the purchase history.
        User::Leave( KErrNotFound );
        }
    CleanupReleasePushL( *node );
    
    // First check if the node has already been installed and it is still installed.
    MNcdNodeInstall* install( node->QueryInterfaceLC<MNcdNodeInstall>() );
    if ( install )
        {
        if ( install->IsInstalledL() )
            {
            // Content has already been installed.
            // So, the whole process has been completed.
            // Notice, downloaded files are removed or
            // moved after installation.
            state = EInstalled;                
            }
        CleanupStack::PopAndDestroy( install );
        install = NULL;
        }

    if ( state != EInstalled )
        {
        if ( LastOperationErrorCode() == KErrNone
             && Details().State() == MNcdPurchaseDetails::EStateInstalled )
            {
            // This check is required for the cases where the node has been installed,
            // but it has been uninstalled after that.
            // If error code is KErrNone and purchase details value is EStateInstalled,
            // then the last operation has been an install-operation that was success.
            // Because the content is not in the phone anymore, it must have been 
            // uninstalled. Anyways, the last history information should show
            // installed state.
            state = EInstalled;
            }
        else
            {
            // Node was not installed. So, check if it has been downloaded and it is
            // ready for installing.
            MNcdNodeDownload* download( node->QueryInterfaceLC<MNcdNodeDownload>() );
            if ( download )
                {
                if ( download->IsDownloadedL() )
                    {
                    // Content has not been installed yet, but it has been
                    // downloaded and it is available.
                    state = EDownloaded;
                    }
                CleanupStack::PopAndDestroy( download );
                download = NULL; 
                }            
            }
        }
        
    CleanupStack::PopAndDestroy( node );
    node = NULL;
        
    return state;
    }
