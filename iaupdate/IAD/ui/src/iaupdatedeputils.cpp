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
* Description:   This module contains the implementation of IAUpdateDepUtils class 
*                member functions.
*
*/



// INCLUDES
#include "iaupdatedeputils.h"
#include "iaupdatebasenode.h"
#include "iaupdatenode.h"


// ---------------------------------------------------------------------------
// IAUpdateDepUtils::GetDependenciesL
// 
// ---------------------------------------------------------------------------
//
void IAUpdateDepUtils::GetDependenciesL( const MIAUpdateNode& aNode,
                                         const RPointerArray<MIAUpdateNode>& aAllNodes,  
                                         RPointerArray<MIAUpdateNode>& aDependencyNodes ) 
    {
    RPointerArray<MIAUpdateNode> dependencies;
    CleanupClosePushL( dependencies );
    // Include hidden nodes here. So, we can check the whole hierarchy
    // of dependencies. Maybe there are some hidden nodes in the middle of the
    // hierarchy. So, this way we can be sure that visible dependencies after
    // those hidden nodes are also counted here.
    // Notice, ownership of the nodes is not transferred here.
    aNode.GetDependenciesL( dependencies, ETrue );
    for ( TInt i = 0; i < dependencies.Count(); ++i )
        {
        MIAUpdateNode* dependencyNode( dependencies[ i ] );
        if ( dependencyNode != &aNode ) //skip if dependency is node itself
            {
            TInt nodeInd = aAllNodes.Find( dependencyNode );
            if ( nodeInd >= 0 ) // be sure that dependency is shown in UI
                {
                if ( !aAllNodes[ nodeInd ]->Base().IsSelected() )  
                    { // dependency is umarked 
                    if ( aDependencyNodes.Find( dependencyNode ) == KErrNotFound )//skip if dependency already in a list
                        {
                        aDependencyNodes.AppendL( dependencyNode );
                        GetDependenciesL( *dependencyNode, aAllNodes, aDependencyNodes );
                        }
                    }
                }
            else if ( dependencyNode->Base().Hidden() )
                {
                // Hidden nodes are not included into the list but
                // still their dependencies need to be checked.                
                // Notice, that some nodes in the middle of the 
                // dependency chain may be hidden. But, some of 
                // their dependencies may still be visible. So, 
                // those dependencies should be shown in the UI
                // in a normal way.
                GetDependenciesL( *dependencyNode, aAllNodes, aDependencyNodes );
                }
            }
        }
    CleanupStack::PopAndDestroy( &dependencies );
    }

// ---------------------------------------------------------------------------
// IAUpdateDepUtils::GetDependantsL
// 
// ---------------------------------------------------------------------------
//    
void IAUpdateDepUtils::GetDependantsL( const MIAUpdateNode& aNode, 
                                       const RPointerArray<MIAUpdateNode>& aAllNodes,  
                                       RPointerArray<MIAUpdateNode>& aDependantNodes ) 
    {
    RPointerArray<MIAUpdateNode> dependants;
    CleanupClosePushL( dependants );
    // Include hidden nodes here. So, we can check the whole hierarchy
    // of dependants. Maybe there are some hidden nodes in the middle of the
    // hierarchy. So, this way we can be sure that visible dependants after
    // those hidden nodes are also counted here.
    // Notice, ownership of the nodes is not transferred here.
    aNode.GetDependantsL( dependants, ETrue );
    for ( TInt i = 0; i < dependants.Count(); ++i )
        {
        MIAUpdateNode* dependantNode = dependants[ i ];
        if ( dependantNode != &aNode ) //skip if dependant is node itself
            {
            TInt nodeInd = aAllNodes.Find( dependantNode );
            if ( nodeInd >= 0 ) // be sure that dependant is shown in UI
                {
                if ( aAllNodes[ nodeInd ]->Base().IsSelected() )
                    { // dependant is marked 
                    if ( aDependantNodes.Find( dependantNode ) == KErrNotFound )//skip if dependant already in a list
                        {
                        aDependantNodes.AppendL( dependantNode );
                        GetDependantsL( *dependantNode, aAllNodes, aDependantNodes );
                        }
                    }
                }
            else if ( dependantNode->Base().Hidden() )
                {
                // Hidden nodes are not included into the list but
                // still their dependants need to be checked.                
                // Notice, that some nodes in the middle of the 
                // dependant chain may be hidden. But, some of 
                // their dependants may still be visible. So, 
                // those dependants should be shown in the UI
                // in a normal way.
                GetDependantsL( *dependantNode, aAllNodes, aDependantNodes );
                }
            }
        }
    CleanupStack::PopAndDestroy( &dependants );
    }
