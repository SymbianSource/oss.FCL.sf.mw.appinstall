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
* Description:   This module contains the description of CIAUpdateNodeFilter 
*                class member functions.
*
*/


#include <e32property.h>
#include <iaupdateparameters.h>
#include <centralrepository.h>

#include "iaupdatenodefilter.h"
#include "iaupdatebasenode.h"
#include "iaupdatenode.h"
#include "iaupdatefwnode.h"
#include "iaupdateutils.h"
#include "iaupdatedeputils.h"
#include "iaupdateuids.h"
#include "iaupdatedebug.h"
#include "iaupdatefwnsmlpskeys.h"
#include "iaupdatefwupdatehandler.h"
#include "iaupdateprivatecrkeys.h"

// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateNodeFilter* CIAUpdateNodeFilter::NewL()
    {
    CIAUpdateNodeFilter* self =
        CIAUpdateNodeFilter::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//    
CIAUpdateNodeFilter* CIAUpdateNodeFilter::NewLC()
    {
    CIAUpdateNodeFilter* self =
        new( ELeave ) CIAUpdateNodeFilter();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::~CIAUpdateNodeFilter
// Destructor
// -----------------------------------------------------------------------------
// 
CIAUpdateNodeFilter::~CIAUpdateNodeFilter()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::~CIAUpdateNodeFilter() begin");

    delete iFilterParams;
    iStoredNodes.Reset();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::~CIAUpdateNodeFilter() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::CIAUpdateNodeFilter
// c++ constructor
// -----------------------------------------------------------------------------
// 
CIAUpdateNodeFilter::CIAUpdateNodeFilter()
: CBase()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CIAUpdateNodeFilter()");
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeFilter::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::ConstructL()");    
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::SetFilterParams
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeFilter::SetFilterParams( CIAUpdateParameters* aParams )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::SetFilterParams() begin");

    delete iFilterParams;
    iFilterParams = aParams;    

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::SetFilterParams() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::FilterParams
// 
// -----------------------------------------------------------------------------
//    
CIAUpdateParameters* CIAUpdateNodeFilter::FilterParams() const
   {
   return iFilterParams; 	
   }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::FilterAndSortNodesL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeFilter::FilterAndSortNodesL( 
                          RPointerArray< MIAUpdateNode >& aNodes,
                          RPointerArray< MIAUpdateFwNode >& aFwNodes )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::FilterAndSortNodesL() begin");
    
    // Just to make sure that filter has the correct UID set.
    // Checks if the filter UID was SID and changes it to pUID if possible.

    TUid pUid( PackageUidL( aNodes ) );
    
    // Use bubble sort to sort the array
    TInt tmpIndex( aNodes.Count() );
    
    if ( tmpIndex == 1 )
        {
        IAUPDATE_TRACE("[IAUPDATE] Only one node.");
        // If the array has only one node, no need to do sorting. 
        // Instead, just check if the node needs to be marked or not. 
        MarkFilterNodeL( *aNodes[ 0 ], pUid );
        }
    else if ( tmpIndex > 1 )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Multiple nodes: %d", tmpIndex);
        // There are multiple nodes in the array.
        // So, sorting may be required.
        TBool swapped( EFalse );
        do
            {
            swapped = EFalse;
            --tmpIndex;
            for ( TInt i = 0; i < tmpIndex; ++i )
                {
                // Check if the aNodes[ i ] should be after aNodes[ i + 1 ]
                // in the array.
                if ( CompareAndMarkFilterNodesL( 
                        *aNodes[ i ], 
                        *aNodes[ i + 1 ],
                        pUid ) )
                    {
                    // Swap the nodes
                    MIAUpdateNode* tmpNode( aNodes[ i ] );
                    aNodes[ i ] = aNodes[ i + 1 ];
                    aNodes[ i + 1 ] = tmpNode;
                    swapped = ETrue;
                    }
                }        
            } while( swapped );
        }    
    // When dependant is marked, all dependencies to be marked also
    for ( TInt i = 0; i < aNodes.Count(); ++i )
        {
        MIAUpdateNode* node( aNodes[ i ] );
        if ( node->Base().IsSelected() )
            {
            SetDependenciesSelectedL( *node, aNodes );
            }
        }
    
    // Now update nodes were handled
    // Let's handle firmware updates
    if ( aFwNodes.Count() > 0  && DMSupportL() )
        {
        if ( iFilterParams )
            {
            if ( iFilterParams->Uid() != TUid::Null() ||
                 iFilterParams->SearchCriteria() != KNullDesC() )
                { //UID or search criteria given, firmware updates are not shown at all
                aFwNodes.Reset();
                }
            else
                {
                if ( aFwNodes.Count() == 1 && aFwNodes[0]->FwType() == MIAUpdateFwNode::EFotiNsu )
                    {
                    aFwNodes[0]->Base().SetSelected( EFalse );
                    }
                else
                    {
                    for ( TInt i = 0; i < aFwNodes.Count(); ++i )   
                        {
                        if ( aFwNodes[i]->FwType() == MIAUpdateFwNode::EFotiNsu )
                            {
                            aFwNodes[i]->Base().SetSelected( EFalse ); 
                            }
                        else
                            {
                            aFwNodes[i]->Base().SetSelected( ETrue );
                            }
                        }
                    for ( TInt i = 0; i < aNodes.Count(); ++i )   
                        {
                        aNodes[i]->Base().SetSelected( EFalse );
                        }
                    }
                }
            }
        else
            {
            if ( aFwNodes.Count() == 1 && aFwNodes[0]->FwType() == MIAUpdateFwNode::EFotiNsu )
                {
                aFwNodes[0]->Base().SetSelected( EFalse );
                }
            else
                {
                for ( TInt i = 0; i < aFwNodes.Count(); ++i )   
                    {
                    if ( aFwNodes[i]->FwType() == MIAUpdateFwNode::EFotiNsu )
                        {
                        aFwNodes[i]->Base().SetSelected( EFalse ); 
                        }
                    else
                        {
                        aFwNodes[i]->Base().SetSelected( ETrue );
                        }
                    }
                for ( TInt i = 0; i < aNodes.Count(); ++i )   
                    {
                    aNodes[i]->Base().SetSelected( EFalse );
                    }
                }
            }
        }
    else
        {
        aFwNodes.Reset();
        }
  

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::FilterAndSortNodesL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::CountOfAvailableUpdatesL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeFilter::CountOfAvailableUpdatesL( 
    const RPointerArray< MIAUpdateNode >& aNodes, 
    const RPointerArray< MIAUpdateFwNode >& aFwNodes, 
    TInt& aCount ) 
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() begin");
    
    aCount = 0; 

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() Node count (only MIAUpdateNodes): %d", aNodes.Count() );
    
    if ( iFilterParams->Type() & CIAUpdateParameters::ESis )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() ESis included in type parameter");
        if ( iFilterParams->Uid() == TUid::Null() 
             && iFilterParams->SearchCriteria() == KNullDesC() 
             && !FilteredByImportance() )
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() No filter values. Count all nodes.");
            // Because no filter values were given, 
            // all nodes should be counted.
            aCount = aNodes.Count();
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() Some filter values given.");
            // Some filter values were given.
            // So, filter nodes.        
            for ( TInt i = 0; i < aNodes.Count(); ++i )
                {
                if ( ImportanceMatches( *aNodes[ i ] ) )
                    {
                    if ( iFilterParams->Uid() == TUid::Null() && iFilterParams->SearchCriteria() == KNullDesC() )
                        {
                        ++aCount;
                        }
                    // If UID was given, filter by using UID.
                    // If UID was not given, filter by using search criteria.
                    else if ( iFilterParams->Uid() != TUid::Null() && iFilterParams->Uid() == aNodes[ i ]->Base().Uid()
                        || ( iFilterParams->Uid() == TUid::Null() 
                            && aNodes[ i ]->Base().SearchCriteria() != KNullDesC()
                            && SearchCriteriaMatches( iFilterParams->SearchCriteria(), aNodes[ i ]->Base().SearchCriteria() ) ) )
                        {
                        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() Filter parameters matched the node");
                        // Filter parameters matched the node.
                        ++aCount;
                        }
                    else if ( FilterCheckForServicePackL( *aNodes[ i ], 
                                                          iFilterParams->Uid(), 
                                                          iFilterParams->SearchCriteria() ) )
                        {
                        ++aCount;    
                        }
                    }
                }
                        
            
            if ( iFilterParams->Uid() != TUid::Null() && aCount == 0 )
                {
                IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() Uid did not match. Is it SID?");
                // Uid did not match to anything in the list.
                // Maybe it was not a package UID but SID.
                // So, check if we can find out the package UID and use it.
                TUid newUid( 
                       IAUpdateUtils::AppPackageUidL( iFilterParams->Uid() ) );
                IAUPDATE_TRACE_2("[IAUPDATE] [IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() NewUid: %d, Old uid: %d", 
                                     newUid.iUid, iFilterParams->Uid() );
                if ( newUid != TUid::Null() && newUid != iFilterParams->Uid() )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() New UID found."); 
                    // It was SID but now we know pUID.
                    // So, retry the search.
                    for ( TInt i = 0; i < aNodes.Count(); ++i )
                        {
                        if ( ImportanceMatches( *aNodes[ i ] ) )
                            {
                            if ( newUid == aNodes[ i ]->Base().Uid() )
                                {
                                IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() New UID matched.");
                                // New uid matched with the node uid.
                                ++aCount;
                                }
                            else if ( FilterCheckForServicePackL( *aNodes[ i ], 
                                     newUid, 
                                     iFilterParams->SearchCriteria() ) )
                                {
                                ++aCount;
                                }
                            }
                        }
                    }
                }
            }
        }
    
    
    
    // Firmware updates are not counted if UID or search criteria parameter exists 
    if ( iFilterParams->Uid() == TUid::Null() &&
         iFilterParams->SearchCriteria() == KNullDesC() )
        {
        TBool dmsupport = DMSupportL();
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() Checking DM support");
        //take firmware into account when dm support is available
        if ( dmsupport )
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() DM is supported");
            for ( TInt i = 0; i < aFwNodes.Count(); ++i )   
                {
                if ( aFwNodes[i]->FwType() == MIAUpdateFwNode::EFotaDp2 )
                    {
                    if ( iFilterParams->Type() & CIAUpdateParameters::EFota )
                        {
                        ++aCount;
                        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() FOTA node found");
                        }
                    }
                else if ( aFwNodes[i]->FwType() == MIAUpdateFwNode::EFotiNsu )  
                    {
                    if ( iFilterParams->Type() & CIAUpdateParameters::ENsu )
                         {
                         ++aCount;
                         IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() NSU node found");
                         }       
                    }
                }
            }
        }
      
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeFilter::CountOfAvailableUpdatesL() end count:  %d",
                     aCount );
    }

// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::SetDependenciesSelectedL();
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeFilter::SetDependenciesSelectedL( MIAUpdateNode& aNode, 
                                 const RPointerArray<MIAUpdateNode>& aAllNodes ) 
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::SetDependenciesSelectedL() begin"); 
    RPointerArray<MIAUpdateNode> deps;
    CleanupClosePushL( deps );
    
        
    IAUpdateDepUtils::GetDependenciesL( aNode, aAllNodes, deps );    

    TInt depCount = deps.Count();
    for( TInt i = 0; i < depCount; i++ )
        {
        TInt depNodeInd = aAllNodes.Find( deps[i] );
        aAllNodes[depNodeInd]->Base().SetSelected( ETrue );
        }
    CleanupStack::PopAndDestroy( &deps );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::SetDependenciesSelectedL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::StoreNodeListL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeFilter::StoreNodeListL( const RPointerArray< MIAUpdateNode >& aNodes )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::StoreNodeListL() begin");
    iStoredNodes.Reset();
    for( TInt i = 0; i < aNodes.Count(); ++i )
        {
        iStoredNodes.AppendL( aNodes[i] ); 
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::StoreNodeListL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::RestoreNodeListL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeFilter::RestoreNodeListL( RPointerArray< MIAUpdateNode >& aNodes) const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::RestoreNodeListL() begin");   
    aNodes.Reset();
    for( TInt i = 0; i < iStoredNodes.Count(); ++i )
        {
        aNodes.AppendL( iStoredNodes[i] ); 
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::RestoreNodeListL() end");   
    }

// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::SortSelectedNodesFirstL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeFilter::SortSelectedNodesFirstL( 
                          const RPointerArray<MIAUpdateNode>& aSelectedNodes, 
                          RPointerArray< MIAUpdateNode >& aNodes )
    {
    for ( TInt i = aNodes.Count() - 1 ; i >= 0 ; --i )
        {
        MIAUpdateNode* node( aNodes[ i ] );
        if ( node->Base().IsSelected() )
            {
            aNodes.Remove( i );        
            }
        }
    for ( TInt j = aSelectedNodes.Count() -1 ; j >= 0 ; --j )
        {
        aNodes.InsertL( aSelectedNodes[j], 0 ); 
        }
    }
 
// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::SortThisNodeFirstL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateNodeFilter::SortThisNodeFirstL( const MIAUpdateNode* aFirstNode, 
                                              RPointerArray< MIAUpdateNode >& aNodes)
    {
    TBool removed = EFalse; 
    for ( TInt i = aNodes.Count() - 1 ; i >= 0 && !removed ; --i )
        {
        if ( aNodes[ i ] == aFirstNode )
            {
            aNodes.Remove( i );      
            removed = ETrue;
            }
        }
    aNodes.InsertL( aFirstNode, 0 );
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::CompareAndMarkFilterNodesL
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateNodeFilter::CompareAndMarkFilterNodesL( 
    MIAUpdateNode& aNode1, 
    MIAUpdateNode& aNode2,
    const TUid& aPackageUid )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CompareAndMarkFilterNodesL() begin");

    TBool filterCheck1( MarkFilterNodeL( aNode1, aPackageUid ) );
    TBool filterCheck2( MarkFilterNodeL( aNode2, aPackageUid ) );
    
    // 1. The one node that matches filter should be first.
    // 2. More ciritical should be first.
    // 3. Alphabetical order inside critical sets.
    if ( filterCheck1 && !filterCheck2 )
        {
        IAUPDATE_TRACE("[IAUPDATE] Only node 1 marked for filter.");
        // The Node1 should be before Node2 
        // if Node1 matches the filter check 
        // but Node2 does not.
        // Then, the importance or name does not have effect.
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CompareAndMarkFilterNodesL() end: EFalse");
        return EFalse;
        }
    else if ( !filterCheck1 && filterCheck2  
              || aNode1.Base().Importance() < aNode2.Base().Importance()
              || ( aNode1.Base().Importance() == aNode2.Base().Importance()
                   && aNode1.Base().Name().CompareC( aNode2.Base().Name() ) > 0 ) ) 
        {
        // Node2 should be before Node1.
        // Notice, that when comparing the node names, the comparison is thought
        // case-insensitive. Also, CompareC is used instead of CompareF because CompareC
        // uses collation method for narrow text appropriate to the current locale. CompareF
        // would be locale-independent behaviour.
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CompareAndMarkFilterNodesL() end: ETrue");
        return ETrue;
        }
    else
        {
        // Node1 should be before Node2.
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::CompareAndMarkFilterNodesL() end: EFalse");
        return EFalse;
        }
    } 


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::MarkFilterNodeL
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateNodeFilter::MarkFilterNodeL( 
    MIAUpdateNode& aNode,
    const TUid& aPackageUid )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::MarkFilterNodeL() begin");
    
    // This describes if the node matches the filter parameter.
    TBool filterCheck( EFalse );
    
    if ( iFilterParams )
        {
        // Notice, that when filtering was started the UID check was also done.
        // So, if the SID was given and it did not result to any matches,
        // then the filter UID was changed to corresponding package UID if
        // it was possible.
        
        // Notice, that if UID is given then use it for filtering.
        // If UID is not given, then use search criteria if it is given.
        if ( aPackageUid != TUid::Null() )
            {
            // Because uid was given, filter by using that value
            if ( aPackageUid == aNode.Base().Uid() )
                {
                // Because this matches the filter, set the node as selected
                aNode.Base().SetSelected( ETrue );
                filterCheck = ETrue;
                }
            else if ( FilterCheckForServicePackL( aNode, aPackageUid, iFilterParams->SearchCriteria() ) )
                {
                aNode.Base().SetSelected( ETrue );
                filterCheck = ETrue;    
                }
            }
        else if ( iFilterParams->SearchCriteria() != KNullDesC() )
            {
            // Because search criteria was given, use it for filtering
            if ( SearchCriteriaMatches( iFilterParams->SearchCriteria(), aNode.Base().SearchCriteria() ) )
                {
                // Because this matches the filter, set the node as selected
                aNode.Base().SetSelected( ETrue );
                filterCheck = ETrue;
                }
            else if ( FilterCheckForServicePackL( aNode, aPackageUid, iFilterParams->SearchCriteria() ) )
                {
                aNode.Base().SetSelected( ETrue );
                filterCheck = ETrue;    
                }
            }
        
        // If the node did not match the filter parameter, it should not be selected.
        // unless it is a mandatory node
        if ( !filterCheck )
            {
            if ( aNode.Base().Importance() == MIAUpdateBaseNode::EMandatory )
                {
                //if the node importance is mandatory, then select it eventhough it doesn't match
                //the filter 
                
                //here only handle the case of normal packages. For the case of firmware available, it will be
                //handled in ui controller
                aNode.Base().SetSelected( ETrue );
                }
            else
                {
                // Set node as not selected.
                // Just in case the filter has changed but node has remained, 
                // since last time.
                
                aNode.Base().SetSelected( EFalse );
                }
            }
        }
    else
        {
        // Filtering is not on. Set as selected unless importance is ENormal.
        if ( aNode.Base().Importance() == MIAUpdateBaseNode::ENormal )
            {
            aNode.Base().SetSelected( EFalse );
            }
        else
            {
            aNode.Base().SetSelected( ETrue );
            }
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeFilter::MarkFilterNodeL() end: %d",
                     filterCheck);

    return filterCheck;
    } 

// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::FilteredByImportance
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateNodeFilter::FilteredByImportance() const
    {
	return !( iFilterParams->Importance() & CIAUpdateParameters::ENormal &&
	          iFilterParams->Importance() & CIAUpdateParameters::ERecommended &&
	          iFilterParams->Importance() & CIAUpdateParameters::ECritical &&
	          iFilterParams->Importance() & CIAUpdateParameters::EMandatory &&
	          iFilterParams->Importance() & CIAUpdateParameters::EHidden);
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::ImportanceMatches
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateNodeFilter::ImportanceMatches( MIAUpdateNode& aNode ) const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::ImportanceMatches() begin");
    TBool matches( EFalse);
    if ( aNode.NodeType() == MIAUpdateAnyNode::ENodeTypeNormal && iFilterParams )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeFilter::ImportanceMatches() importance: %d", aNode.Base().Importance() );
        switch( aNode.Base().Importance() )
            {
            case MIAUpdateBaseNode::ENormal:
                if ( iFilterParams->Importance() & CIAUpdateParameters::ENormal )
                    {
                   	matches = ETrue;
                    }
                break;
            case MIAUpdateBaseNode::ERecommended:
                if ( iFilterParams->Importance() & CIAUpdateParameters::ERecommended )
                    {
                   	matches = ETrue;
                    }
                break;
            case MIAUpdateBaseNode::ECritical:
                if ( iFilterParams->Importance() & CIAUpdateParameters::ECritical )
                    {
                   	matches = ETrue;
                    }
                break;
            case MIAUpdateBaseNode::EMandatory:
                if ( iFilterParams->Importance() & CIAUpdateParameters::EMandatory )
                    {
                   	matches = ETrue;
                    }
                break;
            case MIAUpdateBaseNode::EHidden:
                if ( iFilterParams->Importance() & CIAUpdateParameters::EHidden )
                    {
                   	matches = ETrue;
                    }
                break;    
            default: 
                break;
            }
        }
    else
        {
        // firmware updates are not filtered by importance
        matches = ETrue;	
        }
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeFilter::ImportanceMatches() end, matches: %d", matches);
    return matches;
    }

// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::DMSupportL();
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateNodeFilter::DMSupportL() 
    {
    if ( !iDmSupportAsked )
        {
        TInt err = RProperty::Get( KUidSystemCategory, KIAUpdateUiUid, iDmSupport );
        
        if ( err != KErrNone )
            {
            if ( err == KErrNotFound )
                {
                IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::DMSupportL() DM support is not defined in P&S");
                
                CIAUpdateFWUpdateHandler* fw = CIAUpdateFWUpdateHandler::NewL();
                CleanupStack::PushL( fw );
                TBool isDMSupported = fw->IsDMSupportAvailableL();
                
                if ( isDMSupported  )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::DMSupportL() DM is supported");
                    }
                
                TBool FWEnable = EFalse;
                CRepository* cenrep = CRepository::NewL( KCRUidIAUpdateSettings ); 
                CleanupStack::PushL( cenrep );
                err = cenrep->Get( KIAUpdateEnableFWFeature, FWEnable );
    
                if ( err != KErrNone )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::DMSupportL() Cenrep failed");
                    }
                 
                if ( FWEnable )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::DMSupportL() firmware feature enabled");
                    }
                else
                    {
                    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::DMSupportL() firmware feature disabled");
                    isDMSupported = EFalse;
                    }

                CleanupStack::PopAndDestroy( 2 ); //fwhandle, cenrep                
                
                TSecureId secureid( KIAUpdateUiUid );
                const TSecurityPolicy readPolicy (secureid);
                const TSecurityPolicy writePolicy (secureid);
                
                // define first property to be integer type
                err = RProperty::Define( KUidSystemCategory, KIAUpdateUiUid, RProperty::EInt, readPolicy, writePolicy  );
                 
                IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeFilter::DMSupportL() define P&S err = %d", err );
             
                err =  RProperty::Set( KUidSystemCategory, KIAUpdateUiUid, isDMSupported );
                IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeFilter::DMSupportL() Set P&S err = %d", err );
                
                iDmSupport = isDMSupported;
                }
            else
                {
                iDmSupport = EFalse;
                }
            }
        
        iDmSupportAsked = ETrue;
        }
    return iDmSupport;
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::PackageUidL
// 
// -----------------------------------------------------------------------------
//
TUid CIAUpdateNodeFilter::PackageUidL( 
    const RPointerArray< MIAUpdateNode >& aNodes ) 
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL() begin");
        
    TUid retUid( TUid::Null() );
    
    if ( !iFilterParams )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL() end. No filter set. Return null uid.");
        return retUid;
        }
    
    if ( iFilterParams->Uid() == TUid::Null() )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL() end. Parameter UID set is to null");
        return iFilterParams->Uid();
        }
    
    retUid = iFilterParams->Uid();  
      
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL() Parameter UID value given.");

    // UID value was given.
    // So, filter nodes.     
    TBool uidMatches = EFalse;
    for ( TInt i = 0; i < aNodes.Count() && !uidMatches ; ++i )
        {
        if ( ImportanceMatches( *aNodes[ i ] ) )
            {
            // If UID was given, filter by using UID.
            // If UID was not given, filter by using search criteria.
            
            if ( retUid == aNodes[ i ]->Base().Uid() )
                {
                IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL() Filter parameter UID matched the node");
                uidMatches = ETrue;
                }
            }
        }

    if ( !uidMatches )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL() Uid did not match. Is it SID?");
        // Uid did not match to anything in the list.
        // Maybe it was not a package UID but SID.
        // So, check if we can find out the package UID and use it.
        TUid newUid( 
            IAUpdateUtils::AppPackageUidL( retUid ) );
        IAUPDATE_TRACE_2("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL() NewUid: %d, Old uid: %d", 
                         newUid.iUid, retUid.iUid);
        if ( newUid != TUid::Null() && newUid != retUid )
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL() New UID found."); 
            // It was SID but now we know pUID.
            // So, retry the search.
            for ( TInt i = 0; i < aNodes.Count() && !uidMatches ; ++i )
                {
                if ( newUid == aNodes[ i ]->Base().Uid() && ImportanceMatches( *aNodes[ i ] ) )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL() New UID matched.");
                    // New uid matched with the node uid.
                    uidMatches = ETrue;
                    }
                }

            if ( uidMatches )
                {
                IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL() Start using new pUID.");
                // The new uid actually found something. So, the check mattered.
                // Update the retUid. So, others can use the packageUid instead of
                // SID.
                // Notice, do not update the filter params here. The old value maybe
                // used for some comparisons outside of this class. So, it should not
                // be changed.
                // Note, that FilterAndSortNodes function calls this function to get the
                // correct uid for filter. So, this setting is required for that.
                retUid = newUid;
                }
            }
        }
          
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeFilter::PackageUidL( end UID: %d", retUid);

    return retUid;
    }


// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::FilterCheckForServicePackL
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateNodeFilter::FilterCheckForServicePackL( const MIAUpdateNode& aNode, 
                                                       const TUid& aPUid,
                                                       const TDesC& aSearchCriteria )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::FilterCheckForServicePackL() begin");
    TBool filterCheck = EFalse;    
    if ( aNode.Type() == MIAUpdateNode::EPackageTypeServicePack )
        {
        RPointerArray<MIAUpdateNode> dependencies;
        CleanupClosePushL( dependencies );
        aNode.GetDependenciesL( dependencies, ETrue );
        for ( TInt i = 0; i < dependencies.Count() && !filterCheck; ++i )
            {
            if ( !dependencies[ i ]->IsInstalled() )
                {
                if ( aPUid != TUid::Null() )
                    {
                    if ( aPUid == dependencies[ i ]->Base().Uid() )
                        {
                        filterCheck = ETrue;
                        }
                    }
                else if ( aSearchCriteria != KNullDesC() )
                    {
                    if ( SearchCriteriaMatches( aSearchCriteria, dependencies[ i ]->Base().SearchCriteria() ) )
                        {
                        filterCheck = ETrue;
                        }
                    }
                }
            
            CleanupStack::PopAndDestroy( &dependencies );
            }
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeFilter::FilterCheckForServicePackL() end");
    return filterCheck;  
    }

// -----------------------------------------------------------------------------
// CIAUpdateNodeFilter::SearchCriteriaMatches
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateNodeFilter::SearchCriteriaMatches( const TDesC& aParamSearchCriteria, 
                                                  const TDesC& aNodeSearchCriteria ) const
    {
    TBool matches = EFalse;
    if ( aNodeSearchCriteria.Length() >= aParamSearchCriteria.Length() )
        {
        matches = aParamSearchCriteria == aNodeSearchCriteria.Mid( 0, aParamSearchCriteria.Length() );  
        }
    return matches;
    }
