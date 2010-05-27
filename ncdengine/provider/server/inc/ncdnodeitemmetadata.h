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
* Description:   Contains CNcdNodeMetaData class
*
*/


#ifndef NCD_NODE_ITEM_META_DATA_H
#define NCD_NODE_ITEM_META_DATA_H


#include "ncdnodemetadataimpl.h"


/**
 *  CNcdNodeItemMetaData ...
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeItemMetaData : public CNcdNodeMetaData
    {

public:

    /**
     * NewL
     *
     * @return CNcdNodeItemMetaData* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeItemMetaData* NewL( const CNcdNodeIdentifier& aIdentifier,
                                       CNcdNodeManager& aNodeManager );

    /**
     * NewLC
     *
     * @return CNcdNodeItemMetaData* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeItemMetaData* NewLC( const CNcdNodeIdentifier& aIdentifier,
                                        CNcdNodeManager& aNodeManager );


    /**
     * Destructor
     */
    virtual ~CNcdNodeItemMetaData();


    /**
     *
     */
    virtual void InternalizeL( MNcdPreminetProtocolDataEntity& aData );


public: // CNcdNodeMetaData

    /**
     * @see CNcdNodeMetaData::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see CNcdNodeMetaData::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


protected:

    /**
     * Constructor
     */
    CNcdNodeItemMetaData( NcdNodeClassIds::TNcdNodeClassId aClassId,
                          CNcdNodeManager& aNodeManager );

    /**
     * ConstructL
     */
    virtual void ConstructL( const CNcdNodeIdentifier& aIdentifier );

    /**
     * This function adds possible item metadata to the stream
     * after the parent data.
     */    
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;
    
    
private:

    // Prevent these two if they are not implemented
    CNcdNodeItemMetaData( const CNcdNodeItemMetaData& aObject );
    CNcdNodeItemMetaData& operator =( const CNcdNodeItemMetaData& aObject );


private: // data

    };
    
#endif // C_NCD_NODE_ITEM_META_DATA_H    
