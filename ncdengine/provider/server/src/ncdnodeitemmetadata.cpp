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
* Description:   Implements CNcdNodeItemMetaData class
*
*/


#include "ncdnodeitemmetadata.h"
#include "ncdnodeclassids.h"
#include "ncd_pp_dataentity.h"


CNcdNodeItemMetaData* CNcdNodeItemMetaData::NewL( const CNcdNodeIdentifier& aIdentifier, 
                                                  CNcdNodeManager& aNodeManager )
    {
    CNcdNodeItemMetaData* self =   
        CNcdNodeItemMetaData::NewLC( aIdentifier, aNodeManager );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeItemMetaData* CNcdNodeItemMetaData::NewLC( const CNcdNodeIdentifier& aIdentifier,
                                                   CNcdNodeManager& aNodeManager )
    {
    CNcdNodeItemMetaData* self = 
        new( ELeave ) CNcdNodeItemMetaData( NcdNodeClassIds::ENcdItemNodeMetaDataClassId,
                                            aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;        
    }


CNcdNodeItemMetaData::CNcdNodeItemMetaData( NcdNodeClassIds::TNcdNodeClassId aClassId,
                                            CNcdNodeManager& aNodeManager )
: CNcdNodeMetaData( aClassId, aNodeManager )
    {
    }

void CNcdNodeItemMetaData::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdNodeMetaData::ConstructL( aIdentifier );
    }

CNcdNodeItemMetaData::~CNcdNodeItemMetaData()
    {
    }        
    

void CNcdNodeItemMetaData::InternalizeL( MNcdPreminetProtocolDataEntity& aData )
    {
    CNcdNodeMetaData::InternalizeL( aData );
    }

void CNcdNodeItemMetaData::ExternalizeL( RWriteStream& aStream )
    {
    CNcdNodeMetaData::ExternalizeL( aStream );
    }
    
void CNcdNodeItemMetaData::InternalizeL( RReadStream& aStream )
    {
    CNcdNodeMetaData::InternalizeL( aStream );
    }
    
void CNcdNodeItemMetaData::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    CNcdNodeMetaData::ExternalizeDataForRequestL( aStream );
    }

