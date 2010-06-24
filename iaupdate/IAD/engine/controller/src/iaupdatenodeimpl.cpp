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
* Description:   This module contains the implementation of CIAUpdateNode class 
*                member functions.
*
*/

#include <ncdnode.h>
#include <ncdnodepurchase.h>
#include <ncdnodedownload.h>
#include <ncdnodeinstall.h>
#include <ncdnodecontentinfo.h>

#include "iaupdatenodeimpl.h"
#include "iaupdatenodeobserver.h"
#include "iaupdatenodedependencyimpl.h"
#include "iaupdatenodedetails.h"
#include "iaupdatecontrollerimpl.h"
#include "iaupdateutils.h"
#include "iaupdatecontentoperationmanager.h"
#include "iaupdateprotocolconsts.h"
#include "iaupdatedebug.h"



// -----------------------------------------------------------------------------
// CIAUpdateNode::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
// 
CIAUpdateNode* CIAUpdateNode::NewLC( MNcdNode* aNode,
                                     CIAUpdateController& aController )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::NewLC() begin");

    CIAUpdateNode* self = 
        new( ELeave ) CIAUpdateNode( aController );
    CleanupStack::PushL( self );    
    self->ConstructL( aNode );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::NewLC() end");

    return self;
    }

    
// -----------------------------------------------------------------------------
// CIAUpdateNode::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//     
CIAUpdateNode* CIAUpdateNode::NewL( MNcdNode* aNode,
                                    CIAUpdateController& aController )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::NewL() begin");
    CIAUpdateNode* self = 
        CIAUpdateNode::NewLC( aNode, aController );
    CleanupStack::Pop( self );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::NewL() end");
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateNode::CIAUpdateNode
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateNode::CIAUpdateNode( CIAUpdateController& aController ) 
: CIAUpdateBaseNode( aController )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateNode::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateNode::ConstructL( MNcdNode* aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::ConstructL() begin");

    // Let the parent handle it all.
    CIAUpdateBaseNode::ConstructL( aNode );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::ConstructL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateNode::~CIAUpdateNode
// Destructor
// -----------------------------------------------------------------------------
//    
CIAUpdateNode::~CIAUpdateNode()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::~CIAUpdateNode() begin");

    iDependants.Reset();
    iExcessDependencyNodes.Reset();
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::~CIAUpdateNode() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateBaseNode overloaded functions
// 
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateNode::ContentSizeL
// 
// ---------------------------------------------------------------------------
//
TInt CIAUpdateNode::ContentSizeL() const
    {
    // Notice, here we give the total content size through the interface.
    // Here we want only the dependency nodes that are hidden.
    // Skip visible dependencies because those nodes are visible in UI
    // and their content size is separately shown and calculated in UI.
    return 
        Controller().
            ContentOperationManager().
                TotalContentSizeL( *this, ETrue, EFalse, ETrue, EFalse );        
    }


// ---------------------------------------------------------------------------
// MIAUpdateNode functions
// 
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateNode::Type
// 
// ---------------------------------------------------------------------------
//    
MIAUpdateNode::TPackageType CIAUpdateNode::Type() const
    {
    MIAUpdateNode::TPackageType packageType;
    if ( Mime().Compare( IAUpdateProtocolConsts::KMimeWidget ) == 0 )
        {
        packageType = MIAUpdateNode::EPackageTypeWidget;
        }
    else
        {
        packageType = Details().ContentType();
        }
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::Type() = %d", 
                             packageType );
    return packageType;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::IsSelfUpdate
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateNode::IsSelfUpdate() const    
    {
    return EFalse;
    }        


// ---------------------------------------------------------------------------
// CIAUpdateNode::GetDependenciesL
// 
// ---------------------------------------------------------------------------
//       
void CIAUpdateNode::GetDependenciesL( 
    RPointerArray< MIAUpdateNode >& aDependencies,
    TBool aIncludeHidden ) const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::GetDependenciesL() begin");

    RPointerArray< CIAUpdateNode > dependencies;
    CleanupClosePushL( dependencies );

    GetDependencyNodesL( dependencies, aIncludeHidden );

    aDependencies.ReserveL( dependencies.Count() );    
    for ( TInt i = 0; i < dependencies.Count(); ++i )
        {
        aDependencies.AppendL( dependencies[ i ] );
        }

    CleanupStack::PopAndDestroy( &dependencies );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::GetDependenciesL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::GetDependantsL
// 
// ---------------------------------------------------------------------------
//    
void CIAUpdateNode::GetDependantsL( 
    RPointerArray< MIAUpdateNode >& aDependants,
    TBool aIncludeHidden ) const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::GetDependantsL() begin");

    aDependants.Reserve( iDependants.Count() );
    for( TInt i = 0; i < iDependants.Count(); ++i )
        {
        CIAUpdateNode* dependant( iDependants[ i ] );
        if ( aIncludeHidden 
             || !dependant->Hidden() )
            {
            aDependants.AppendL( iDependants[ i ] );
            }
        } 

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::GetDependantsL() end"); 	
    }
    

// ---------------------------------------------------------------------------
// CIAUpdateNode::IsDownloaded
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateNode::IsDownloaded() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::IsDownloaded() begin");

    TBool downloaded( EFalse );
    TRAP_IGNORE ( downloaded = IsDownloadedL() );

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::IsDownloaded() end: %d",
                     downloaded);

    return downloaded;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::IsInstalled
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateNode::IsInstalled() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::IsInstalled() begin");

    TBool installed( EFalse );
    TRAP_IGNORE ( installed = IsInstalledL() );

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::IsInstalled() end: %d",
                     installed);

    return installed;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::DownloadL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::DownloadL( MIAUpdateNodeObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::DownloadL() begin");

    if( iOperationObserver )
        {
        User::Leave( KErrInUse );
        }

    // Instead of starting download operation here, we start purchase operation.
    // Purchase needs to be done before download. When purchase compeletes,
    // the call back function is called and that will start download operation.
    // Notice, that we can try to create the purchase even if it has already been
    // done. Then, the operation just finishes without repurchasing.

    // Use content operation manager.
    // It can handle the possible hidden node chain that requires operations 
    // for multiple nodes.
    Controller().
        ContentOperationManager().
            StartL( *this, 
                    CIAUpdateContentOperationManager::EPurchaseOperation, 
                    *this );

    iOperationObserver = &aObserver;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::DownloadL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::InstallL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::InstallL( MIAUpdateNodeObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::InstallL() begin");
    
    if( iOperationObserver )
        {
        User::Leave( KErrInUse );
        }

    // Use content operation manager.
    // It can handle the possible hidden node chain that requires operations 
    // for multiple nodes.
    Controller().
        ContentOperationManager().
            StartL( *this, 
                    CIAUpdateContentOperationManager::EInstallOperation, 
                    *this );

    iOperationObserver = &aObserver;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::InstallL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::CancelOperation()
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::CancelOperation()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::CancelOperation() begin");

    // This will cancel the possible ongoing operation
    Controller().ContentOperationManager().Cancel();
    
    // Also, set the observer to NULL.
    // Because this value is used to check if operations are going on.
    iOperationObserver = NULL;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::CancelOperation() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::Depth()
// 
// ---------------------------------------------------------------------------
//
TInt CIAUpdateNode::Depth() const
    {
    return iDepth;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::SetUiState()
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::SetUiState( MIAUpdateNode::TUIUpdateState aState )
    {
    iUiUpdateState = aState;
    }

// ---------------------------------------------------------------------------
// CIAUpdateNode::UiState()
// 
// ---------------------------------------------------------------------------
//
MIAUpdateNode::TUIUpdateState CIAUpdateNode::UiState() const
    {
    return iUiUpdateState;
    }

// ---------------------------------------------------------------------------
// CIAUpdateNode::SetProgress
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::SetProgress( TInt aProgress )
    {
    iProgress = aProgress; 
    }

// ---------------------------------------------------------------------------
// CIAUpdateNode::Progress
// 
// ---------------------------------------------------------------------------
//
TInt CIAUpdateNode::Progress() const
    {
    return iProgress;
    }

// ---------------------------------------------------------------------------
// CIAUpdateNode::SetTotal
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::SetTotal( TInt aTotal )
    {
    iTotal = aTotal; 
    }

// ---------------------------------------------------------------------------
// CIAUpdateNode::Total
// 
// ---------------------------------------------------------------------------
//
TInt CIAUpdateNode::Total() const
    {    
    return iTotal;
    }
    

// ---------------------------------------------------------------------------
// CIAUpdateNode::NodeType
// 
// ---------------------------------------------------------------------------
//
MIAUpdateAnyNode::TNodeType CIAUpdateNode::NodeType() const
    {
    return MIAUpdateAnyNode::ENodeTypeNormal;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::Base
// 
// ---------------------------------------------------------------------------
//
MIAUpdateBaseNode& CIAUpdateNode::Base()
    {
    return *this;   
    }


// ---------------------------------------------------------------------------
// MIAUpdateContentOperationObserver functions
// 
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateNode::ContentOperationCompleteL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::ContentOperationComplete( CIAUpdateBaseNode& /*aNode*/, 
                                              TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::ContentOperationComplete() begin");

    // Get the observer to a temporary pointer. So, we can use it 
    // when informing observers. The member variable needs to be set to NULL
    // before callbacks are called.
    MIAUpdateNodeObserver* tmpObserver( iOperationObserver );
    
    const CIAUpdateContentOperationManager::TContentOperationType& operationType(
        Controller().ContentOperationManager().OperationType() );

    if ( operationType == CIAUpdateContentOperationManager::EPurchaseOperation )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Purchase operation complete: %d", aError);
        if ( aError == KErrNone )
            {
            // Purchase operation was successfull.
            // So, now try to download the actual content.
            TRAPD ( trapError, 
                    Controller().
                        ContentOperationManager().
                            StartL( *this, 
                                    CIAUpdateContentOperationManager::EDownloadOperation, 
                                    *this ); );
            IAUPDATE_TRACE_1("[IAUPDATE] download trap error code: %d", trapError );
            if ( trapError != KErrNone )
                {
                IAUPDATE_TRACE("[IAUPDATE] Could not create download operation.");
                // Something went wrong when initializing download operation.
                iOperationObserver = NULL;
                // Inform observer about the completion of operation.
                tmpObserver->DownloadComplete( *this, trapError );
                }            
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] Purchase was not success. Complete download.");
            // Operation was not successfull.
            // There is no reason to continue.
            // Inform observer about the completion of operation.
            iOperationObserver = NULL;
            tmpObserver->DownloadComplete( *this, aError );
            }
        }
    else if ( operationType == CIAUpdateContentOperationManager::EDownloadOperation )
        {
        IAUPDATE_TRACE("[IAUPDATE] Download operation complete");
        iOperationObserver = NULL;
        // Inform observer about the completion of operation.
        tmpObserver->DownloadComplete( *this, aError );
        }
    else if ( operationType == CIAUpdateContentOperationManager::EInstallOperation )
        {
        IAUPDATE_TRACE("[IAUPDATE] Install operation complete");
        iOperationObserver = NULL;
        // Inform observers about the completion of operation.
        tmpObserver->InstallComplete( *this, aError );        
        }
    else
        {
        // We should never come here.
        IAUPDATE_TRACE("[IAUPDATE] ERROR No operation was going on even if callback called");        
        iOperationObserver = NULL;
        }
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::ContentOperationComplete() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::ContentOperationProgress
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateNode::ContentOperationProgress( CIAUpdateBaseNode& /*aNode*/, 
                                              TInt aProgress, 
                                              TInt aMaxProgress )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::ContentOperationProgress() begin");

    // Inform the observer about the progress of download and install.
    // There is no need to inform about the purchase progress.
    switch( Controller().ContentOperationManager().OperationType() )
        {
        case CIAUpdateContentOperationManager::EDownloadOperation:
            IAUPDATE_TRACE("[IAUPDATE] Download operation progress");
            iOperationObserver->
                DownloadProgress( *this, aProgress, aMaxProgress );
            break;

        case CIAUpdateContentOperationManager::EInstallOperation:
            IAUPDATE_TRACE("[IAUPDATE] Install operation progress");
            iOperationObserver->
                InstallProgress( *this, aProgress, aMaxProgress );
            break;

        default:
            IAUPDATE_TRACE_1("[IAUPDATE] Do not inform observer: %d",
                             Controller().
                                ContentOperationManager().
                                    OperationType());
            break;
        }
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::ContentOperationProgress() end");
    }


// ---------------------------------------------------------------------------
// Public functions
// 
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateNode::Reset
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::Reset()
    {
    SetDependencyCheckStatus( 
        CIAUpdateNode::EDependencyCheckNotSet );
    SetDepth( 0 );
    SetLeafDistance( 0 );
    iDependants.Reset();
    iExcessDependencyNodes.Reset();
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::IsPurchased
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateNode::IsPurchased() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::IsPurchased() begin");

    TBool purchased( EFalse );
    TRAP_IGNORE( purchased = IsPurchasedL() );

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::IsPurchased() end: %d", purchased);

    return purchased;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::SetExcessDependencyL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::SetExcessDependencyL( 
    CIAUpdateNode& aDependencyNode,
    TBool aAddDependency )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::SetExcessDependencyL() begin");
    
    // First check if the dependency has already been set in details.
    RPointerArray< CIAUpdateNodeDependency > dependencies;
    CleanupClosePushL( dependencies );
    Details().GetDependenciesL( dependencies );
    for ( TInt i = 0; i < dependencies.Count(); ++i )
        {
        CIAUpdateNodeDependency* dependency( dependencies[ i ] );
        if ( dependency->Uid() == aDependencyNode.Uid() )
            {
            // Dependency has already been set.
            CleanupStack::PopAndDestroy( &dependencies );
            return;
            }
        }
    CleanupStack::PopAndDestroy( &dependencies );

    // Check if the node already exists in excess array.
    for ( TInt i = 0; i < iExcessDependencyNodes.Count(); ++i )
        {
        CIAUpdateNode* node( iExcessDependencyNodes[ i ] );
        if ( node->Uid() == aDependencyNode.Uid() )
            {
            // Dependency has already been set.            
            return;
            }
        }

    // Depth update and loop check.
    RPointerArray< CIAUpdateNode > dependencyNodes;
    CleanupClosePushL( dependencyNodes );
    // Get dependency hierarchy.
    // Accept all kind of nodes.
    Controller().
        ContentOperationManager().
            GetOperationNodesL( aDependencyNode, 
                                dependencyNodes,
                                ETrue, ETrue );
    TInt findError( 
        dependencyNodes.Find( this ) );
    CleanupStack::PopAndDestroy( &dependencyNodes );

    // If the given dependency node depends on this node,
    // then new dependency would create a loop. So, only
    // accept node if loop will not occur.
    if ( findError == KErrNotFound )
        {
        IAUPDATE_TRACE("[IAUPDATE] Check passed");

        // Update the leaf distances of the this dependant node
        // and its dependant hierarchy if necessary. Dependant leaf
        // distance is always at least one greater than its dependency
        // leaf distance.
        UpdateDependantLeafDistancesL(
            aDependencyNode.LeafDistance() + 1 );

        // Update the depth of the dependency hierarchy
        // if necessary. Dependency depth is always at least one
        // greater than depth of its dependant.
        aDependencyNode.
            UpdateDependencyDepthsL( Depth() + 1 );

        if ( aAddDependency )
            {
            IAUPDATE_TRACE("[IAUPDATE] Add dependency"); 
            // Insert the given dependant node into the array.
            // The given node is thought as the best match for the dependency.
            // Also, this function supposes that the dependency chain below the dependency
            // node is intact.
            iExcessDependencyNodes.AppendL( &aDependencyNode );
            // Also, because new dependency is inserted, make sure that
            // the dependency node dependant info is set correctly.
            aDependencyNode.AddDependantL( *this );            
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::SetExcessDependencyL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::GetDependencyNodesL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::GetDependencyNodesL( 
    RPointerArray< CIAUpdateNode >& aDependencies,
    TBool aIncludeHidden ) const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::GetDependencyNodesL() begin");
    
    RPointerArray< CIAUpdateNodeDependency > dependencies;
    CleanupClosePushL( dependencies );

    // First get the dependencies from the details object.
    // These are dependencies given by the server itself.
    Details().GetDependenciesL( dependencies );
    
    aDependencies.ReserveL( 
        dependencies.Count() + iExcessDependencyNodes.Count() );
    
    // Insert node that corresponds the details to the array.
    for ( TInt i = 0; i < dependencies.Count(); ++i )
        {
        CIAUpdateNodeDependency* dependency( dependencies[ i ] );
        CIAUpdateNode* node( dependency->BestMatch() );
        if ( node && ( aIncludeHidden || !node->Hidden() ) )
            {
            aDependencies.AppendL( node );        
            }
        }
    
    CleanupStack::PopAndDestroy( &dependencies );

    // Insert excess dependencies to the array.
    for ( TInt i = 0; i < iExcessDependencyNodes.Count(); ++i )
        {
        CIAUpdateNode* node( iExcessDependencyNodes[ i ] );
        if ( aIncludeHidden || !node->Hidden() )
            {
            aDependencies.AppendL( node );
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::GetDependencyNodesL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::SetDependencyCheckStatus
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::SetDependencyCheckStatus( 
    CIAUpdateNode::TDependencyCheckStatus aStatus )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::SetDependencyCheckStatus() = %d", 
                     aStatus );
    iDependencyCheckStatus = aStatus;
    }

// ---------------------------------------------------------------------------
// CIAUpdateNode::DependencyCheckStatus()
// 
// ---------------------------------------------------------------------------
//    
CIAUpdateNode::TDependencyCheckStatus CIAUpdateNode::DependencyCheckStatus() const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::DependencyCheckStatus() = %d", 
                     iDependencyCheckStatus );
    return iDependencyCheckStatus;
    }   
    

// ---------------------------------------------------------------------------
// CIAUpdateNode::LeafDistance
// 
// ---------------------------------------------------------------------------
//
TInt CIAUpdateNode::LeafDistance() const
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::LeafDistance() = %d", 
                     iLeafDistance );
    return iLeafDistance;
    }

// ---------------------------------------------------------------------------
// CIAUpdateNode::SetLeafDistance
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::SetLeafDistance( TInt aDistance )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::SetLeafDistance() = %d", 
                     aDistance );
    iLeafDistance = aDistance;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::SetDepth
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::SetDepth( TInt aDepth )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::SetDepth() = %d", 
                     aDepth );
    iDepth = aDepth;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::DependantNodes
// 
// ---------------------------------------------------------------------------
//
const RPointerArray< CIAUpdateNode >& CIAUpdateNode::DependantNodes() const
    {
    return iDependants;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::AddDependantL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::AddDependantL( CIAUpdateNode& aDependantNode ) 
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::AddDependantL() begin");
	

	for ( TInt i = 0; i < iDependants.Count(); ++i )
	    {
	    CIAUpdateNode* node( iDependants[ i ] );
	    if ( node->Mime().Compare( IAUpdateProtocolConsts::KMimeWidget) == 0 )
	        {
	        if ( node->Identifier() == aDependantNode.Identifier() )
	            {
	            // Corresponding node is already in the array.
	            return;
	            }
	        }
	    else 
	        {
	        if ( node->Uid() == aDependantNode.Uid() )
	            {
	            // Corresponding node is already in the array.
	            return;
	            }
	        }
	    }

	iDependants.AppendL( &aDependantNode );

	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::AddDependantL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::UpdateDependencyDepthsL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::UpdateDependencyDepthsL( TInt aDepth )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::UpdateDependencyDepthsL() begin: %d",
                     aDepth);
    
    // Notice, that by comparing depths here, we also make sure
    // that the depth is increased correctly if we come to same
    // dependency node via multiple branches. Multiple dependants 
    // can depend on the same node.

    // If dependant depth had increased, then depenencies should
    // also increase their depth. Also, update if negative value is given.
    // Then, think this node as root.
    if ( aDepth > Depth() )
        {
        IAUPDATE_TRACE("[IAUPDATE] Update dependency depth");

        SetDepth( aDepth );

        RPointerArray< CIAUpdateNode > dependencies;
        CleanupClosePushL( dependencies );
        
        // Also, accept hidden nodes here.
        GetDependencyNodesL( dependencies, ETrue );
        
        // Recursively loop all the dependencies of the node
        for ( TInt i = 0; i < dependencies.Count(); ++i )
            {
            dependencies[ i ]
                ->UpdateDependencyDepthsL( 
                    Depth() + 1 );
            }

        CleanupStack::PopAndDestroy( &dependencies );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::UpdateDependencyDepthsL() end");
    }


// ---------------------------------------------------------------------------
// Protected functions
// 
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateNode::IsPurchasedL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateNode::IsPurchasedL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::IsPurchasedL() begin");
    
    TBool purchased( EFalse );
    
    MNcdNodePurchase* purchase( 
        Node().QueryInterfaceLC< MNcdNodePurchase >() );
    
    if ( purchase )
        {
        if( purchase->IsPurchased() )
            {
            purchased = ETrue;
            }

        CleanupStack::PopAndDestroy( purchase );
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::IsPurchasedL() end: %d", 
                     purchased);

    return purchased;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNode::IsDownloadedL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateNode::IsDownloadedL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::IsDownloadedL() begin");
    
    TBool downloaded( EFalse );
    
    MNcdNodeDownload* download( 
        Node().QueryInterfaceLC< MNcdNodeDownload >() );
    
    if ( download )
        {
        if( download->IsDownloadedL() )
            {
            downloaded = ETrue;            
            }

        CleanupStack::PopAndDestroy( download );
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNode::IsDownloadedL() end: %d", 
                     downloaded);

    return downloaded;
    }    


// ---------------------------------------------------------------------------
// CIAUpdateNode::IsInstalledL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateNode::IsInstalledL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::IsInstalledL() begin");

    TIAUpdateVersion installedVersion;

    TBool installed = EFalse;
        
    if ( Mime().Compare( IAUpdateProtocolConsts::KMimeWidget ) == 0 )
        {
        installed = IAUpdateUtils::IsWidgetInstalledL( Identifier(), installedVersion );
        }
    else 
        {
        installed = IAUpdateUtils::IsAppInstalledL( Uid(), installedVersion );
        }    
		
    IAUPDATE_TRACE_3("CIAUpdateNode::IsInstalledL() Installed version  %d.%d.%d", 
            installedVersion.iMajor, 
            installedVersion.iMinor, 
            installedVersion.iBuild );
    IAUPDATE_TRACE_3("CIAUpdateNode::IsInstalledL() Metadata version  %d.%d.%d", 
            Version().iMajor, 
            Version().iMinor, 
            Version().iBuild );
    if ( installed && installedVersion >= Version() )
        {
        // If the installed version is same or newer, then think the node as
        // installed.
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::IsInstalledL() end ETrue");
        return ETrue;
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::IsInstalledL() end EFalse");
        return EFalse;
        }
    }    


// ---------------------------------------------------------------------------
// CIAUpdateNode::OperationObserverL
// 
// ---------------------------------------------------------------------------
//
MIAUpdateNodeObserver* CIAUpdateNode::OperationObserver() const
    {
    return iOperationObserver;
    }


// ---------------------------------------------------------------------------
// Private functions
// 
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CIAUpdateNode::UpdateDependantLeafDistancesL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNode::UpdateDependantLeafDistancesL( TInt aLeafDistance )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::UpdateDependantLeafDistancesL() begin");
    
    // Notice, that by comparing depths here, we also make sure
    // that the leaf distance is increased correctly if we come to same
    // dependant node via multiple branches. Multiple dependants 
    // can depend on the same node.

    // Dependant leaf distance should be at least one greater than
    // dependency depth. 
    if ( aLeafDistance > LeafDistance() )
        {
        IAUPDATE_TRACE("[IAUPDATE] Update dependant distance");

        SetLeafDistance( aLeafDistance );

        // Recursively loop all the dependants of the node
        for ( TInt i = 0; i < DependantNodes().Count(); ++i )
            {
            DependantNodes()[ i ]
                ->UpdateDependantLeafDistancesL( 
                    LeafDistance() + 1 );
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNode::UpdateDependantLeafDistancesL() end");
    }
