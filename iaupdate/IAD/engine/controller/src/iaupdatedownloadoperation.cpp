/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdateDownloadOperation 
*
*/



#include <ncdquery.h>
#include <ncdprogress.h>
#include <ncdoperation.h>
#include <ncdnodedownload.h>
#include <ncddownloadoperation.h>
#include <ncdnode.h>

#include "iaupdatedownloadoperation.h"
#include "iaupdatenodeimpl.h"



#include "iaupdatedebug.h"


// ---------------------------------------------------------------------------
// CIAUpdateDownloadOperation::NewL
// 
// ---------------------------------------------------------------------------
//
CIAUpdateDownloadOperation* CIAUpdateDownloadOperation::NewL(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
    {
    CIAUpdateDownloadOperation* self = 
        CIAUpdateDownloadOperation::NewLC( aNode, aObserver );
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// CIAUpdateDownloadOperation::NewLC
// 
// ---------------------------------------------------------------------------
//
CIAUpdateDownloadOperation* CIAUpdateDownloadOperation::NewLC(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
    {
    CIAUpdateDownloadOperation* self =
        new( ELeave ) CIAUpdateDownloadOperation( aNode, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIAUpdateDownloadOperation::CIAUpdateDownloadOperation
// 
// ---------------------------------------------------------------------------
//
CIAUpdateDownloadOperation::CIAUpdateDownloadOperation(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
: CIAUpdateNormalContentOperation( aNode, aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::CIAUpdateDownloadOperation");
    }


// ---------------------------------------------------------------------------
// CIAUpdateDownloadOperation::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateDownloadOperation::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::ConstructL begin");
    
    CIAUpdateNormalContentOperation::ConstructL();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::ConstructL end");
    }
    

// ---------------------------------------------------------------------------
// CIAUpdateDownloadOperation::~CIAUpdateDownloadOperation
// 
// ---------------------------------------------------------------------------
//
CIAUpdateDownloadOperation::~CIAUpdateDownloadOperation()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::~CIAUpdateDownloadOperation begin");

    // This will also reset and delete all the necessary data if needed.
    CancelOperation();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::~CIAUpdateDownloadOperation end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateDownloadOperation::DownloadProgress
// Content download functions
// ---------------------------------------------------------------------------
//
void CIAUpdateDownloadOperation::DownloadProgress( MNcdDownloadOperation& /*aOperation*/, 
                                                   TNcdProgress aProgress )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::DownloadProgress() begin");

    OperationProgress( aProgress.iProgress, aProgress.iMaxProgress );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::DownloadProgress() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateDownloadOperation::QueryReceived
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateDownloadOperation::QueryReceived( MNcdDownloadOperation& aOperation, 
                                                MNcdQuery* aQuery )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::QueryReceived() begin");

    ContentQueryReceived( aOperation, aQuery );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::QueryReceived() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateDownloadOperation::OperationComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateDownloadOperation::OperationComplete( MNcdDownloadOperation& aOperation, 
                                                    TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::OperationComplete() begin");

    ContentOperationComplete( aOperation, aError );
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::OperationComplete() end");
    }        


// ---------------------------------------------------------------------------
// CIAUpdateDownloadOperation::HandleContentL
// 
// ---------------------------------------------------------------------------
//
MNcdOperation* CIAUpdateDownloadOperation::HandleContentL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::HandleContentL begin");

    MNcdOperation* operation( NULL );

    // If node is already installed, then do not download it again.
    // Notice, that casting is safe here, because NewLC function uses
    // CIAUpdateNode which is given to the constructor and for parent class. 
    // So, the node is CIAUpdateNode even if the parent operation class uses
    // pointer to node's parent. 
    CIAUpdateNode& node( static_cast< CIAUpdateNode& >( Node() ) );
    if ( !node.IsInstalled() )
        {
        IAUPDATE_TRACE("[IAUPDATE] Create download operation.");
        operation = DownloadL();
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::HandleContentL end");

    return operation;
    }


// ---------------------------------------------------------------------------
// CIAUpdateDownloadOperation::DownloadL
// 
// ---------------------------------------------------------------------------
//    
MNcdOperation* CIAUpdateDownloadOperation::DownloadL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDownloadOperation::DownloadL() begin");

    MNcdOperation* operation( NULL );
    
    MNcdNodeDownload* download = 
        Node().Node().QueryInterfaceLC< MNcdNodeDownload >();
    
    if ( download )
        {
        // Download interface exists. So, we can do downloading.
        if( !download->IsDownloadedL() )
            {
            // Only do download operation if we have not already downloaded.
            operation = download->DownloadContentL( *this );
            
            if( !operation )
                {
                // Could not create the operation for some reason.
                IAUPDATE_TRACE("[IAUPDATE] ERROR Could not create the operation.");
                User::Leave( KErrGeneral );
                }
            }

        CleanupStack::PopAndDestroy( download );
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] Download interface was not found.");
        User::Leave( KErrNotFound );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::DownloadL() end");

    return operation;
    }
        


