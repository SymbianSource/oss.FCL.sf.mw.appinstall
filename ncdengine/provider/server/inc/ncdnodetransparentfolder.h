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
* Description:   Contains CNcdNodeTransparentFolder class
*
*/


#ifndef NCD_NODE_TRANSPARENT_FOLDER_H
#define NCD_NODE_TRANSPARENT_FOLDER_H


#include <e32cmn.h>

#include "ncdnodefolder.h"

class CNcdNodeIdentifier;


/**
 *  This class provides transparent features for the node.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeTransparentFolder : public CNcdNodeFolder
    {
    
public:

    /**
     * @see CNcdNodeFolder::NewL
     *
     * @return CNcdNodeTransparentFolder* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeTransparentFolder* NewL( CNcdNodeManager& aNodeManager,
                                            const CNcdNodeIdentifier& aIdentifier );

    /**
     * @see CNcdNodeTransparentFolder::NewL
     */
    static CNcdNodeTransparentFolder* NewLC( CNcdNodeManager& aNodeManager,
                                             const CNcdNodeIdentifier& aIdentifier );


    /**
     * Destructor
     */
    virtual ~CNcdNodeTransparentFolder();


public: // CNcdNodeFolder

    /**
     * @see CNcdNodeFolder::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see CNcdNodeFolder::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );

public:

    /**
     * Checks that are any of the children expired, missing, or is a child missing metadata.
     *
     * Transparent folders must be up to date because their content's are displayed
     * one level up.
     *
     * @return ETrue if some children are missing or expired.
     */
    TBool HasExpiredOrMissingChildrenL();

protected:

    /**
     * @see CNcdNodeFolder::CNcdNodeFolder
     */
    CNcdNodeTransparentFolder( CNcdNodeManager& aNodeManager,
        NcdNodeClassIds::TNcdNodeClassId aNodeClassId = NcdNodeClassIds::ENcdTransparentFolderNodeClassId, 
        NcdNodeClassIds::TNcdNodeClassId aAcceptedLinkClassId = NcdNodeClassIds::ENcdFolderNodeLinkClassId,
        NcdNodeClassIds::TNcdNodeClassId aAcceptedMetaDataClassId = NcdNodeClassIds::ENcdFolderNodeMetaDataClassId );
    
    /**
     * @see CNcdNodeFolder::ConstructL
     */
    virtual void ConstructL( const CNcdNodeIdentifier& aIdentifier );


    /** 
     * @see CNcdNodeFolder::ExternalizeDataForRequestL
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;    
    
    
private:
    
    // Prevent these two if they are not implemented
    CNcdNodeTransparentFolder( const CNcdNodeTransparentFolder& aObject );
    CNcdNodeTransparentFolder& operator =( const CNcdNodeTransparentFolder& aObject );
    
    };


#endif // NCD_NODE_TRANSPARENT_FOLDER_H
