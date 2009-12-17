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



#include <ncdnodecontentinfo.h>
#include <ncdnode.h>
#include <catalogsuids.h>

#include "iaupdatenodefactory.h"
#include "iaupdatenodeimpl.h"
#include "iaupdatefwnodeimpl.h"
#include "iaupdatespecialnode.h"
#include "iaupdateservicepacknode.h"
#include "iaupdateruids.h"
#include "iaupdatectrlnodeconsts.h"
#include "iaupdateprotocolconsts.h"
#include "iaupdatedebug.h"


// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::CreateNodeLC
// 
// ---------------------------------------------------------------------------
//
CIAUpdateNode* IAUpdateNodeFactory::CreateNodeLC( 
    MNcdNode* aNode,
    CIAUpdateController& aController )
    {
    CIAUpdateNode* updateNode( 
        IAUpdateNodeFactory::CreateNodeL( aNode, aController ) );
    CleanupStack::PushL( updateNode );
    return updateNode;
    }
    

// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::CreateNodeL
// 
// ---------------------------------------------------------------------------
//
CIAUpdateNode* IAUpdateNodeFactory::CreateNodeL( 
    MNcdNode* aNode, 
    CIAUpdateController& aController )
    {
    if ( !aNode )
        {
        User::Leave( KErrArgument );
        }

    CIAUpdateNode* updateNode( NULL );

    TUid nodeUid( TUid::Null() ); 
    TBool isServicePack( EFalse );

    // Put the node into the cleanup stack.
    // So, it will be released if update node creation fails.
    CleanupReleasePushL( *aNode );

    // Get information from content info.
    MNcdNodeContentInfo* contentInfo( 
        aNode->QueryInterfaceLC< MNcdNodeContentInfo >() );
    if ( contentInfo )
        {
        // Notice, check if the node is actually a firmware node.
        // Do not accept them. Content info is used to check this.
        // Because content info exists, also checking can be done here without
        // leaving because content info did not exist.
        if ( IsFwUpdateL( *aNode ) )
            {
            User::Leave( KErrArgument );
            }

        nodeUid = contentInfo->Uid(); 
        isServicePack = IsServicePack( contentInfo->MimeType() );

        // Release content info.
        // So, we can later check if it still exists and then get 
        // new one if it exists.
        CleanupStack::PopAndDestroy( contentInfo );
        contentInfo = NULL;
        }

    if ( IsSelfUpdate( nodeUid ) )
        {
        // Node contents require special install handling. 
        // So, create special node.
        updateNode = CIAUpdateSpecialNode::NewL( aNode, aController );
        }
    else if ( isServicePack )
        {
        // The node is actually service pack.
        // Notice, that the service pack should not contain any content
        // itself. Instead, content is provided via dependencies.
        // So, self update nodes should never be service packs themselves.
        // So, they can be handled separately above in if-clause.
        updateNode = CIAUpdateServicePackNode::NewL( aNode, aController );
        }
    else
        {
        // Normal node.
        updateNode = CIAUpdateNode::NewL( aNode, aController );       
        }

    // The update node took the ownership. So, it is safe to pop.        
    CleanupStack::Pop( aNode );
        
    return updateNode;
    }


// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::IsSelfUpdate
// 
// ---------------------------------------------------------------------------
//
TBool IAUpdateNodeFactory::IsSelfUpdate( const TUid& aUid )
    {
    // The UI, that uses this iaupdate engine dll, is thought as a special content.
    // Iaupdate uses NCD Engine to install itself. So, it requires also special installer.
    // These special cases should be installed by using updater.

    if ( IsIad( aUid ) || IsNcd( aUid ) )    
        {
        // Special installing is required.
        return ETrue;
        }

    return EFalse;
    }


// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::IsIad
// 
// ---------------------------------------------------------------------------
//
TBool IAUpdateNodeFactory::IsIad( const TUid& aUid )
    {
    RProcess process;
    TUid uiUid( process.SecureId() );
    
    if ( aUid == uiUid )
        {
        // UID matched the IAD UID.
        return ETrue;
        }

    return EFalse;
    }


// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::IsNcd
// 
// ---------------------------------------------------------------------------
//
TBool IAUpdateNodeFactory::IsNcd( const TUid& aUid )
    {
    if ( aUid.iUid == KCatalogsServerUid
         || aUid.iUid == KNcdProviderUid
         || aUid.iUid == KCatalogsEngineUid
         || aUid.iUid == KCatalogsEngineInterfaceUid
         || aUid.iUid == KCatalogsEngineImplementationUid )
        {
        // UID matched to one NCD UID.
        return ETrue;
        }

    return EFalse;
    }


// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::IsUpdater
// 
// ---------------------------------------------------------------------------
//
TBool IAUpdateNodeFactory::IsUpdater( const TUid& aUid )
    {
    if ( aUid.iUid == KIAUpdaterUid )
        {
        // UID matched the self updater UID.
        return ETrue;
        }

    return EFalse;
    }


// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::CreateFwNodeLC
// 
// ---------------------------------------------------------------------------
//
CIAUpdateFwNode* IAUpdateNodeFactory::CreateFwNodeLC( 
    MNcdNode* aNode,
    CIAUpdateController& aController )
    {
    CIAUpdateFwNode* updateNode( 
        IAUpdateNodeFactory::CreateFwNodeL( aNode, aController ) );
    CleanupStack::PushL( updateNode );
    return updateNode;
    }
    

// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::CreateFwNodeL
// 
// ---------------------------------------------------------------------------
//    
CIAUpdateFwNode* IAUpdateNodeFactory::CreateFwNodeL( 
    MNcdNode* aNode, 
    CIAUpdateController& aController )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateNodeFactory::CreateFwNodeL() begin");
    
    if ( !aNode )
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR: NULL node");
        User::Leave( KErrArgument );
        }

    CIAUpdateFwNode* updateNode( NULL );

    // Put the node into the cleanup stack.
    // So, it will be released if update node creation fails.
    CleanupReleasePushL( *aNode );

    MNcdNodeContentInfo* contentInfo(
        aNode->QueryInterfaceLC< MNcdNodeContentInfo >() );
    if ( contentInfo )
        {
        IAUPDATE_TRACE("[IAUPDATE] Node has content info");
        
        const TDesC& mime( contentInfo->MimeType() );
        IAUPDATE_TRACE_1("[IAUPDATE] Mime: %S", &mime);        
        
        if ( mime == IAUpdateProtocolConsts::KMimeFotaDp2 )
            {
            IAUPDATE_TRACE("[IAUPDATE] Fota Dp2");
            updateNode = 
                CIAUpdateFwNode::NewL( 
                    aNode, aController, MIAUpdateFwNode::EFotaDp2 );
            }
        else if ( mime == IAUpdateProtocolConsts::KMimeFotiNsu )
            {
            IAUPDATE_TRACE("[IAUPDATE] Foti Nsu");
            updateNode = 
                CIAUpdateFwNode::NewL( 
                    aNode, aController, MIAUpdateFwNode::EFotiNsu );
            }
        else
            {
            // Unknown firmware mime type
            IAUPDATE_TRACE("[IAUPDATE] ERROR: Unknown MIME for fw");
            User::Leave( KErrArgument );
            }

        // Firmware item should always have content info available.
        CleanupStack::PopAndDestroy( contentInfo );
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR: Firmware item did not have content info");
        User::Leave( KErrNotFound );
        }
                     
    // The update node took the ownership. So, it is safe to pop.        
    CleanupStack::Pop( aNode );

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateNodeFactory::CreateFwNodeL() end");
        
    return updateNode;
    }


// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::IsFwUpdateL
// 
// ---------------------------------------------------------------------------
//
TBool IAUpdateNodeFactory::IsFwUpdateL( MNcdNode& aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateNodeFactory::IsFwUpdateL() begin");
    
    TBool isFwUpdate( EFalse );

    MNcdNodeContentInfo* contentInfo(
        aNode.QueryInterfaceLC< MNcdNodeContentInfo >() );
    if ( contentInfo )
        {
        IAUPDATE_TRACE("[IAUPDATE] Node has content info");

        const TDesC& mime( contentInfo->MimeType() );
        IAUPDATE_TRACE_1("[IAUPDATE] Mime: %S", &mime);

        isFwUpdate = IsFwUpdate( mime );

        // Firmware item should always have content info available.
        CleanupStack::PopAndDestroy( contentInfo );
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR: Firmware item did not have content info");
        User::Leave( KErrNotFound );
        }

    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateNodeFactory::IsFwUpdateL() end: %d",
                     isFwUpdate);

    return isFwUpdate;
    }


// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::IsFwUpdate
// 
// ---------------------------------------------------------------------------
//
TBool IAUpdateNodeFactory::IsFwUpdate( const TDesC& aMime )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateNodeFactory::IsFwUpdate() begin");
    
    TBool isFwUpdate( EFalse );
    
    if ( aMime == IAUpdateProtocolConsts::KMimeFotaDp2
         || aMime == IAUpdateProtocolConsts::KMimeFotiNsu )
        {
        isFwUpdate = ETrue;
        }

    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateNodeFactory::IsFwUpdate() end: %d",
                     isFwUpdate);
    
    return isFwUpdate;
    }


// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::IsServicePack
// 
// ---------------------------------------------------------------------------
//     
TBool IAUpdateNodeFactory::IsServicePack( const TDesC& aMime )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateNodeFactory::IsServicePack: %d",
                     ( aMime.Match( IAUpdateProtocolConsts::KMimeServicePackPattern() )
                       != KErrNotFound ));

    // Notice, that here we compare the beginning of the MIME.
    // So, the end of the MIME may contain some postfix, for example,
    // IAUpdateCtrlNodeConsts::KMimeServicePackDependencyPostfix. But,
    // we always interpret the item as service pack if the MIME so 
    // suggests. So, even if this service pack is a service pack inside
    // another service pack. This is still service pack.
    return ( aMime.Match( IAUpdateProtocolConsts::KMimeServicePackPattern() )
             != KErrNotFound );
    }


// ---------------------------------------------------------------------------
// IAUpdateNodeFactory::IsHidden
// 
// ---------------------------------------------------------------------------
//     
TBool IAUpdateNodeFactory::IsHidden( const TDesC& aMime )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateNodeFactory::IsHidden(): %d",
                     ( aMime.Match( IAUpdateCtrlNodeConsts::KMimeHiddenPostfixPattern() )
                       != KErrNotFound ));

    // Notice, that here we compare the end of the MIME.
    // The end of the MIME may contain some postfix, for example,
    // IAUpdateCtrlNodeConsts::KMimeHiddenPostfix.
    // So, if correct postfix is found, then this is a hidden node.
    return ( aMime.Match( IAUpdateCtrlNodeConsts::KMimeHiddenPostfixPattern() )
             != KErrNotFound );
    }
