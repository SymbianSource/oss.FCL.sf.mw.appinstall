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
* Description:   This file contains the header file of MIAUpdateNode class 
*
*/



#ifndef IA_UPDATE_NODE_H
#define IA_UPDATE_NODE_H


#include <e32cmn.h>
#include "iaupdateanynode.h"


class MIAUpdateNodeObserver;


/**
 * MIAUpdateNode
 * Interface for normal nodes.
 *
 * @note Implementations of MIAUpdateNode interface will always
 * return MIAUpdateAnyNode::ENodeTypeNormal for MIAUpdateAnyNode::NodeType 
 * function call.
 *
 * @see MIAUpateAnyNode
 */
class MIAUpdateNode : public MIAUpdateAnyNode
    {    
    
public:

    /**
     * Enumerates the possible types of nodes
     **/
    enum TPackageType
        {
        
        /** 
         * The node content is independent SA package.
         */
        EPackageTypeSA,

        /** 
         * The node content is an upgrade SP package.
         */
        EPackageTypeSP,
                
        /** 
         * The node content is a patch update PU package.
         */
        EPackageTypePU,
        
        /** 
         * The node is a service pack.
         * So, its content is given as separate packages
         * that are included in this service pack.
         */
        EPackageTypeServicePack,
        
       /** 
        * The node content is widget.
        */
        EPackageTypeWidget
       
        };

    /**
    * For UI during update process
    **/
    
    enum TUIUpdateState
        {
        ENormal,
        EDownloading,
        EInstalling,
        EUpdated,
        EFailed,
        EDownloaded        
        };
    

    /**
     * @return TPackageType The type of this node content.
     **/
    virtual TPackageType Type() const = 0;
            
    /**
     * This function informs if the content of the node requires self updating.
     * @return ETrue if the content is self updating. Else EFalse.
     */
    virtual TBool IsSelfUpdate() const = 0;

    /**
     * @note If hidden files are not included, the hierarchy behind them
     * is still checked and visible nodes behind hidden nodes are also
     * always included.
     *
     * @param aDependencies Pointer array supplied by the caller. 
     * Dependency nodes are appended to it.
     * @param aIncludeHidden ETrue means that hidden nodes are included. 
     * EFalse means that hidden nodes are not included. Hidden nodes are
     * not usually meant for UI side. But, in case of checking that
     * all dependencies are handled correctly, all the nodes may be
     * required.
     **/
    virtual void GetDependenciesL( 
        RPointerArray< MIAUpdateNode >& aDependencies,
        TBool aIncludeHidden ) const = 0;

    /**
     * @note If hidden files are not included, the hierarchy behind them
     * is still checked and visible nodes behind hidden nodes are also
     * always included.
     *
     * @param aDependants Pointer array supplied by the caller. 
     * Dependant nodes are appended to it
     * @param aIncludeHidden ETrue means that hidden nodes are included. 
     * EFalse means that hidden nodes are not included. Hidden nodes are
     * not usually meant for UI side. But, in case of checking that
     * all dependencies are handled correctly, all the nodes may be
     * required.
     **/
    virtual void GetDependantsL( 
        RPointerArray< MIAUpdateNode >& aDependants,
        TBool aIncludeHidden ) const = 0;
    
    /**
     * @return TBool ETrue if the node is downlaoded. Else EFalse.
     **/
    virtual TBool IsDownloaded() const = 0;  
    
    /**
     * @return TBool ETrue if the node is installed. Else EFalse.
     **/
    virtual TBool IsInstalled() const = 0;

    /**
     * Issue an operation for the node. Only one operation can be in progress at a time; 
     * user has to always wait until previous operation has completed.
     * 
     * @param aObserver Observer of the operation
     **/
    virtual void DownloadL( MIAUpdateNodeObserver& aObserver ) = 0;

    /**
     * Issue an operation for the node. Only one operation can be in progress at a time; 
     * user has to always wait until previous operation has completed.
     * 
     * @param aObserver Observer of the operation
     **/
    virtual void InstallL( MIAUpdateNodeObserver& aObserver ) = 0; 

    /**
     * Cancel an ongoing operation
     **/    
    virtual void CancelOperation() = 0;

    /**
     * Dependency hierarchy information may be usefull when dependency or dependant
     * items are sorted for the operation flow. The dependant node always has smaller
     * depth than its dependency node.
     *
     * @note This depth is counted from the longest dependency chain to the node.
     * Node may belong to multiple dependency branches. In some of the branches, 
     * the depth could be less than the greatest value that is given here.
     *
     * @return TInt This describes how deep in the dependency hierarchy
     * this node is. Zero means that node is a root.
     */
    virtual TInt Depth() const = 0;

    virtual void SetUiState( TUIUpdateState aState ) = 0;
    
    virtual TUIUpdateState UiState() const = 0;
    
    // for progress bar in UI
    virtual void SetProgress( TInt aProgress ) = 0;
    
    virtual TInt Progress() const = 0; 
    
    virtual void SetTotal( TInt aTotal ) = 0;
    
    virtual TInt Total() const = 0; 
    
       

protected:

    virtual ~MIAUpdateNode() { }
        
    };

#endif  //  IA_UPDATE_NODE_H
