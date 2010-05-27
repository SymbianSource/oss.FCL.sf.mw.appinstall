/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdDependencyInfo
*
*/


#ifndef NCD_DEPENDENCY_INFO
#define NCD_DEPENDENCY_INFO

#include <e32base.h>
#include <e32cmn.h>

#include "ncdnodedependency.h"

class RReadStream; 
class RWriteStream;
class CNcdNodeIdentifier;

/**
 *  This class provides functions to get information about dependency
 *  nodes or dependency content.
 */
class CNcdDependencyInfo: public CBase
{
public:

    /** 
     * @see CNcdDependencyInfo::ConstructL
     *
     * @param aIdentifier Ownership is transferred immediately, caller MUST NOT put
     * this into cleanupstack
     */
    static CNcdDependencyInfo* NewL(  
                     const TDesC& aName, 
                     const TDesC& aVersion, 
                     const TDesC& aUid,
                     CNcdNodeIdentifier* aIdentifier );
    
    /**
     * @see CNcdDependencyInfo::NewL
     * @param aIdentifier Ownership is transferred immediately, caller MUST NOT put
     * this into cleanupstack
     *
     */
    static CNcdDependencyInfo* NewLC(
                     const TDesC& aName, 
                     const TDesC& aVersion, 
                     const TDesC& aUid,
                     CNcdNodeIdentifier* aIdentifier );


    /** 
     * @see CNcdDependencyInfo::ConstructL
     * @param aIdentifier Ownership is transferred immediately, caller MUST NOT put
     * this into cleanupstack     
     */
    static CNcdDependencyInfo* NewL(  
                     const TDesC& aName, 
                     const TDesC& aVersion, 
                     TUid aUid,
                     CNcdNodeIdentifier* aIdentifier );
    
    /**
     * @see CNcdDependencyInfo::NewL
     * @param aIdentifier Ownership is transferred immediately, caller MUST NOT put
     * this into cleanupstack     
     */
    static CNcdDependencyInfo* NewLC(
                     const TDesC& aName, 
                     const TDesC& aVersion, 
                     TUid aUid,
                     CNcdNodeIdentifier* aIdentifier );


    /** 
     * @see CNcdDependencyInfo::ConstrucL
     */
    static CNcdDependencyInfo* NewL( RReadStream& aReadStream );
    
    /**
     * @see CNcdDependencyInfo::NewL
     */
    static CNcdDependencyInfo* NewLC( RReadStream& aReadStream );


    /**
     * Destructor
     */
    virtual ~CNcdDependencyInfo();


    /** 
     * @return const TDesC& Name of the dependency content
     */
    const TDesC& Name() const;

    /** 
     * @return const TDesC& Version of the dependency content
     */
    const TDesC& Version() const;
    
    /** 
     * @return TUid Uid of the dependency content
     */
    TUid Uid() const;


    /** 
     * return const CNcdNodeIdentifier* Identifier of the metadata that
     * will provide the dependency content. If an identifier has not
     * been set, then NULL is returned. Ownership is NOT transferred.
     */
    const CNcdNodeIdentifier* Identifier() const;


    /**
     * Dependency state setter
     *
     * @param aState Dependency state
     */
    void SetDependencyState( TNcdDependencyState aState );
    
    
    /**
     * Dependency state getter
     *
     * @return aState Dependency state
     */
    TNcdDependencyState DependencyState() const;
    

    /** 
     * @param aReadStream
     */
    virtual void InternalizeL( RReadStream& aReadStream );

    /** 
     * @param aWriteStream
     */
    virtual void ExternalizeL( RWriteStream& aWriteStream ) const;


protected:
    /** 
     * Constructor
     * @param aIdentifier Identifier of the metadata that will provide the
     * dependency content. NULL if dependency is already given as a content
     * and node is not provided.
     */
    CNcdDependencyInfo( CNcdNodeIdentifier* aIdentifier );
    
    /** 
     * ConstructL
     *
     * @note If this function leaves, then the node identifier will be
     * deleted.
     * 
     * @param aName Name of the dependency content
     * @param aVersion Version of the dependency content
     * @param aUid Uid of the dependency content
     * Ownership is transferred.
     */
    void ConstructL( const TDesC& aName, 
                     const TDesC& aVersion, 
                     const TDesC& aUid );

    /** 
     * ConstructL
     *
     * @note If this function leaves, then the node identifier will be
     * deleted.
     * 
     * @param aName Name of the dependency content
     * @param aVersion Version of the dependency content
     * @param aUid Uid of the dependency content
     */
    void ConstructL( const TDesC& aName, 
                     const TDesC& aVersion, 
                     TUid aUid );
                     
    /** 
     * ConstructL
     * @param aReadStream Stream is used to internalize this class object.
     */
    void ConstructL( RReadStream& aReadStream );


private:
    CNcdDependencyInfo( const CNcdDependencyInfo& aObject );
    CNcdDependencyInfo& operator =( const CNcdDependencyInfo& aObject );


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
    
    // State of the dependency: missing, upgrade available, installed
    TNcdDependencyState iDependencyState;
};

#endif // NCD_DEPENDENCY_INFO
