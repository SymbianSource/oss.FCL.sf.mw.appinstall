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
* Description:  
*
*/


#ifndef C_NCDSEARCHOPERATIONIMPL_H
#define C_NCDSEARCHOPERATIONIMPL_H

#include "ncdloadnodeoperationimpl.h"
#include "ncdnodefactory.h"

class CNcdSearchFilter;

/**
 *  Search operation implementation.
 *
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdSearchOperation : public CNcdLoadNodeOperationImpl
    {
    
public:
    
    /*
     * @param aRecursionLevel Determines the number of remote folder levels
     * the search request is made to. Negative number means unlimited levels.
     */
    static CNcdSearchOperation* NewL(
        const CNcdNodeIdentifier& aNodeIdentifier,
        const CNcdNodeIdentifier& aParentIdentifier,
        const CNcdSearchFilter& aFilter,
        TNcdResponseFilterParams aFilterParams,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MNcdOperationQueue* aOperationQueue,
        MCatalogsSession& aSession,
        TBool aLoadChildren = EFalse,
        TNcdChildLoadMode aMode = ELoadStructure,
        TInt aRecursionLevels = -1,
        TBool iIsSubOperation = EFalse );

    /*
     * @param aRecursionLevel Determines the number of remote folder levels
     * the search request is made to. Negative number means unlimited levels.
     */
    static CNcdSearchOperation* NewLC(
        const CNcdNodeIdentifier& aNodeIdentifier,
        const CNcdNodeIdentifier& aParentIdentifier,
        const CNcdSearchFilter& aFilter,
        TNcdResponseFilterParams aFilterParams,        
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MNcdOperationQueue* aOperationQueue,
        MCatalogsSession& aSession,
        TBool aLoadChildren = EFalse,
        TNcdChildLoadMode aMode = ELoadStructure,
        TInt aRecursionLevels = -1,
        TBool aIsSubOperation = EFalse );
        
    virtual ~CNcdSearchOperation();
    
public: // from  MNcdParserEntityObserver

    /**
     * @see MNcdParserEntityObserver
     */
    virtual void FolderRefL( MNcdPreminetProtocolFolderRef* aData );
    
    /**
     * @see MNcdParserEntityObserver
     */
    virtual void FolderDataL( MNcdPreminetProtocolDataEntity* aData );
    
    /**
     * @see MNcdParserEntityObserver
     */
    virtual void ItemRefL( MNcdPreminetProtocolItemRef* aData );
    
    /**
     * @see MNcdParserEntityObserver
     */
    virtual void ItemDataL( MNcdPreminetProtocolDataEntity* aData );
    
public: // from MNcdParserErrorObserver

    virtual void ErrorL( MNcdPreminetProtocolError* aData );
    
protected:

    CNcdSearchOperation( TNcdResponseFilterParams aFilterParams,
        TNcdChildLoadMode aMode,
        TBool aLoadChildren,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MNcdOperationQueue* aOperationQueue,
        MCatalogsSession& aSession,
        TInt aRecursionLevels,
        TBool aIsSubOperation = EFalse );
    
    void ConstructL( const CNcdNodeIdentifier& aNodeIdentifier,
        const CNcdNodeIdentifier& aParentIdentifier,
        const CNcdSearchFilter& aFilter );

protected: // From CNcdLoadNodeOperationImpl

    virtual HBufC8* CreateRequestLC( CNcdNodeIdentifier* aNodeIdentifier,
        TNcdResponseFilterParams aFilterParams,
        const TDesC& aUri );
        
    virtual void CreateSubOperationsL();
    
    
    /**
     * @see CNcdLoadNodeOperationImpl::IsLoadingNecessaryL
     */
    TBool IsLoadingNecessaryL();
    
    /**
     * @see CNcdLoadNodeOperationImpl::IsChildClearingNecessaryL
     */
    virtual TBool IsChildClearingNecessaryL();
    
    /**
     * @see CNcdLoadNodeOperationImpl::RemoteFolderCount
     */    
    virtual TInt RemoteFolderCount() const;
    

    /**
     * @see CNcdLoadNodeOperationImpl::RemoveChildrenL
     */    
    virtual void RemoveChildrenL( CNcdNodeFolder& aFolder );
    
    
protected:

    void DetermineParentTypeL( const TUid& aUid );
    
    /**
     * Creates a skeleton node that contains all needed info to
     * conduct search for a bundle.
     *
     * @return The created search folder.
     */
    CNcdNode& CreateSearchBundleSkeletonL();
    
    /**
     * Creates a skeleton node that contains all needed info to
     * conduct search for a regular folder.
     *
     * @return The created search folder.
     */
    CNcdNode& CreateSearchFolderSkeletonL();
    
    /**
     * Actual implementation for FolderRefL
     */
    void DoFolderRefL( MNcdPreminetProtocolFolderRef* aData );
    
    /**
     * Actual implementation for ItemRefL
     */
    void DoItemRefL( MNcdPreminetProtocolItemRef* aData );
    
    /**
     * Actual implementation for ItemDataL
     */
    void DoItemDataL( MNcdPreminetProtocolDataEntity* aData );
    
private: // data
    
    CNcdSearchFilter* iSearchFilter;
    
    CNcdNodeFactory::TNcdNodeType iParentType;
    
    // Recursion levels left
    TInt iRecursionLeft;
    
    // Remote folders that are children of transparent will be here.
    RPointerArray<CNcdNodeIdentifier> iRemoteFoldersChildOfTransparent;

    };

#endif // C_NCDSEARCHOPERATIONIMPL_H
