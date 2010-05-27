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

#include "iaupdatespecialnode.h"
#include "iaupdatedebug.h"


// ---------------------------------------------------------------------------
// CIAUpdateSpecialNode::NewLC
// 
// ---------------------------------------------------------------------------
//
CIAUpdateSpecialNode* CIAUpdateSpecialNode::NewLC( MNcdNode* aNode,
                                                   CIAUpdateController& aController )
    {
    CIAUpdateSpecialNode* self = 
        new( ELeave ) CIAUpdateSpecialNode( aController );
    CleanupStack::PushL( self );    
    self->ConstructL( aNode );
    return self;
    }
  
    
// ---------------------------------------------------------------------------
// CIAUpdateSpecialNode::NewL
// 
// ---------------------------------------------------------------------------
//    
CIAUpdateSpecialNode* CIAUpdateSpecialNode::NewL( MNcdNode* aNode,
                                                  CIAUpdateController& aController )
    {
    CIAUpdateSpecialNode* self = 
        CIAUpdateSpecialNode::NewLC( aNode, aController );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CIAUpdateSpecialNode::CIAUpdateSpecialNode
// 
// ---------------------------------------------------------------------------
//
CIAUpdateSpecialNode::CIAUpdateSpecialNode( CIAUpdateController& aController ) 
: CIAUpdateNode( aController )
    {
    }


// ---------------------------------------------------------------------------
// CIAUpdateSpecialNode::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateSpecialNode::ConstructL( MNcdNode* aNode )
    {
    CIAUpdateNode::ConstructL( aNode );
    }

    
// ---------------------------------------------------------------------------
// CIAUpdateSpecialNode::~CIAUpdateSpecialNode
// 
// ---------------------------------------------------------------------------
//
CIAUpdateSpecialNode::~CIAUpdateSpecialNode()
    {
    }


// ---------------------------------------------------------------------------
// CIAUpdateSpecialNode::IsSelfUpdate
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateSpecialNode::IsSelfUpdate() const    
    {
    return ETrue;
    }        


// ---------------------------------------------------------------------------
// CIAUpdateSpecialNode::SetInstallStatusToPurchaseHistoryL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateSpecialNode::SetInstallStatusToPurchaseHistoryL( 
    TInt aErrorCode, TBool aForceVisibleInHistory )
    {
    IAUPDATE_TRACE_2("[IAUPDATE] CIAUpdateSpecialNode::SetInstallStatusToPurchaseHistoryL() begin: %d, %d",
                     aErrorCode, aForceVisibleInHistory);

    MNcdNodeInstall* install( Node().QueryInterfaceLC<MNcdNodeInstall>() );
    if ( !install )
        {
        User::Leave( KErrNotFound );
        }

    // This will update purchase history and do all the required settings.
    // Notice, that this will also handle report sending.
    install->SetApplicationInstalledL( aErrorCode );

    // Because in some cases the MIME type of the node should contain postfixes
    // that are not given from the server, use the IAD function just in case to
    // make sure that purchase history contains the correct MIME values. This
    // is not necessary in most cases but just to be sure.
    SetIdleErrorToPurchaseHistoryL( aErrorCode, aForceVisibleInHistory );

    CleanupStack::PopAndDestroy( install );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSpecialNode::SetInstallStatusToPurchaseHistoryL() end");
    }
