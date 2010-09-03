/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdateInstallOperation 
*
*/



#include <ncdquery.h>
#include <ncdprogress.h>
#include <ncdoperation.h>
#include <ncdinstalloperation.h>
#include <ncdnodeinstall.h>
#include <ncdnode.h>


#include "iaupdateinstalloperation.h"
#include "iaupdatenodeimpl.h"
#include "iaupdateutils.h"
#include "iaupdatedebug.h"


// ---------------------------------------------------------------------------
// CIAUpdateInstallOperation::NewL
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateInstallOperation* CIAUpdateInstallOperation::NewL(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
    {
    CIAUpdateInstallOperation* self = 
        CIAUpdateInstallOperation::NewLC( aNode, aObserver );
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// CIAUpdateInstallOperation::NewLC
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateInstallOperation* CIAUpdateInstallOperation::NewLC(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
    {
    CIAUpdateInstallOperation* self =
        new( ELeave ) CIAUpdateInstallOperation( aNode, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIAUpdateInstallOperation::CIAUpdateInstallOperation
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateInstallOperation::CIAUpdateInstallOperation(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
: CIAUpdateNormalContentOperation( aNode, aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::CIAUpdateInstallOperation");
    }


// ---------------------------------------------------------------------------
// CIAUpdateInstallOperation::ConstructL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateInstallOperation::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::ConstructL begin");
    
    CIAUpdateNormalContentOperation::ConstructL();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::ConstructL end");
    }
    

// ---------------------------------------------------------------------------
// CIAUpdateInstallOperation::~CIAUpdateInstallOperation
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateInstallOperation::~CIAUpdateInstallOperation()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::~CIAUpdateInstallOperation begin");

    // This will also reset and delete all the necessary data if needed.
    CancelOperation();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::~CIAUpdateInstallOperation end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateInstallOperation::InstallProgress
// Content install functions
// ---------------------------------------------------------------------------
//
void CIAUpdateInstallOperation::InstallProgress( 
    MNcdInstallOperation& /*aOperation*/, 
    TNcdProgress aProgress )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::InstallProgress() begin");
    
    OperationProgress( aProgress.iProgress, aProgress.iMaxProgress );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::InstallProgress() end");
    }

// ---------------------------------------------------------------------------
// CIAUpdateInstallOperation::QueryReceived
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateInstallOperation::QueryReceived( 
    MNcdInstallOperation& aOperation, 
    MNcdQuery* aQuery )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::QueryReceived() begin");

    ContentQueryReceived( aOperation, aQuery );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::QueryReceived() end");
    }

// ---------------------------------------------------------------------------
// CIAUpdateInstallOperation::OperationComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateInstallOperation::OperationComplete( 
    MNcdInstallOperation& aOperation, 
    TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::OperationComplete() begin");

    ContentOperationComplete( aOperation, aError );
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::OperationComplete() end");
    }
        

// ---------------------------------------------------------------------------
// CIAUpdateInstallOperation::HandleContentL
// 
// ---------------------------------------------------------------------------
// 
MNcdOperation* CIAUpdateInstallOperation::HandleContentL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::HandleContentL begin");

    MNcdOperation* operation( NULL );

    // If node is already installed, then do not install it again.
    // Notice, that casting is safe here, because NewLC function uses
    // CIAUpdateNode which is given to the constructor and for parent class. 
    // So, the node is CIAUpdateNode even if the parent operation class uses
    // pointer to node's parent. 
    CIAUpdateNode& node( static_cast< CIAUpdateNode& >( Node() ) );
    if ( !node.IsInstalled() )
        {
        IAUPDATE_TRACE("[IAUPDATE] Create install operation.");
        operation = InstallL();
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::HandleContentL end");

    return operation;
    }


// ---------------------------------------------------------------------------
// CIAUpdateInstallOperation::InstallL
// 
// ---------------------------------------------------------------------------
// 
MNcdOperation* CIAUpdateInstallOperation::InstallL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::InstallL() begin");
    
    MNcdOperation* operation( NULL );
    
    MNcdNodeInstall* install( 
        Node().Node().QueryInterfaceLC<MNcdNodeInstall>() );
    
    if( install )
        {
        // Install interface was available.
        // So, start installing with silent install operation.
        // Get silent install parameters 
        Usif::COpaqueNamedParams * options = Usif::COpaqueNamedParams::NewLC();

        IAUpdateUtils::UsifSilentInstallOptionsL ( Node(), options );
        
        operation = install->SilentInstallL( *this, options );

        if( !operation )
            {
            // Could not create the operation for some reason.
            IAUPDATE_TRACE("[IAUPDATE] ERROR Could not create the operation.");
            User::Leave( KErrGeneral );
            }
        CleanupStack::Pop( options );
        CleanupStack::PopAndDestroy( install );
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] Install interface was not found.");
        User::Leave( KErrNotFound );
        }
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallOperation::InstallL() end");

    return operation;
    }
