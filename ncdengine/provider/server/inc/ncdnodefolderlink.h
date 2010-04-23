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
* Description:   Contains CNcdNodeFolderLink class
*
*/


#ifndef NCD_NODE_FOLDER_LINK_H
#define NCD_NODE_FOLDER_LINK_H


#include "ncdnodelink.h"
#include "ncdnodeclassids.h"


/**
 *  CNcdNodeFolderLink ...
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeFolderLink : public CNcdNodeLink
    {

public:

    /**
     * NewL
     *
     * @param aNode The node that owns this link.
     * @return CNcdNodeFolderLink* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeFolderLink* NewL( CNcdNode& aNode );

    /**
     * NewLC
     *
     * @param aNode The node that owns this link.
     * @return CNcdNodeFolderLink* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeFolderLink* NewLC( CNcdNode& aNode );


    /**
     * Destructor
     */
    virtual ~CNcdNodeFolderLink();


    /** 
     *
     */
    TInt ExpectedChildrenCount() const;



public: // CNcdNodeLink

    /**
     * @see CNcdNodeLink::InternalizeL
     */
    virtual void InternalizeL( const MNcdPreminetProtocolEntityRef& aData,
                               const CNcdNodeIdentifier& aParentIdentifier,
                               const CNcdNodeIdentifier& aRequestParentIdentifier,
                               const TUid& aClientUid );


    /**
     * @see CNcdNodeLink::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see CNcdNodeLink::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


protected:

    /**
     * Constructor
     */
    CNcdNodeFolderLink( CNcdNode& aNode,
                        NcdNodeClassIds::TNcdNodeClassId aClassId = NcdNodeClassIds::ENcdFolderNodeLinkClassId );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    /**
     * This function overrides or adds functionality to the parent class
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;


private:

    // Prevent these two if they are not implemented
    CNcdNodeFolderLink( const CNcdNodeFolderLink& aObject );
    CNcdNodeFolderLink& operator =( const CNcdNodeFolderLink& aObject );


private: // data

    // This number informs the total number of children that exists in
    // the server side.
    // Not all of them are necessarily inserted to the folder.
    // For example if amount of children is restricted when data is
    // downloaded (this may be the case for example when paging has been
    // done). iChildrenLinks count and iExpectedChildrenCount can be
    // compared when checking if all the links have been downloaded.
    TInt iExpectedChildrenCount;
    };


#endif // NCD_NODE_LINK_H
