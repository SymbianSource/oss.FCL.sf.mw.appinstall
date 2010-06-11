/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdNodeDependencyInfo
*
*/


#ifndef NCD_NODE_DEPENDENCY_INFO_IMPL
#define NCD_NODE_DEPENDENCY_INFO_IMPL


#include <e32base.h>
#include <e32cmn.h>

#include "ncdnodedependencyinfo.h"

class CNcdNodeIdentifier;
class CNcdDependencyInfo;
class CNcdNodeManagerProxy;


/**
 *  This class provides functions to get information about dependency
 *  nodes or dependency content.
 */
class CNcdNodeDependencyInfo: public CBase,
                              public MNcdNodeDependencyInfo
{
public:

    /** 
     * @see CNcdNodeDependencyInfo::CNcdNodeDependencyInfo   
     * @see CNcdNodeDependencyInfo::ConstructL
     */
    static CNcdNodeDependencyInfo* NewL( const CNcdDependencyInfo& aInfo,
                                         CNcdNodeManagerProxy& aNodeManager ); 
    
    /**
     * @see CNcdDependencyInfo::NewL
     */
    static CNcdNodeDependencyInfo* NewLC( const CNcdDependencyInfo& aInfo,
                                          CNcdNodeManagerProxy& aNodeManager );


    /**
     * Destructor
     */
    ~CNcdNodeDependencyInfo();


public: // MNcdNodeDependencyInfo
    /** 
     * @see MNcdNodeDependencyInfo::Name
     */
    virtual const TDesC& Name() const;

    /** 
     * @see MNcdNodeDependencyInfo::Version
     */
    virtual const TDesC& Version() const;
    
    /** 
     * @see MNcdNodeDependencyInfo::Uid
     */
    virtual TUid Uid() const;

    /** 
     * @see MNcdNodeDependencyInfo::DependencyNodeL
     */
    virtual MNcdNode* DependencyNodeL() const;
    

    /** 
     * @see MNcdNodeDependencyInfo::State
     */
    virtual TNcdDependencyState State() const;

public: // new methods

    /** 
     * return const CNcdNodeIdentifier* Identifier of the metadata that
     * will provide the dependency content. If an identifier has not
     * been set, then NULL is returned. Ownership is NOT transferred.
     */
    const CNcdNodeIdentifier* Identifier() const;
    
    
protected:
    /** 
     * Constructor
     *
     * @param aNodeManager Node manager is used to create the node.
     */
    CNcdNodeDependencyInfo( CNcdNodeManagerProxy& aNodeManager );
    
    /** 
     * ConstructL
     *
     * @note If this function leaves, then the node identifier will be
     * deleted.
     * 
     * @param aInfo Dependency information that is used to internalize
     * this class object.
     */
    void ConstructL( const CNcdDependencyInfo& aInfo );


    /**
     * @return CNcdNodeManageraProxy& Can be used to create nodes.
     */
    CNcdNodeManagerProxy& NodeManager() const;
    

private:
    CNcdNodeDependencyInfo( const CNcdNodeDependencyInfo& aObject );
    CNcdNodeDependencyInfo& operator =( const CNcdNodeDependencyInfo& aObject );

private: // data

    // Name of the dependency content
    HBufC* iName;
    // Version of the dependency content
    HBufC* iVersion;
    // Uid of the dependency content
    TUid iUid;
    // Identifier of the metadata that will provide the
    // dependency content.
    CNcdNodeIdentifier* iIdentifier;

    CNcdNodeManagerProxy& iNodeManager;

    // State of the dependency
    TNcdDependencyState iDependencyState;
};

#endif // NCD_NODE_DEPENDENCY_INFO_IMPL
