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



#include "iaupdatenodecontainer.h"

#include "iaupdatecontrollerimpl.h"
#include "iaupdatecontentoperationmanager.h"
#include "iaupdatenodeimpl.h"
#include "iaupdatefwnodeimpl.h"
#include "iaupdatenodedetails.h"
#include "iaupdatenodedependencyimpl.h"
#include "iaupdateplatformdependency.h"
#include "iaupdateutils.h"
#include "iaupdatenodefactory.h"

#include "iaupdatedebug.h"


#include "ncdnodecontentinfo.h"
#include "ncdnode.h"
#include "iaupdateprotocolconsts.h"


// Constant that is used to inform that dependency node
// was not found when dependencies were checked.
const TInt KDependencyNotFound( -1 );


CIAUpdateNodeContainer* CIAUpdateNodeContainer::NewLC(
    CIAUpdateController& aController )
    {
    CIAUpdateNodeContainer *self = 
        new (ELeave) CIAUpdateNodeContainer( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
CIAUpdateNodeContainer* CIAUpdateNodeContainer::NewL(
    CIAUpdateController& aController )
    {    
    CIAUpdateNodeContainer *self = 
        CIAUpdateNodeContainer::NewLC( aController );
    CleanupStack::Pop( self );
    return self;
    }

    
CIAUpdateNodeContainer::CIAUpdateNodeContainer(
    CIAUpdateController& aController )
: CBase(),
  iController( aController )
    {
    }

void CIAUpdateNodeContainer::ConstructL()
    {
    }

        
CIAUpdateNodeContainer::~CIAUpdateNodeContainer()
    {
    Clear();
    }    
    
    
void CIAUpdateNodeContainer::AddNodeL( CIAUpdateNode* aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::AddNodeL() begin");
    
    if ( !aNode )
        {
        // Null is not acceptable
        IAUPDATE_TRACE("[IAUPDATE] LEAVE: Node null");
        User::Leave( KErrArgument );
        }

    CleanupStack::PushL( aNode );

    IAUPDATE_TRACE("[IAUPDATE] Check if node is acceptable");
    if ( !NodeAlreadyExists( *aNode )
         && aNode->Details().PlatformDependency().AcceptablePlatformL()
         && InstallCheckL( *aNode ) 
         && !aNode->Details().EmbededDegrades()
         && PackageTypeAcceptedL( *aNode ) )
        {
        IAUPDATE_TRACE("[IAUPDATE] Node accepted. Add into the node array.");

        // The node list will contain nodes 
        // that can be downloaded and installed.
        // The header node list will provide 
        // most recent version of the content.

        // The ownership of the node transfers.
        // So try to append it into the array.
        // If node can not be added into the array, 
        // it will be deleted when appending leaves.
        iNodes.AppendL( aNode );
        // Appending was successfull. 
        // So, ownership has been transferred successfully.
        CleanupStack::Pop( aNode );
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] Node not accepted. Delete it.");
        
        // Because the node is not deployable 
        // or it was for the wrong platform, 
        // there is no use for it. Delete the node.
        CleanupStack::PopAndDestroy( aNode );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::AddNodeL() end");
    }


void CIAUpdateNodeContainer::AddExcessNodeL( CIAUpdateNode* aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::AddExcessNodeL() begin");
    
    if ( aNode == NULL )
        {
        // Null is not acceptable
        IAUPDATE_TRACE("[IAUPDATE] LEAVE: NULL node");
        User::Leave( KErrArgument );
        }

    // The node list will contain nodes that do not belong to any other
    // node category.
     
    // The ownership of the node transfers.
    // So try to append it into the array.
    CleanupStack::PushL( aNode );
    iExcessNodes.AppendL( aNode );
    CleanupStack::Pop( aNode );

    // Force the node to be hidden.
    aNode->ForceHidden( ETrue );
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::AddExcessNodeL() end");
    }


void CIAUpdateNodeContainer::AddFwNodeL( CIAUpdateFwNode* aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::AddFwNodeL() begin");

    if ( !aNode )
        {
        // Null is not acceptable
        IAUPDATE_TRACE("[IAUPDATE] LEAVE: Node null");
        User::Leave( KErrArgument );
        }

    CleanupStack::PushL( aNode );

    iFwNodes.AppendL( aNode );

    CleanupStack::Pop( aNode );
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::AddFwNodeL() end");
    }


void CIAUpdateNodeContainer::Clear()
    {
    // Just reset this list because all the nodes 
    // are owned by the iNodes list.
    iHeadNodes.Reset();
    
    // Reset the list and delete the nodes.
    iNodes.ResetAndDestroy();

    // Reset the list and delete the nodes.    
    iExcessNodes.ResetAndDestroy();

    // Reset the list and delete the nodes.
    iFwNodes.ResetAndDestroy();
    }


const RPointerArray< CIAUpdateNode >& CIAUpdateNodeContainer::AllNodes() const
    {
    return iNodes;
    }


const RPointerArray< CIAUpdateNode >& CIAUpdateNodeContainer::ExcessNodes() const
    {
    return iExcessNodes;
    }
    
    
const RPointerArray< CIAUpdateNode >& CIAUpdateNodeContainer::HeadNodesL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::HeadNodesL() begin");
    
    // Make sure that dependencies are correct. 
    // Notice, that here the dependencies are checked for all the nodes 
    // in the node list. So, some dependencies that may not be required 
    // are also checked. (Unnecessary dependency checks can exist, for example, 
    // if the server sends multiple update packets for a same content version.)
    // But, checking of dependencies first makes it easier to find correct header 
    // nodes later because we can be sure that all the nodes have intact dependency 
    // chains then. Also, notice that this function will set the information about 
    // the dependency checks into the nodes and remove nodes, whose dependencies
    // are broken, from the list.
    UpdateDependenciesL();

    // Create the header node list and remove nodes 
    // whose dependency chains are broken.
    ManageHeaderNodesL();

    // Create the dependecy for self update nodes 
    // and mark selfupdater and ncd hidden.
    // RemoveHiddenNodesFromHeadList will handle those nodes 
    // after they have been marked hidden.
    CreateSelfUpdateBundleL();

    // This will set the service pack dependency nodes as hidden.
    HandleServicePacksL();

    // Bundles may contain nodes that are head nodes
    // but have been marked hidden.
    // So, remove hidden nodes from the head list. 
    // Hidden nodes should be visible to UI.
    RemoveHiddenNodesFromHeadList();
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::HeadNodesL() end");
    
    return iHeadNodes;
    }


const RPointerArray< CIAUpdateFwNode >& CIAUpdateNodeContainer::FwNodes()
    {
    return iFwNodes;    
    }


void CIAUpdateNodeContainer::UpdateDependenciesL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::UpdateDependenciesL() begin");

    // Reset node statuses
    for ( TInt i = 0; i < iNodes.Count(); ++i )
        {
        iNodes[ i ]->Reset();
        }
    
    // Check and update the dependencies for all the nodes.
    // So, it will be checked that all the nodes can be installed correctly
    // and no dependencies for them are missing.
    for ( TInt i = 0; i < iNodes.Count(); ++i )
        {
        // If the node dependency status has already been set, 
        // then it was already checked in some previous loop 
        // or recursion of dependency check. So, do not recheck 
        // the same branch here. Notice, the DependencyCheckStatus is 
        // also checked in the recursion in UpdateNodeDependenciesL. 
        // So, recheck should not even be allowed here. Otherwise checks
        // will fail unless the statuses are resetted before.
        // If the state has not been set, then check the branch.
        CIAUpdateNode& node( *iNodes[ i ] );
        if ( node.DependencyCheckStatus() 
             == CIAUpdateNode::EDependencyCheckNotSet )
            {
            // Notice, that this function sets the dependency check status 
            // for the node and to the nodes belonging to that branch in 
            // its dependency tree.
            UpdateNodeDependenciesL( node, 0 );
            }        
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::UpdateDependenciesL() end");
    }

    
TInt CIAUpdateNodeContainer::UpdateNodeDependenciesL( 
    CIAUpdateNode& aNode, TInt aDepth )
    {
    IAUPDATE_TRACE_2("[IAUPDATE] CIAUpdateNodeContainer::UpdateNodeDependenciesL() begin: %S, %S",
                     &aNode.MetaNamespace(), &aNode.MetaId());

    // Notice that this function creates a recursive loop together with 
    // FindBestMatchL function.

    // Check if we have already handled this node and dependency loops back to it.
    // If node check is already going on for the node, then there is loop in dependency.
    // Do not accept loops for dependencies. If no loop, then continue normally. 
    // Notice, that if we loop here, other sub branches may have not been
    // checked for this node yet. So, only set the status when the whole subtree has
    // been checked.

    if ( aNode.DependencyCheckStatus() 
         == CIAUpdateNode::EDependencyCheckNotSet )
        {
        IAUPDATE_TRACE("[IAUPDATE] Dependency check not set yet");

        // Gives the total leaf distance of this part of the dependency tree. 
        // The tallest branch gives the total leaf distance.    
        TInt totalLeafDistance( KDependencyNotFound );

        // Mark the check state here to prevent loops.
        aNode.SetDependencyCheckStatus( CIAUpdateNode::EDependencyCheckGoing );
        
        RPointerArray< CIAUpdateNodeDependency > deps;
        CleanupClosePushL( deps );
                
        // Get dependency objects from aNode into the deps array.
        // Get dependencies that were gotten from the server.
        aNode.Details().GetDependenciesL( deps );

        // Now, that we have the dependency information for the given node,
        // find the nodes that provide the best dependency branches.            
        for ( TInt i = 0; i < deps.Count(); ++i )
            {
            // The best branch is thought to be the branch that is the flattest.
            // FindBestMatchL function inserts the best match information 
            // to the dependency object. The best match information of the dependency 
            // will contain the possible node whose content is required 
            // for the dependency chain to be complete. Also, that node may need other nodes. 
            // So, the dependency branch may continue there.
            // Dependency depth is one greater than the depth of dependant aNode.
            TInt leafDistance( 
                FindBestMatchL( aNode, *deps[ i ], aDepth + 1 ) );

            // Notice, that here we are looking for the total leaf distance for this branch.
            // FindBestMatchL -function that is used above will use this value to check 
            // what branch has the shortes totalLeafDistance. 
            // The branch that is flattest will be chosen for the current parent node.
            if ( leafDistance == KDependencyNotFound )
                {
                IAUPDATE_TRACE("[IAUPDATE] Dependency check failed");

                // Because dependency was not found from the node list or from 
                // the installed applications, the node dependency chain is broken.
                // Set this information into the node.
                // So, this node will be removed from the node list later.
                aNode.SetDependencyCheckStatus( CIAUpdateNode::EDependencyCheckFailed );
                    
                // Because, one missing item is enough to break the chain, just return here.
                // If other nodes are required, they are checked separately later in their own
                // checkings. The chain break affects all the nodes that depend on this one. So,
                // this information will be set to the nodes, when the recursion goes back.
                CleanupStack::PopAndDestroy( &deps );

                IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::UpdateNodeDependenciesL() end: KDependencyNotFound");

                return KDependencyNotFound;            
                }
             else if ( totalLeafDistance < leafDistance )
                {
                IAUPDATE_TRACE_2("[IAUPDATE] Replace old totalLeafDistance %d, %d",
                                 totalLeafDistance, leafDistance);
                // Set the total leaf distance of this part of the dependency chain because
                // the chain is intact.
                totalLeafDistance = leafDistance;
                }            
            }
            
        CleanupStack::PopAndDestroy( &deps );

        // Dependency chain for the node is intact.
        // Set this information into the node.    
        aNode.SetDependencyCheckStatus( CIAUpdateNode::EDependencyCheckPassed );
        IAUPDATE_TRACE("[IAUPDATE] EDependencyCheckPassed");

        // Add one, so the parent will get its own actual leaf distance in return.
        ++totalLeafDistance;
        
        // Also, set the total leaf distance for the node.
        aNode.SetLeafDistance( totalLeafDistance );


        IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeContainer::UpdateNodeDependenciesL() end: %d",
                         totalLeafDistance);
        
        return totalLeafDistance;    
        }
    else if ( aNode.DependencyCheckStatus()
              == CIAUpdateNode::EDependencyCheckPassed )
        {
        IAUPDATE_TRACE("[IAUPDATE] Dependency check already passed");
        
        // Because this node and the hierarchy below it has already been handled,
        // check if the depth of the node and its dependency hierarchy should be
        // updated with the new value. 
        aNode.UpdateDependencyDepthsL( aDepth );

        // Because we have already handled this branch successfully,
        // just return its own leaf distance information.
        IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeContainer::UpdateNodeDependenciesL() end: %d",
                         aNode.LeafDistance());
        return aNode.LeafDistance();
        }
    else if ( aNode.DependencyCheckStatus()
              == CIAUpdateNode::EDependencyCheckGoing )
        {
        IAUPDATE_TRACE("[IAUPDATE] Dependency loop has occurred");
        // Notice, we come here if there are loops in dependencies.
        // Looping dependency chains are not acceptable in normal cases.
        // Notice, in case of service packs, if another branch of service pack
        // is intact and it does not loop, then the service pack check status 
        // is set to CIAUpdateNode::EDependencyCheckPassed when the recursion 
        // is over above and all the branches of the service pack have been handled. 
        // Still loopin branches inside service pack will also be removed.
        aNode.SetDependencyCheckStatus( CIAUpdateNode::EDependencyCheckFailed );
        }

    // We come here if dependency check status has been set to 
    // CIAUpdateNode::EDependencyCheckFailed. So, then always 
    // return KDependencyNotFound to correspond that situation.

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::UpdateNodeDependenciesL() end: KDependencyNotFound");

    return KDependencyNotFound;
    }


TInt CIAUpdateNodeContainer::FindBestMatchL( CIAUpdateNode& aNode,
                                             CIAUpdateNodeDependency& aDependency,
                                             TInt aDependencyDepth )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::FindBestMatchL() begin");

    // Notice that this function creates a recursive loop together with 
    // UpdateNodeDependenciesL function.

    // As a default, the dependency we are looking for is marked as not found.
    TInt totalLeafDistance( KDependencyNotFound );

    // Check if the dependency file has already been installed and get the version
    // of the application if it has been installed.
    TIAUpdateVersion installedVersion;
    TBool installed( 
        IAUpdateUtils::IsAppInstalledL( 
            aDependency.Uid(), installedVersion ) );
    
    if ( aDependency.IsEmbedded() )
        {
        IAUPDATE_TRACE("[IAUPDATE] Dependency is embedded in the node.");
        // If dependency node is already embedded into the package, then use it
        // instead of downloading it separately. Embedded dependencies are always
        // matched by the node itself. Embedded package has to be installed 
        // when content is installed, or the whole installation fails.
        // Best match for this dependency is the node content itself. But, 
        // to prevent loops, set the best match to NULL.
        aDependency.SetBestMatch( NULL );

        // If the embedded node will require downgrades, 
        // then do not accept this dependency. Notice, here we think 
        // that the embedded content version value is given as version roof value.
        // So, the installed version has to be equal or smaller than the roof value,
        // if the embedded content can be accepted.
        // Note for future improvements: 
        // When support is added for embedded sisx to have dependencies, 
        // this needs to be changed accordingly
        if ( !installed
             || installedVersion <= aDependency.VersionRoof() )
            {
            IAUPDATE_TRACE("[IAUPDATE] Embedded content can be installed");
            // Leaf distance is zero because embedded item is already part
            // of the node itself.
            totalLeafDistance = 0;           
            }
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] Dependency content is not embedded.");

        // Check here if the best matching content is already installed or if
        // content needs to be downloaded and installed

        // Note, that the dependency may also describe an update dependency
        // for the content instead of just describing that a content depends 
        // on another content. But, we can handle it all these cases in a same
        // way.

        // This value will be updated when the recursive call of 
        // UpdateNodeDependenciesL inside for-loop has found an acceptable leaf node. 
        // Then, recursion will come back of the tree hierarchy and find other branches 
        // and update this value if necessary.
        CIAUpdateNode* currentBestMatch( NULL );

        RPointerArray< CIAUpdateNode > matches;
        CleanupClosePushL( matches );

        // Find the nodes that fullfill the given dependency requirement.
        // Node list may contain multiple alternatives. 
        // So, first find the alternatives.
        // Notice, that this function skips already installed items.
        FindMatchesL( matches, aDependency );

        // Find the best match from the alternatives.
        for ( TInt i = 0; i < matches.Count(); ++i )
            {
            CIAUpdateNode& tmpNode( *matches[ i ] );

            // The node that matches can be part of the best dependency branch.
            // But, we still need to check the whole branch before we can be sure. 
            // So, check the dependency branch forward by using the recursion.
            TInt leafDistance( 
                UpdateNodeDependenciesL( tmpNode, aDependencyDepth ) );

            // If no match has been handled so far, then the new one is best match.
            // If there is already a current best match, then check if the
            // new one should replace it.
            if ( leafDistance != KDependencyNotFound 
                 && ( totalLeafDistance == KDependencyNotFound 
                      || currentBestMatch 
                         && ReplaceRecommendedL( 
                                *currentBestMatch, tmpNode ) ) )
                {
                IAUPDATE_TRACE("[IAUPDATE] New best match found.");
                currentBestMatch = &tmpNode;
                totalLeafDistance = leafDistance;
                }
            }
        
        CleanupStack::PopAndDestroy( &matches );

        // Set the best match information for the dependency.
        // Notice, that if installedVersion equals the currentBestMatch version,
        // the node is set instead of NULL value. This way if a content in the
        // middle of the dependency chain is installed, the dependency way will
        // not be cut in that place.
        if ( installed
             && installedVersion >= aDependency.VersionFloor()
             && installedVersion <= aDependency.VersionRoof()
             && ( !currentBestMatch 
                  || installedVersion > currentBestMatch->Version() ) )
            {
            IAUPDATE_TRACE("[IAUPDATE] Best dependency content is already installed. Use it.");
            // The content was already installed and its version belonged 
            // to the required version range. Also, the installed version 
            // was greater than the version of other matches.
            // The best match was found, but it requires no nodes and their content.
            aDependency.SetBestMatch( NULL );
            totalLeafDistance = 0;
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] Use current best match item if available");
            // If currentBestMatch is not NULL, the best dependency node was found.
            // If currentBestMatch is NULL, the best dependency node was not found.
            // So, just set the best match information accordingly.
            aDependency.SetBestMatch( currentBestMatch );
            if ( currentBestMatch )
                {
                IAUPDATE_TRACE("[IAUPDATE] Current best match item is available");

                // Notice, that UpdateNodeDependenciesL checks and prevents looping.
                // So, no need to do it here. AddDependant now, that we found the
                // best match. Notice, we only want to insert dependant information
                // to the correct item not to possible choices. So, that is why we
                // do it here, not inside the for-loop above.
                currentBestMatch->AddDependantL( aNode );

                // Because node depth has not been set yet, update it now.
                // Notice, we update the depth here because then the depth
                // will be updated only to the best match node. In other words, 
                // the depth is updated only to the nodes that will be used.
                // So, depths of other nodes that are skipped are not updated. 
                // This way the depth information will be correct if different
                // nodes required alternative dependency hierarchies from some
                // content.
                currentBestMatch->
                    UpdateDependencyDepthsL( aDependencyDepth );
                }
            }
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeContainer::FindBestMatchL() end: %d",
                     totalLeafDistance);
            
    return totalLeafDistance;
    }


void CIAUpdateNodeContainer::FindMatchesL( RPointerArray< CIAUpdateNode >& aNodes, 
                                           const CIAUpdateNodeDependency& aDependency )
    {
    // Find all the matching nodes from the node list.    
    for( TInt i = 0; i < iNodes.Count(); ++i )
        {
        CIAUpdateNode& node( *iNodes[ i ] );
        // Check if the node matches the given dependency.
        if( node.Uid() == aDependency.Uid() 
            && node.Version() >= aDependency.VersionFloor() 
            && node.Version() <= aDependency.VersionRoof()
            && InstallCheckL( node ) )
            {
            // Append the matching node into the given list.
            aNodes.AppendL( &node );
            }
        }
    }


void CIAUpdateNodeContainer::ManageHeaderNodesL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::ManageHeaderNodesL() begin");

    for ( TInt i = iNodes.Count() - 1; i >= 0; --i )
        {
        CIAUpdateNode& node( *iNodes[ i ] );
        if ( node.DependencyCheckStatus() 
             != CIAUpdateNode::EDependencyCheckPassed )
            {
            // The node dependency chain has either failed 
            // or the nodes are not needed
            // because their dependency status has not been set.
            // So, remove these nodes from the node list.
            IAUPDATE_TRACE_2("[IAUPDATE] Node dependency chain broken or node not needed: %S, %S",
                             &node.MetaNamespace(), &node.MetaId());
            delete iNodes[ i ];
            iNodes[ i ] = NULL;
            iNodes.Remove( i );
            }
        else
            {
            // Notice, that dependencies have been handled 
            // for every item previously. So, now we can choose 
            // the header nodes whose dependencies are intact. 
            // Update the header list to contain newer version 
            // of nodes whose contents have not been installed yet.
            UpdateHeadNodeListL( node );                        
            }
        }    
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::ManageHeaderNodesL() end");
    }
    

void CIAUpdateNodeContainer::UpdateHeadNodeListL( CIAUpdateNode& aNode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::UpdateHeadNodeListL() begin");
    
    // All nodes that are not installed yet are possible head nodes.
    // The final list will contain head nodes that are most recent 
    // versions of nodes that can be downloaded and/or installed.
    
    if( !aNode.IsInstalled() )
        {
        IAUPDATE_TRACE("[IAUPDATE] Not installed yet.");

        // Only consider the highest version of nodes as head nodes.

        // This flag informs if the node should be appended into the list
        // because it was not inserted in the for-loop
        // or if the correct node was already in the list.
        TBool alreadyIncluded( EFalse );
        
        for ( TInt i = 0; i < iHeadNodes.Count(); ++i )
            {
            CIAUpdateNode& tmpNode( *iHeadNodes[ i ] );   
            
            if( NodeExists( aNode, tmpNode ) &&  
                  aNode.Type() != MIAUpdateNode::EPackageTypeServicePack ) 
                {
                IAUPDATE_TRACE("[IAUPDATE] Head node already in the list.");
                
                // Node for the corresponding content already exists in the list.
                // Replace node from the list if the new node is better choice.
                if ( ReplaceRecommendedL( tmpNode, aNode ) )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] Replace existing head node");
                    iHeadNodes[ i ] = &aNode;
                    // Because the old node was removed from the head list,
                    // it is not head node anymore. Set it as hidden. So,
                    // it will be handled correctly when downloading 
                    // and installing is done. For example, if the new node 
                    // depends on the older version.
                    tmpNode.ForceHidden( ETrue );
                    }
                else
                    {
                    IAUPDATE_TRACE("[IAUPDATE] Existing head node stays in head list");
                    // The existing head node is newer. So, the recommended new node
                    // will not be head node. Make sure that it is set as hidden
                    // because hidden value is used to check if the node is head node
                    // or not.
                    aNode.ForceHidden( ETrue );
                    }

                // We found a corresponding item from the head list.
                // Now, the correct node is in the list.
                alreadyIncluded = ETrue;
                break;
                }
            }
            
        if( !alreadyIncluded )
            {            
            // The given node was not inserted above. 
            // So, append it now to the end of the list.
            // Notice, that here we still may append hidden node into the head
            // list. This way, the newest node is always in the head list when
            // comparison is done. Later RemoveHiddenNodesFromHeadList 
            // is called. If the head node is hidden, it will be removed from 
            // the head list. This way we can be sure that head list contains
            // only newest nodes and no visible head node version is older than 
            // some hidden one.
            IAUPDATE_TRACE("[IAUPDATE] Add new head node into the list.");
            iHeadNodes.AppendL( &aNode );            
            }            
        }
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::UpdateHeadNodeListL() begin");
    }
    
    
TBool CIAUpdateNodeContainer::ReplaceRecommendedL(
    const CIAUpdateNode& aCurrentNode,
    const CIAUpdateNode& aNewNode ) const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::ReplaceRecommendedL() begin");
    
    
    if ( !NodeExists( aCurrentNode, aNewNode ) )
        {
        IAUPDATE_TRACE("[IAUPDATE] LEAVE: Nodes do not match.");
        User::Leave( KErrArgument );
        }
    
    // New node is recommended if encountered node has higher version, 
    // or if the version is equal but current node packet type is 
    // not preferred. Also, the leaf distance of dependency chain is checked
    // because long dependency chains should be avoided.
    // The content package order is: (first) PU, SP, SA
    // Notice, that PU and SP require already installed version of
    // the application but this was already checked when node were added
    // into the array.
    if ( aNewNode.Version() > aCurrentNode.Version() 
         || aNewNode.Version() == aCurrentNode.Version() 
            && aNewNode.LeafDistance() <= aCurrentNode.LeafDistance()
            && ( aNewNode.Type() == aCurrentNode.Type()
                 || aNewNode.Type() != MIAUpdateNode::EPackageTypeSA
                    && aCurrentNode.Type() == MIAUpdateNode::EPackageTypeSA
                 || aNewNode.Type() == MIAUpdateNode::EPackageTypePU 
                    && aCurrentNode.Type() == MIAUpdateNode::EPackageTypeSP ) )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::ReplaceRecommendedL() end: ETrue");
        return ETrue;
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::ReplaceRecommendedL() end: EFalse");
        return EFalse;
        }
    }


TBool CIAUpdateNodeContainer::PackageTypeAcceptedL( 
    const CIAUpdateNode& aNode ) const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::PackageTypeAcceptedL() begin");
    
    if ( aNode.Type() == MIAUpdateNode::EPackageTypeServicePack
         || aNode.Type() == MIAUpdateNode::EPackageTypeSA 
         || aNode.Type() == MIAUpdateNode::EPackageTypeWidget )
        {
        // Service packs, SA types and Widgets are always accepted.
        IAUPDATE_TRACE("[IAUPDATE] Upgrade packet type SA, service pack or widget");
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::PackageTypeAcceptedL() end: ETrue");
        return ETrue;
        }

    TBool accepted( EFalse );
        
    // The PU and SP packet types are accepted only if 
    // the content that will be updated already exists in the phone. 
    // Otherwise, the SA should be used.
    RPointerArray< CIAUpdateNodeDependency > dependencies;
    CleanupClosePushL( dependencies );
    
    aNode.Details().GetDependenciesL( dependencies );
    
    for ( TInt i = 0; i < dependencies.Count(); ++i )
        {
        CIAUpdateNodeDependency* dep( dependencies[ i ] );
        if ( aNode.Uid() == dep->Uid() )   
            {
            // This dependency describes the upgrade dependency
            // because the UIDs match. So, now we check if 
            // the upgrade dependency content is available in the phone.
            TIAUpdateVersion version;
            
            // Check if the dependency file has already been installed 
            // and get the version of the application if it has been installed.
            TBool installed = 
                IAUpdateUtils::IsAppInstalledL( dep->Uid(), version );
            
            if ( installed  
                 && version >= dep->VersionFloor()
                 && version <= dep->VersionRoof() )
                {
                // The content was already installed and its version belonged 
                // to the required version range. So, upgrade content is accepted.
                IAUPDATE_TRACE("[IAUPDATE] Accepted");
                accepted = ETrue;
                break;        
                }
            }
        }

    // If dependencies to the previous versions of 
    // the same content were not found, PU and SP are not accepted.
    CleanupStack::PopAndDestroy( &dependencies );

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeContainer::PackageTypeAcceptedL() end: %d",
                     accepted);
    
    return accepted;
    }


TBool CIAUpdateNodeContainer::NodeAlreadyExists( const CIAUpdateNode& aNode ) const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::NodeAlreadyExists() begin");
    
    CIAUpdateNode* node( NULL );
    for ( TInt i = 0; i < iNodes.Count(); ++i )
        {
        node = iNodes[ i ];
        if ( node->Equals( aNode ) )
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::NodeAlreadyExists() end: ETrue");
            return ETrue;
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::NodeAlreadyExists() end: EFalse");

    return EFalse;
    }


void CIAUpdateNodeContainer::CreateSelfUpdateBundleL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::CreateSelfUpdateBundleL() begin");
    
    // Check if IAD exists and if self updater and NCD exist.
    // Mark self updater and NCD hidden if necessary.
    // Then, RemoveHiddenNodesFromHeadList will handle rest
    // when it is called later in the head list creation process.

    CIAUpdateNode* iad( NULL );
    CIAUpdateNode* ncd( NULL );
    CIAUpdateNode* selfUpdater( NULL );
    
    for ( TInt i = 0; i < iHeadNodes.Count(); ++i )
        {
        CIAUpdateNode* node( iHeadNodes[ i ] );
        const TUid& nodeUid( node->Uid() );

        if ( IAUpdateNodeFactory::IsIad( nodeUid ) )
            {
            iad = node;
            }
        else if ( IAUpdateNodeFactory::IsNcd( nodeUid ) )
            {
            ncd = node;
            }
        else if ( IAUpdateNodeFactory::IsUpdater( nodeUid ) )
            {
            selfUpdater = node;
            }
        }

    // If IAD is given, then other self update related items should
    // be set hidden.
    if ( iad )
        {
        IAUPDATE_TRACE("[IAUPDATE] IAD");
        // Because IAD is given, the possible NCD and self updater nodes
        // should be set hidden and dependencies should be created for
        // self update bundle.
        // Notice, here we create the forced dependencies only from IAD.
        if ( ncd )
            {
            IAUPDATE_TRACE("[IAUPDATE] NCD");
            // Make NCD hidden and force IAD to depend on it.
            ncd->ForceHidden( ETrue );
            // SetExcessDependencyL will also update the dependant info
            // for the dependency node.
            iad->SetExcessDependencyL( *ncd, ETrue );

            // Because there is possibility that some service pack items
            // depend on NCD but do not depend on IAD we need to force
            // the dependency to IAD. This way if that service pack is
            // chosen, the IAD that is visible in UI will also be chosen
            // in UI and the flow can continue correctly. If IAD would not
            // be forced, then NCD would not be installed because at the
            // moment service packs do not support self update in IAD Engine
            // side. If IAD does not exist, then NCD will be shown in UI
            // and it would be selected if dependency requires that.
            TInt ncdDependantCount( ncd->DependantNodes().Count() );
            const RPointerArray< CIAUpdateNode >& ncdDependants(
                ncd->DependantNodes() );
            IAUPDATE_TRACE_1("[IAUPDATE] Ncd dependant count: %d",    
                             ncdDependantCount);
            for ( TInt i = 0; i < ncdDependantCount; ++i )
                {
                CIAUpdateNode* ncdDependantNode( 
                    ncdDependants[ i ] );
                if ( iad != ncdDependantNode )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] NCD dependant gets IAD as dependency");
                    // The dependant was not iad itself.
                    // Force dependant to depend also from iad.
                    ncdDependantNode->SetExcessDependencyL( *iad, ETrue );
                    }
                }
            }
        if ( selfUpdater )
            {
            IAUPDATE_TRACE("[IAUPDATE] Selfupdater");
            // Make selfupdater hidden and force IAD to depend on it.
            selfUpdater->ForceHidden( ETrue );
            // SetExcessDependencyL will also update the dependant info
            // for the dependency node.
            iad->SetExcessDependencyL( *selfUpdater, ETrue );
            }
        }

    // Above, dependencies were forced to make sure that self update
    // bunlde is created and the self udpate flow will handle the items
    // in correct order. Now, make sure that self updater depth and
    // leaf distance are correct if both ncd and self updater exist.
    // There is at least some sort of dependency also between them.
    if ( selfUpdater && ncd )
        {
        IAUPDATE_TRACE("[IAUPDATE] Self updater dependency for NCD");
        // Because NCD also requires self updater for its install,
        // set the dependency depth and leaf distances also here. 
        // Then, the update flow will always try to install self
        // updater before NCD. Notice, if server has not defined 
        // the dependency between selfupdater and NCD, then do not
        // force the dependency here either.
        // In case of IAD, the items will be hidden from UI.
        // Then, forced dependency chains will not change functionality
        // in UI. But, if IAD was missing and only NCD and selfupdater would
        // exist, NCD and self updater should be shown in UI themselves. Then,
        // UI side should not force dependency between those items, unless 
        // dependency is given in metadata. That is why we do not force
        // excess dependency into the dependency chain here. Notice, that
        // if IAD exists, then selfupdater is forced to its dependency above.
        // So, self updater is part of the IAD bundle then.
        ncd->SetExcessDependencyL( *selfUpdater, EFalse );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::CreateSelfUpdateBundleL() end");
    }


void CIAUpdateNodeContainer::RemoveHiddenNodesFromHeadList()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::RemoveHiddenNodesFromHeadList() begin");
    
    TInt count( iHeadNodes.Count() );
    for ( TInt i = count - 1; i >= 0; --i )
        {
        CIAUpdateNode* node( iHeadNodes[ i ] );
        if ( node->Hidden() )
            {
            // Node is not deleted here because it is still part of the
            // iNodes list that contains all the nodes.
            IAUPDATE_TRACE("[IAUPDATE] Remove node");
            iHeadNodes.Remove( i );
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::RemoveHiddenNodesFromHeadList() begin");
    }


void CIAUpdateNodeContainer::HandleServicePacksL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::HandleServicePacksL() begin");

    // This array will contain the nodes that belong to
    // the dependency hierarchy of the service pack.
    RPointerArray< CIAUpdateNode > servicePackNodes;
    CleanupClosePushL( servicePackNodes );

    // Notice, here that we will force all the items under the found service pack
    // as hidden. The main service pack will be shown in UI and possible service packs
    // in dependencies will be hidden.

    for ( TInt i = 0; i < iNodes.Count(); ++i )
        {
        CIAUpdateNode& node( *iNodes[ i ] );
        if ( node.Type() == MIAUpdateNode::EPackageTypeServicePack )
            {
            IAUPDATE_TRACE("[IAUPDATE] Service pack node found");

            if ( node.IsInstalled() )
                {
                IAUPDATE_TRACE("[IAUPDATE] Force service pack hidden");
                // Because service pack is already installed, set
                // it as hidden itself. Do not delete it because
                // there may be some dependencies set to it before
                // from other service packs. By setting node hidden,
                // it will not be included into the main list.
                node.ForceHidden( ETrue );
                }
            
            // Nodes of service pack are set to hidden. 
            // These nodes itself may have dependencies, but they are not forced hidden. 
            //
            node.GetDependencyNodesL( servicePackNodes, ETrue );
            for ( TInt j = 0; j < servicePackNodes.Count(); ++j )
                {
                CIAUpdateNode& servicePackNode( *servicePackNodes[ j ] );
                if ( !servicePackNode.IsSelfUpdate() )
                    {
                    servicePackNode.ForceHidden( ETrue );
                    }
                }
            // Reset the array, because we may use the array in 
            // next loops.
            servicePackNodes.Reset();
            }
        }

    CleanupStack::PopAndDestroy( &servicePackNodes );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::HandleServicePacksL() end");
    }


TBool CIAUpdateNodeContainer::InstallCheckL( CIAUpdateNode& aNode ) const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::InstallCheckL() begin");
    
    TBool checkPassed( ETrue );

    // Notice, that service packs are accepted even if they
    // would be already installed. This is because service
    // packs themselves contain dependencies and these dependencies
    // may not be set yet. So, after all the nodes are available,
    // the service packs can be checked separately.
    if ( aNode.Type() != MIAUpdateNode::EPackageTypeServicePack )
        {
        IAUPDATE_TRACE("[IAUPDATE] Not a service pack");

     
        TIAUpdateVersion installedVersion;
        TBool installed = EFalse;
        if ( aNode.Mime().Compare( IAUpdateProtocolConsts::KMimeWidget ) == 0 )
            {
            installed = IAUpdateUtils::IsWidgetInstalledL( aNode.Identifier(), installedVersion );
            }
        else
            {
            installed = IAUpdateUtils::IsAppInstalledL( aNode.Uid(), installedVersion );
            }
        // Notice that here we let the check pass also if node has the same version
        // as the installed content. By accepting same version, the dependency chains
        // will contain the currently installed node dependency information. Then,
        // if newer dependencies are provided for the already installed content,
        // they will be handled correctly for example inside service packs.
        if ( installed )
            {
            IAUPDATE_TRACE("[IAUPDATE] Content installed");
            if ( installedVersion > aNode.Version() )
                {
                IAUPDATE_TRACE("[IAUPDATE] Newer version already installed.");
                // If the installed version is newer, then think the node as
                // installed.
                checkPassed = EFalse;
                }
            else if ( installedVersion == aNode.Version() )
                {
                IAUPDATE_TRACE("[IAUPDATE] Force node hidden");
                // Node is already installed but we still want to include
                // it into the dependency chain. But, node should not be
                // shown in the UI. So, force the installed node as hidden.
                aNode.ForceHidden( ETrue );
                }
            }
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeContainer::InstallCheckL() end: %d",
                     checkPassed);

    return checkPassed;
    }

// Checks if  the head node ids / identifers are equal
TBool CIAUpdateNodeContainer::NodeExists( const CIAUpdateNode& aNode, 
                                          const CIAUpdateNode& tmpNode )const 
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeContainer::NodeExists begin");
    
    TBool exists = EFalse;

    if ( aNode.Mime().Compare( IAUpdateProtocolConsts::KMimeWidget ) == 0 )
        {
        if ( aNode.Identifier() == tmpNode.Identifier() )
            {
            exists = ETrue;
            }
        }
    else
        {
        if ( aNode.Uid() == tmpNode.Uid() )
            {
            exists = ETrue;
            }
        }
    
    
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateNodeContainer::NodeExists end: %d",
                     exists);
    
    return exists;
    }
