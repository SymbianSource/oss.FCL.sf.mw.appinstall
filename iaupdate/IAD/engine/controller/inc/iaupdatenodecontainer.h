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



#ifndef IAUPDATENODECONTAINER_H
#define IAUPDATENODECONTAINER_H

#include <e32base.h>


class CIAUpdateController;
class CIAUpdateNode;
class CIAUpdateFwNode;
class CIAUpdateNodeDependency;

class CIAUpdateNodeContainer : public CBase
    {
    
public:

    static CIAUpdateNodeContainer *NewLC( 
        CIAUpdateController& aController );
    static CIAUpdateNodeContainer *NewL(
        CIAUpdateController& aController );
    
    virtual ~CIAUpdateNodeContainer();

    
    /**
     * Add a node to the node container.
     *
     * @note Caller should take care that same node is inserted only once.
     * This function does not do any checks. So, if the node is inserted more than once
     * it will be deleted multiple times which will result to panic.
     *
     * @note If the content of the given node can be thought as installed or if it can not be
     * installed, then this node will be immediately deleted. So, the node pointer may become 
     * obsolete when this function is called. Also, if this function is about to leave, 
     * the node will automatically be deleted.
     *
     * @param aNode Node to be added. Ownership is transferred.
     **/
    void AddNodeL( CIAUpdateNode* aNode );


    /**
     * Add an excess node to the node container. These nodes are some extra nodes that a user
     * of this class wants to store into the container separately. They are also handled
     * separately and no intelligence are done to these nodes. For example, dependecy chains
     * are not checked here. This function will also automatically make the node hidden.
     *
     * @note Caller should take care that same node is inserted only once.
     * This function does not do any checks. So, if the node is inserted more than once
     * it will be deleted multiple times which will result to panic.
     *
     * @param aNode Node to be added. Ownership is transferred.
     **/
    void AddExcessNodeL( CIAUpdateNode* aNode );


    /**
     * Add a firmware update node to the node container.
     *
     * @note Caller should take care that same node is inserted only once.
     * This function does not do any checks. So, if the node is inserted more than once
     * it will be deleted multiple times which will result to panic.
     *
     * @note If the content of the given node can be thought as installed or if it can not be
     * installed, then this node will be immediately deleted. So, the node pointer may become 
     * obsolete when this function is called. Also, if this function is about to leave, 
     * the node will automatically be deleted.
     *
     * @param aNode Node to be added. Ownership is transferred.
     **/
    void AddFwNodeL( CIAUpdateFwNode* aNode );

    
    /**
     * Clears node container and frees memory. 
     * Is automatically called during container destruction.
     **/
    void Clear();
    

    /**
     * @return const RPointerArray< CIAUpdateNode >& All nodes of the container, including ones that
     * are not header nodes. Excess nodes are not included here.
     */
    const RPointerArray< CIAUpdateNode >& AllNodes() const;


    /**
     * @return const RPointerArray< CIAUpdateNode >& Excess nodes of the container.
     */
    const RPointerArray< CIAUpdateNode >& ExcessNodes() const;
    
    
    /**
     * Gets current head nodes of nodes added to this container. Also updates node dependencies by calling
     * UpdateDependenciesL()
     * Head node is a node that represents the latest update available to a software installed on the device.
     * They should be the primary nodes to be offered to the user to make his update selections.
     * If a node is a head node it does NOT mean that it always can be installed, because if might have
     * dependencies to other nodes that cannot be installed for whatever reason.
     * @return const RPointerArray< CIAUpdateNode >& Head nodes. Ownership of the arary and its contents are
     * not transferred.
     **/
    const RPointerArray< CIAUpdateNode >& HeadNodesL();


    /**
     * @return const RPointerArray< CIAUpdateFwNode >& Firmware update nodes
     * of the container.
     */
    const RPointerArray< CIAUpdateFwNode >& FwNodes();

    
private:

    /**
     * Constructor
     */
    CIAUpdateNodeContainer( CIAUpdateController& aController );
    
    /**
     * ConstructL
     */    
    void ConstructL();


    // Prevent these if not implemented
    CIAUpdateNodeContainer( const CIAUpdateNodeContainer& aObject );
    CIAUpdateNodeContainer& operator =( const CIAUpdateNodeContainer& aObject );


    /**
     * Updates node dependencies.
     **/
    void UpdateDependenciesL();    

    TInt UpdateNodeDependenciesL( CIAUpdateNode& aNode, 
                                  TInt aDepth );

    TInt FindBestMatchL( CIAUpdateNode& aNode, 
                         CIAUpdateNodeDependency& aDependency,
                         TInt aDepth );
    
    void FindMatchesL( RPointerArray<CIAUpdateNode>& aNodes, 
                       const CIAUpdateNodeDependency& aDependency );


    void ManageHeaderNodesL();
    
    /**
     * Updates the head node list.
     * If the node content has not been installed yet or if the content
     * is newer version than installed one, then the node is thought as
     * head node.
     * @param aNode Ownership is transferred.
     */
    void UpdateHeadNodeListL( CIAUpdateNode& aNode );
    
    TBool ReplaceRecommendedL( const CIAUpdateNode& aCurrentNode, 
                               const CIAUpdateNode& aNewNode ) const;

    TBool PackageTypeAcceptedL( const CIAUpdateNode& aNode ) const;

    TBool NodeAlreadyExists( const CIAUpdateNode& aNode ) const;

    // Creates bundle for self update purposes.
    // Sets the possible existing self updater and NCD head nodes to hidden.
    // So, they will not be provided for UI. After, RemoveHiddenNodesFromHeadList
    // function is called.
    void CreateSelfUpdateBundleL();

    // Removes all possible existing head nodes that are marked as hidden from the head list.
    void RemoveHiddenNodesFromHeadList();

    // This will start to set the correct values to the service pack 
    // dependency nodes.
    void HandleServicePacksL();

    // Checks if newer version of the node is already installed.
    // Check is passed if newer version is not installed.
    // If same version of the node is installed, makes it hidden.
    TBool InstallCheckL( CIAUpdateNode& aNode ) const;

    // Checks if  the head node ids / identifers are equal
    TBool NodeExists( const CIAUpdateNode& aNode, 
                      const CIAUpdateNode& tmpNode ) const;
    
private: // data

    // Controller object
    CIAUpdateController& iController;

    // This array will contain all the nodes that belong to the intact dependency chains.
    // These nodes are head nodes and possible dependency nodes (for example previous versions
    // that are required).
    // Array owns the nodes. And they will be deleted when the array is cleared.
    RPointerArray<CIAUpdateNode> iNodes;
    
    // This array will contain only the head nodes after all the dependency checks are done.
    // Array does not own the nodes.
    RPointerArray<CIAUpdateNode> iHeadNodes;
        
    // This array will contain all the nodes excess nodes that are given by the user of this
    // class object. The excess nodes are just stored here and no normal actions are directed 
    // to them by the operations of this class object.
    RPointerArray<CIAUpdateNode> iExcessNodes;

    // This array will contain all the firmware update nodes.
    // Array owns the nodes.
    RPointerArray<CIAUpdateFwNode> iFwNodes;

    };

#endif  //  IAUPDATENODECONTAINER_H
