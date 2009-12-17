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
* Description:   Implements CNcdNodeFolderMetaData class
*
*/


#include "ncdnodefoldermetadata.h"
#include "ncdnodeclassids.h"
#include "ncd_pp_dataentity.h"


CNcdNodeFolderMetaData* CNcdNodeFolderMetaData::NewL( const CNcdNodeIdentifier& aIdentifier,
                                                      CNcdNodeManager& aNodeManager )
    {
    CNcdNodeFolderMetaData* self = 
        CNcdNodeFolderMetaData::NewLC( aIdentifier, aNodeManager );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeFolderMetaData* CNcdNodeFolderMetaData::NewLC( const CNcdNodeIdentifier& aIdentifier,
                                                       CNcdNodeManager& aNodeManager )
    {
    CNcdNodeFolderMetaData* self = 
        new( ELeave ) CNcdNodeFolderMetaData( NcdNodeClassIds::ENcdFolderNodeMetaDataClassId,
                                              aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;        
    }


CNcdNodeFolderMetaData::CNcdNodeFolderMetaData( NcdNodeClassIds::TNcdNodeClassId aClassId,
                                                CNcdNodeManager& aNodeManager )
: CNcdNodeMetaData( aClassId, aNodeManager )
    {
    }

void CNcdNodeFolderMetaData::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdNodeMetaData::ConstructL( aIdentifier );
    }

CNcdNodeFolderMetaData::~CNcdNodeFolderMetaData()
    {
    }        
    

void CNcdNodeFolderMetaData::InternalizeL( MNcdPreminetProtocolDataEntity& aData )
    {
    // First use the parent to internalize the general data
    CNcdNodeMetaData::InternalizeL( aData );
    }

void CNcdNodeFolderMetaData::ExternalizeL( RWriteStream& aStream )
    {
    // First use the parent to externalize the general data
    CNcdNodeMetaData::ExternalizeL( aStream );
    }
    
void CNcdNodeFolderMetaData::InternalizeL( RReadStream& aStream )
    {
    // First use the parent to internalize the general data
    CNcdNodeMetaData::InternalizeL( aStream );
    }
        
void CNcdNodeFolderMetaData::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    CNcdNodeMetaData::ExternalizeDataForRequestL( aStream );
    }
