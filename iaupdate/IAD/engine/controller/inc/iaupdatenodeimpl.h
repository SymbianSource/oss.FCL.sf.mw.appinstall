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
* Description:   This file contains the header file of CIAUpdateNode class 
*
*/


#ifndef IA_UPDATE_NODE_IMPL_H
#define IA_UPDATE_NODE_IMPL_H


#include "iaupdatebasenodeimpl.h"
#include "iaupdatenode.h"
#include "iaupdatecontentoperationobserver.h"

class CIAUpdateNodeDependency;


/**
 *
 */
class CIAUpdateNode : public CIAUpdateBaseNode, 
                      public MIAUpdateNode,
                      public MIAUpdateContentOperationObserver
    {

public:

    /** 
     *
     */
    enum TDependencyCheckStatus
        {
        EDependencyCheckNotSet,
        EDependencyCheckGoing,
        EDependencyCheckPassed,
        EDependencyCheckFailed
        };


    /**
     * @see IAUpdateNodeFactory::CreateNodeLC
     **/
    static CIAUpdateNode* NewLC( MNcdNode* aNode,
                                 CIAUpdateController& aController );
    
    /**
     * @see CIAUpdateNode::NewLC
     **/
    static CIAUpdateNode* NewL( MNcdNode* aNode,
                                CIAUpdateController& aController );
    
    
    /**
     * Destructor
     **/
    virtual ~CIAUpdateNode();


public: // CIAUpdateBaseNode

    /**
     * @note This implementation also includes all OwnContentSizesL of 
     * the non-hidden nodes of the dependency tree.
     * @see CIAUpdateBaseNode::ContentSize
     */
    virtual TInt ContentSizeL() const;   


public: // MIAUpdateNode

    /**
     * @see MIAUpdateNode::Type
     **/
    virtual TPackageType Type() const;
        
    /**
     * This implementation always returns EFalse.
     * @see MIAUpdateNode::IsSelfUpdate
     */
    virtual TBool IsSelfUpdate() const;

    /**
     * @see MIAUpdateNode::GetDependenciesL
     **/
    virtual void GetDependenciesL( 
        RPointerArray< MIAUpdateNode >& aDependencies,
        TBool aIncludeHidden ) const;
    
    /**
     * @see MIAUpdateNode::GetDependantsL
     */
    virtual void GetDependantsL( 
        RPointerArray< MIAUpdateNode >& aDependants,
        TBool aIncludeHidden ) const;

    /**
     * @see MIAUpdateNode::IsDownloaded
     **/
    virtual TBool IsDownloaded() const;
    
    /**
     * @see MIAUpdateNode::IsInstalled
     **/
    virtual TBool IsInstalled() const;

    /**
     * @see MIAUpdateNode::DownloadL
     **/
    virtual void DownloadL( MIAUpdateNodeObserver& aObserver );

    /**
     * @see MIAUpdateNode::InstallL
     **/
    virtual void InstallL( MIAUpdateNodeObserver& aObserver );

    /**
     * @see MIAUpdateNode::CancelOperation
     **/    
    virtual void CancelOperation();

    /** 
     * @notice That this value has to be set by other objects. 
     * If it is not set, it returns zero.
     *
     * @see MIAUpdateNode::Depth
     */
    virtual TInt Depth() const;
    
    virtual void SetUiState( TUIUpdateState aState );
        
    virtual TUIUpdateState UiState() const;
        
    // for progress bar in UI
    virtual void SetProgress( TInt aProgress );
        
    virtual TInt Progress() const; 
        
    virtual void SetTotal( TInt aTotal );
        
    virtual TInt Total() const; 
               
    /**
     * @see MIAUpdateNode:::NodeType
     */
    virtual TNodeType NodeType() const;
    
    /**
     * @see MIAUpdateNode::Base
     */
    virtual MIAUpdateBaseNode& Base();


public: // MIAUpdateContentOperationObserver

    /**
     * @see MIAUpdateContentOperationObserver::ContentOperationComplete
     */
    virtual void ContentOperationComplete( CIAUpdateBaseNode& aNode, 
                                           TInt aError );

    /**
     * @see MIAUpdateContentOperationObserver::ContentOperationProgress
     */
    virtual void ContentOperationProgress( CIAUpdateBaseNode& aNode, 
                                           TInt aProgress, 
                                           TInt aMaxProgress );

    
public: // ** New functions

    /**
     * Can be used to reset variables that are set externally.
     * Variables such as depth and leaf distance and check status.
     */
    void Reset();


    /**
     * @return TBool ETrue if the node content has been purchased.
     * Else EFalse.
     */
    TBool IsPurchased() const;


    /**
     * This method provides means to add extra dependencies for this node.
     * If the dependency corresponding to the given node already exists, then the
     * old dependency is kept. If given dependency will create a loop, then nothing
     * is done.
     *
     * @note This function supposes that the dependency tree below aDependencyNode
     * is fullfilled and intact.
     *
     * @param aDependencyNode This node depends on aDependencyNode.
     * @param aAddDependency If ETrue, then dependency chain is
     * updated and the dependant depths are updated. If EFalse, then dependency
     * is not added but still dependant depths are updated. This can be usefull, 
     * if dependency chain should not be changed, but still the depths of the 
     * dependant items should know about some excess dependency nodes that may 
     * affect the operation flow order.
     */
    void SetExcessDependencyL( CIAUpdateNode& aDependencyNode,
                               TBool aAddDependency );

    /**
     * @see CIAUpdateNode::GetDependenciesL
     */
    void GetDependencyNodesL( 
        RPointerArray< CIAUpdateNode >& aDependencies,
        TBool aIncludeHidden ) const;

    /**
     * Sets the status that informs if the dependency chain check for this node and
     * all the nodes it depends on were passed or failed.
     *
     * @param aStatus
     */
    void SetDependencyCheckStatus( TDependencyCheckStatus aStatus );

    /**
     * @return TDependencyCheckStatus Dependency check status of this node and
     * all the nodes it depends on.
     */
    TDependencyCheckStatus DependencyCheckStatus() const;
       
    /**
     * @notice That this value has to be set by other objects. 
     * If it is not set, it returns zero. 
     *
     * @return TInt This value informs the distance of this node
     * from the furthest leaf node.
     */
    TInt LeafDistance() const;    

    /**
     * @param aDistance This value informs the distance of this node
     * from the furthest leaf node.
     */
    void SetLeafDistance( TInt aDistance );

    /**
     * @param aDepth This value informs the distance of this node from
     * the furthest root. Root has zero depth.
     */
    void SetDepth( TInt aDepth );


    /**
     * RPointerArray< CIAUpdateNode >& Reference to the dependant node array. 
     * Ownership is not transferred.
     */
    const RPointerArray< CIAUpdateNode >& DependantNodes() const;

    /**
     * Adds dependant node for this node. 
     * @param aDependantNode Dependant node
     */
    void AddDependantL( CIAUpdateNode& aDependantNode );


    /**
     * Updates the depths of the dependency nodes
     * starting from this node and then recursively
     * its dependencies.
     *
     * @note Depth is updated only if the new depth will be greater
     * than the old depth or if a negative value is given for refresh.
     *
     * @param aDepth The depth of this node.
     */
    void UpdateDependencyDepthsL( TInt aDepth );


protected:

    /**
     * @see CIAUpdateBaseNode::CIAUpdateBaseNode
     */
    CIAUpdateNode( CIAUpdateController& aController );

    /**
     * @see CIAUpdateBaseNode::ConstructL
     */    
    virtual void ConstructL( MNcdNode* aNode );

    /**
     * @see CIAUpdateNode::IsPurchased
     */
    virtual TBool IsPurchasedL() const;
    
    /**
     * @see CIAUpdateNode::IsDownloaded
     */
    virtual TBool IsDownloadedL() const;

    /**
     * @see CIAUpdateNode::IsInstalled
     */
    virtual TBool IsInstalledL() const;

    /**
     * @return MIAUpdateNodeObserver* Operation observer.
     * NULL if not set and then operation is not going on.
     * Ownership is not transferred.
     */
    MIAUpdateNodeObserver* OperationObserver() const;


private:

    // Prevent these if not implemented
    CIAUpdateNode( const CIAUpdateNode& aObject );
    CIAUpdateNode& operator =( const CIAUpdateNode& aObject );

    // Updates the leaf distances of the dependant nodes
    // starting from this node and then recursively
    // its dependants. aLeafDistance is the distance of 
    // this node from the furthest leaf node.
    void UpdateDependantLeafDistancesL( TInt aLeafDistance );


private: // data

    TDependencyCheckStatus              iDependencyCheckStatus;
    TInt                                iLeafDistance;
    TInt                                iDepth;
    TUIUpdateState                      iUiUpdateState;
    TInt                                iProgress;
    TInt                                iTotal;
    
    // These arrays do not own the nodes.
    RPointerArray< CIAUpdateNode >      iDependants;
    RPointerArray< CIAUpdateNode >      iExcessDependencyNodes;

    // Not owned.
    MIAUpdateNodeObserver*              iOperationObserver;

    };
    
    
#endif  //  IA_UPDATE_NODE_IMPL_H
