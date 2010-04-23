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
* Description:   Implements CNcdNodeFolderLink class
*
*/


#include "ncdnodefolderlink.h"
#include "ncd_pp_folderref.h"
#include "catalogsdebug.h"


CNcdNodeFolderLink* CNcdNodeFolderLink::NewL( CNcdNode& aNode )
    {
    CNcdNodeFolderLink* self = 
        CNcdNodeFolderLink::NewLC( aNode );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeFolderLink* CNcdNodeFolderLink::NewLC( CNcdNode& aNode )
    {
    CNcdNodeFolderLink* self = new( ELeave ) CNcdNodeFolderLink( aNode );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeFolderLink::CNcdNodeFolderLink( 
    CNcdNode& aNode,
    NcdNodeClassIds::TNcdNodeClassId aClassId )
: CNcdNodeLink( aNode, aClassId )
    {
    }

void CNcdNodeFolderLink::ConstructL()
    {
    CNcdNodeLink::ConstructL();
    }

CNcdNodeFolderLink::~CNcdNodeFolderLink()
    {
    }        
    

TInt CNcdNodeFolderLink::ExpectedChildrenCount() const
    {
    DLTRACEIN(( "iExpectedChildrenCount: %d", iExpectedChildrenCount ));

    return iExpectedChildrenCount;
    }


void CNcdNodeFolderLink::InternalizeL( const MNcdPreminetProtocolEntityRef& aData,
                                       const CNcdNodeIdentifier& aParentIdentifier,
                                       const CNcdNodeIdentifier& aRequestParentIdentifier,
                                       const TUid& aClientUid )
    {
    DLTRACEIN((""));

    if( aData.Type() != MNcdPreminetProtocolEntityRef::EFolderRef )
        {
        DLERROR(("Wrong type"));
        DASSERT( EFalse );
        // The data should be for the folder
        User::Leave( KErrArgument );
        }
    
    // First internalize parent stuff
    CNcdNodeLink::InternalizeL( 
        aData, aParentIdentifier, aRequestParentIdentifier, aClientUid );

    // Safe to cast because the type was checked in the beginning
    // of this function.
    const MNcdPreminetProtocolFolderRef& castedData =
        static_cast<const MNcdPreminetProtocolFolderRef&>( aData );
        
    if( castedData.Count() != MNcdPreminetProtocolFolderRef::KValueNotSet )
        {
        DLTRACE(("Child count set -> change it, previous count: %d new count: %d",
            iExpectedChildrenCount, castedData.Count() ));
        iExpectedChildrenCount = castedData.Count();
        }

    DLTRACEOUT((""));
    }

    
void CNcdNodeFolderLink::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // First use the parent to externalize the general data
    CNcdNodeLink::ExternalizeL( aStream );
    
    // Now externalize the data of this specific class
    aStream.WriteInt32L( iExpectedChildrenCount );

    DLTRACEOUT((""));
    }
    
void CNcdNodeFolderLink::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // First use the parent to internalize the general data
    CNcdNodeLink::InternalizeL( aStream );
    
    // Now internalize the data of this specific class
    iExpectedChildrenCount = aStream.ReadInt32L();

    DLTRACEOUT((""));
    }

    
void CNcdNodeFolderLink::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    // First use the parent to set the data into the stream
    CNcdNodeLink::ExternalizeDataForRequestL( aStream );
    
    // Then, insert the folder specific data.
    
    aStream.WriteInt32L( iExpectedChildrenCount );
    }
