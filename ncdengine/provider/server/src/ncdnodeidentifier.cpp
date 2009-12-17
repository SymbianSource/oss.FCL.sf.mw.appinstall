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
* Description:   Implements CNcdNodeIdentifier class
*
*/


#include <e32cmn.h>
#include <s32strm.h>
#include <s32mem.h>

#include "ncdnodeidentifier.h"
#include "ncdproviderdefines.h"
#include "catalogsutils.h"
#include "catalogsconstants.h"

// CNcdNodeIdentifier uses different implementation when compiled for the server than
// in other places
#ifdef COMPONENT_CATALOGSSERVEREXE 

#include "catalogsrefstring.h"
#include "catalogsstringmanager.h"

// For memory consumption logging
#ifdef LOG_MEMORY_USAGE
TInt64 CNcdNodeIdentifier::iCurrentMemory = 0;
TInt64 CNcdNodeIdentifier::iMaxMemory = 0;
TInt CNcdNodeIdentifier::iIdCount = 0;
TInt CNcdNodeIdentifier::iMaxCount = 0;

    
void CNcdNodeIdentifier::AddId( const CNcdNodeIdentifier& /*aId*/, TInt64& aMemory )
    {

    aMemory = 
/*
        aId.NodeId().Size() + 
        aId.NodeNameSpace().Size() +
        aId.ServerUri().Size() +        */
        sizeof( CNcdNodeIdentifier ) -
        sizeof( TInt64 ); // ignore the size of this temporary member
    
    iCurrentMemory += aMemory;
    if ( iCurrentMemory > iMaxMemory ) 
        {
        iMaxMemory = iCurrentMemory;
        }        
  
    iIdCount++;
    if ( iIdCount > iMaxCount ) 
        {
        iMaxCount = iIdCount;
        }    
    DLTRACE(("Memory: %Ld kB, max: %Ld kB, count: %d, max: %d", 
        iCurrentMemory >> 10, iMaxMemory >> 10, iIdCount, iMaxCount ));       
    }
    
    
void CNcdNodeIdentifier::RemoveId( const CNcdNodeIdentifier& /*aId*/, const TInt64& aMemory )
    {
    iIdCount--;

   
    iCurrentMemory -= aMemory;
    DLTRACE(("Memory: %Ld, max: %Ld, count: %d, max: %d", 
        iCurrentMemory, iMaxMemory, iIdCount, iMaxCount ));    
    
    }
#endif // LOG_MEMORY_USAGE




CNcdNodeIdentifier::CNcdNodeIdentifier(): CBase()
    {
    }


CNcdNodeIdentifier::CNcdNodeIdentifier( const CNcdNodeIdentifier& aId ) 
    : CBase(),
    iNodeNameSpace( aId.iNodeNameSpace ),
    iNodeId( aId.iNodeId ), 
    iServerUri( aId.iServerUri ),
    iClientUid( aId.iClientUid )    
    {
    iNodeNameSpace->AddRef();
    iNodeId->AddRef();
    iServerUri->AddRef();
    }


CNcdNodeIdentifier::~CNcdNodeIdentifier()
    {   
#ifdef LOG_MEMORY_USAGE
    if ( iMyMemory ) 
        {        
        RemoveId( *this, iMyMemory );
        }
#endif    
    
    ReleaseStrings();
    }


void CNcdNodeIdentifier::ReleaseStrings()
    {
    if ( iNodeNameSpace ) 
        {
        CCatalogsStringManager::StringManager().ReleaseString( 
            NcdProviderDefines::ENcdStringNamespace,        
            *iNodeNameSpace );
        iNodeNameSpace = NULL;
        }

    if ( iNodeId ) 
        {
        CCatalogsStringManager::StringManager().ReleaseString( 
            NcdProviderDefines::ENcdStringNodeId,        
            *iNodeId );
        iNodeId = NULL;
        }

    if ( iServerUri ) 
        {
        CCatalogsStringManager::StringManager().ReleaseString( 
            NcdProviderDefines::ENcdStringServerUri,        
            *iServerUri );
        iServerUri = NULL;
        }

    }

void CNcdNodeIdentifier::ConstructL( const TDesC& aNodeNameSpace,
                                     const TDesC& aNodeId,
                                     const TDesC& aServerUri,
                                     const TUid& aClientUid )
    {
    iNodeNameSpace = &CCatalogsStringManager::StringManager().StringL( 
        NcdProviderDefines::ENcdStringNamespace,
        aNodeNameSpace );
        
    iNodeId = &CCatalogsStringManager::StringManager().StringL( 
        NcdProviderDefines::ENcdStringNodeId, 
        aNodeId );
        
    iServerUri = &CCatalogsStringManager::StringManager().StringL(
        NcdProviderDefines::ENcdStringServerUri,
        aServerUri );
    
    iClientUid = aClientUid;    

#ifdef LOG_MEMORY_USAGE
    AddId( *this, iMyMemory );
#endif    
    }
    

void CNcdNodeIdentifier::ConstructL( const TDesC8& aNodeIdentifierData )
    {
    RDesReadStream readStream( aNodeIdentifierData );
    CleanupClosePushL( readStream );
    InternalizeL( readStream );
    CleanupStack::PopAndDestroy( &readStream );

#ifdef LOG_MEMORY_USAGE
    AddId( *this, iMyMemory );
#endif    
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL()
    {
    CNcdNodeIdentifier* self = 
        CNcdNodeIdentifier::NewLC();
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC()
    {
    CNcdNodeIdentifier* self = 
        new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL( KNullDesC(), KNullDesC(), KNullDesC(), TUid::Null() );
    return self;        
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL( const TDesC& aNodeNameSpace,
                                              const TDesC& aNodeId,
                                              const TUid& aClientUid )
    {
    CNcdNodeIdentifier* self = 
        CNcdNodeIdentifier::NewLC(
            aNodeNameSpace,
            aNodeId,
            aClientUid );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC( const TDesC& aNodeNameSpace,
                                               const TDesC& aNodeId,
                                               const TUid& aClientUid )
    {
    CNcdNodeIdentifier* self = 
        new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL( aNodeNameSpace, aNodeId, KNullDesC(), aClientUid );
    return self;        
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL( const TDesC& aNodeNameSpace,
                                              const TDesC& aNodeId,
                                              const TDesC& aServerUri,
                                              const TUid& aClientUid )
    {
    CNcdNodeIdentifier* self = 
        CNcdNodeIdentifier::NewLC(
            aNodeNameSpace,
            aNodeId,
            aServerUri,
            aClientUid );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC( const TDesC& aNodeNameSpace,
                                               const TDesC& aNodeId,
                                               const TDesC& aServerUri,
                                               const TUid& aClientUid )
    {
    CNcdNodeIdentifier* self = 
        new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL( aNodeNameSpace, aNodeId, aServerUri, aClientUid );
    return self;
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL(
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    CNcdNodeIdentifier* self = 
        CNcdNodeIdentifier::NewLC( aNodeIdentifier );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC(
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    CNcdNodeIdentifier* self = 
        new( ELeave ) CNcdNodeIdentifier( aNodeIdentifier );
    CleanupStack::PushL( self );
    return self;
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL( const TDesC8& aNodeIdentifierData )
    {
    CNcdNodeIdentifier* self = 
        CNcdNodeIdentifier::NewLC( aNodeIdentifierData );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC( const TDesC8& aNodeIdentifierData )
    {
    CNcdNodeIdentifier* self = new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL( aNodeIdentifierData );
    return self;        
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL( RReadStream& aReadStream )
    {
    CNcdNodeIdentifier* self = NewLC( aReadStream );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC( RReadStream& aReadStream )
    {
    CNcdNodeIdentifier* self = new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->InternalizeL( aReadStream );
    return self;        
    }


void CNcdNodeIdentifier::InternalizeL( RReadStream& aReadStream )
    {
    HBufC* temp = NULL;
    InternalizeDesL( temp, aReadStream );    

    
    // StringL handles the ownership of temp
    const CCatalogsRefString& nameSpace = 
        CCatalogsStringManager::StringManager().StringL( 
            NcdProviderDefines::ENcdStringNamespace,
            temp );
    
    CCatalogsStringManager::StringManager().ReleaseString(
        NcdProviderDefines::ENcdStringNamespace,
        iNodeNameSpace );
    
    iNodeNameSpace = &nameSpace;
    
    temp = NULL;
    
    InternalizeDesL( temp, aReadStream );
    
    
    const CCatalogsRefString& nodeId = 
        CCatalogsStringManager::StringManager().StringL( 
            NcdProviderDefines::ENcdStringNodeId,
            temp );


    CCatalogsStringManager::StringManager().ReleaseString(
        NcdProviderDefines::ENcdStringNodeId,
        iNodeId );
    
    iNodeId = &nodeId;
    
    temp = NULL;
    
    InternalizeDesL( temp, aReadStream );
    
    const CCatalogsRefString& serverUri = 
        CCatalogsStringManager::StringManager().StringL( 
            NcdProviderDefines::ENcdStringServerUri,
            temp );    

    CCatalogsStringManager::StringManager().ReleaseString(
        NcdProviderDefines::ENcdStringServerUri,
        iServerUri );

    iServerUri = &serverUri;          
   
    iClientUid.iUid = aReadStream.ReadInt32L();      
    }


void CNcdNodeIdentifier::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    ExternalizeDesL( NodeNameSpace(), aWriteStream );
    ExternalizeDesL( NodeId(), aWriteStream );
    ExternalizeDesL( ServerUri(), aWriteStream );
    aWriteStream.WriteInt32L( iClientUid.iUid );    
    }
 
        
const TDesC& CNcdNodeIdentifier::NodeNameSpace() const
    {
    DASSERT( iNodeNameSpace );
    return *iNodeNameSpace;
    }    

const TDesC& CNcdNodeIdentifier::NodeId() const
    {
    DASSERT( iNodeId );
    return *iNodeId;
    }

const TDesC& CNcdNodeIdentifier::ServerUri() const
    {
    DASSERT( iServerUri );
    return *iServerUri;
    }

const TUid& CNcdNodeIdentifier::ClientUid() const
    {
    return iClientUid;
    }


HBufC8* CNcdNodeIdentifier::NodeIdentifierDataL() const
    {
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    // Get the data of this identifier into the stream which
    // directs the data into the buffer.
    ExternalizeL( stream );

    // Now data has been externalized into the buffer.
    // Create copy of the data and return it.
    HBufC8* data = buf->Ptr( 0 ).AllocL();
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buf );
    return data;
    }
    

TBool CNcdNodeIdentifier::ContainsEmptyFields(
    TBool aCheckNameSpace,
    TBool aCheckId,
    TBool aCheckUri,
    TBool aCheckUid ) const
    {
    return ( aCheckNameSpace && NodeNameSpace() == KNullDesC() )
        || ( aCheckId && NodeId() == KNullDesC() )
        || ( aCheckUri && ServerUri() == KNullDesC() )
        || ( ClientUid() == TUid::Null() && aCheckUid );
    }


TBool CNcdNodeIdentifier::Equals( 
    const CNcdNodeIdentifier& aObject,
    TBool aCompareNameSpace,
    TBool aCompareId,
    TBool aCompareUri,
    TBool aCompareUid ) const
    {
    // Use pointer comparison for strings
    return ( iNodeNameSpace == aObject.iNodeNameSpace
                || !aCompareNameSpace ) 
           && ( iNodeId == aObject.iNodeId
                || !aCompareId )
           && ( iServerUri == aObject.iServerUri 
                || !aCompareUri )
           && ( ClientUid() == aObject.ClientUid() 
                || !aCompareUid );

    }

TBool CNcdNodeIdentifier::Equals( 
    const CNcdNodeIdentifier& aObject1,
    const CNcdNodeIdentifier& aObject2,
    TBool aCompareNameSpace,
    TBool aCompareId,
    TBool aCompareUri,
    TBool aCompareUid )
    {
    return aObject1.Equals( 
        aObject2, 
        aCompareNameSpace, 
        aCompareId,
        aCompareUri,
        aCompareUid );
    }


TInt CNcdNodeIdentifier::Compare( 
    const CNcdNodeIdentifier& aObject1,
    const CNcdNodeIdentifier& aObject2 )
    {
    // First compare pointers, if they differ compare strings
    TInt comp = ( aObject1.iNodeId != aObject2.iNodeId );
    if ( comp ) 
        {
        comp = aObject1.NodeId().Compare( aObject2.NodeId() );    
        }
    
    if ( !comp ) 
        {
        comp = ( aObject1.iNodeNameSpace != aObject2.iNodeNameSpace );
        if ( comp ) 
            {            
            comp = aObject1.NodeNameSpace().Compare( aObject2.NodeNameSpace() );
            }
            
        if ( !comp ) 
            {
            comp = aObject1.ClientUid().iUid - aObject2.ClientUid().iUid;
            }
        }
    return comp;        
    }


TInt CNcdNodeIdentifier::CompareOrderByUid( 
    const CNcdNodeIdentifier& aObject1,
    const CNcdNodeIdentifier& aObject2 )
    {
    TInt comp = aObject1.ClientUid().iUid - aObject2.ClientUid().iUid;
        
    if ( !comp ) 
        {
        comp = aObject1.NodeNameSpace().Compare( aObject2.NodeNameSpace() );
        if ( !comp ) 
            {
            comp = aObject1.NodeId().Compare( aObject2.NodeId() );            
            }
        }
    return comp;        
    }



void CNcdNodeIdentifier::SetNodeIdL( const TDesC& aNodeId )
    {    
    const CCatalogsRefString& id = 
        CCatalogsStringManager::StringManager().StringL( 
            NcdProviderDefines::ENcdStringNodeId,
            aNodeId );
    
    CCatalogsStringManager::StringManager().ReleaseString(
        NcdProviderDefines::ENcdStringNodeId,
        iNodeId );
    
    iNodeId = &id;
    }




#else // COMPONENT_CATALOGSSERVEREXE ********************************************************






CNcdNodeIdentifier::CNcdNodeIdentifier(): CBase()
    {
    }

CNcdNodeIdentifier::~CNcdNodeIdentifier()
    {   
    delete iNodeNameSpace;
    iNodeNameSpace = NULL;
    
    delete iNodeId;
    iNodeId = NULL;

    delete iServerUri;
    iServerUri = NULL;
    }


void CNcdNodeIdentifier::ConstructL( const TDesC& aNodeNameSpace,
                                     const TDesC& aNodeId,
                                     const TDesC& aServerUri,
                                     const TUid& aClientUid )
    {
    iNodeNameSpace = aNodeNameSpace.AllocL();
    iNodeId = aNodeId.AllocL();
    iServerUri = aServerUri.AllocL();
    iClientUid = aClientUid;    
    }

void CNcdNodeIdentifier::ConstructL( const TDesC8& aNodeIdentifierData )
    {
    RDesReadStream readStream( aNodeIdentifierData );
    CleanupClosePushL( readStream );
    InternalizeL( readStream );
    CleanupStack::PopAndDestroy( &readStream );
    }



CNcdNodeIdentifier* CNcdNodeIdentifier::NewL()
    {
    CNcdNodeIdentifier* self = 
        CNcdNodeIdentifier::NewLC();
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC()
    {
    CNcdNodeIdentifier* self = 
        new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL( KNullDesC(), KNullDesC(), KNullDesC(), TUid::Null() );
    return self;        
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL( const TDesC& aNodeNameSpace,
                                              const TDesC& aNodeId,
                                              const TUid& aClientUid )
    {
    CNcdNodeIdentifier* self = 
        CNcdNodeIdentifier::NewLC(
            aNodeNameSpace,
            aNodeId,
            aClientUid );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC( const TDesC& aNodeNameSpace,
                                               const TDesC& aNodeId,
                                               const TUid& aClientUid )
    {
    CNcdNodeIdentifier* self = 
        new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL( aNodeNameSpace, aNodeId, KNullDesC(), aClientUid );
    return self;        
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL( const TDesC& aNodeNameSpace,
                                              const TDesC& aNodeId,
                                              const TDesC& aServerUri,
                                              const TUid& aClientUid )
    {
    CNcdNodeIdentifier* self = 
        CNcdNodeIdentifier::NewLC(
            aNodeNameSpace,
            aNodeId,
            aServerUri,
            aClientUid );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC( const TDesC& aNodeNameSpace,
                                               const TDesC& aNodeId,
                                               const TDesC& aServerUri,
                                               const TUid& aClientUid )
    {
    CNcdNodeIdentifier* self = 
        new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL( aNodeNameSpace, aNodeId, aServerUri, aClientUid );
    return self;
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL(
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    CNcdNodeIdentifier* self = 
        CNcdNodeIdentifier::NewLC( aNodeIdentifier );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC(
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    CNcdNodeIdentifier* self = 
        new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL(
        aNodeIdentifier.NodeNameSpace(),
        aNodeIdentifier.NodeId(),
        aNodeIdentifier.ServerUri(),
        aNodeIdentifier.ClientUid() );
    return self;
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL( const TDesC8& aNodeIdentifierData )
    {
    CNcdNodeIdentifier* self = 
        CNcdNodeIdentifier::NewLC( aNodeIdentifierData );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC( const TDesC8& aNodeIdentifierData )
    {
    CNcdNodeIdentifier* self = new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL( aNodeIdentifierData );
    return self;        
    }


CNcdNodeIdentifier* CNcdNodeIdentifier::NewL( RReadStream& aReadStream )
    {
    CNcdNodeIdentifier* self = NewLC( aReadStream );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeIdentifier* CNcdNodeIdentifier::NewLC( RReadStream& aReadStream )
    {
    CNcdNodeIdentifier* self = new( ELeave ) CNcdNodeIdentifier();
    CleanupStack::PushL( self );
    self->InternalizeL( aReadStream );
    return self;        
    }


void CNcdNodeIdentifier::InternalizeL( RReadStream& aReadStream )
    {    
    InternalizeDesL( iNodeNameSpace, aReadStream );
    InternalizeDesL( iNodeId, aReadStream );
    InternalizeDesL( iServerUri, aReadStream );
    iClientUid.iUid = aReadStream.ReadInt32L();    
    }

void CNcdNodeIdentifier::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    ExternalizeDesL( *iNodeNameSpace, aWriteStream );
    ExternalizeDesL( *iNodeId, aWriteStream );
    ExternalizeDesL( *iServerUri, aWriteStream );
    aWriteStream.WriteInt32L( iClientUid.iUid );    
    }
 
        
const TDesC& CNcdNodeIdentifier::NodeNameSpace() const
    {
    return *iNodeNameSpace;
    }    

const TDesC& CNcdNodeIdentifier::NodeId() const
    {
    return *iNodeId;
    }

const TDesC& CNcdNodeIdentifier::ServerUri() const
    {
    return *iServerUri;
    }

const TUid& CNcdNodeIdentifier::ClientUid() const
    {
    return iClientUid;
    }


HBufC8* CNcdNodeIdentifier::NodeIdentifierDataL() const
    {
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    // Get the data of this identifier into the stream which
    // directs the data into the buffer.
    ExternalizeL( stream );

    // Now data has been externalized into the buffer.
    // Create copy of the data and return it.
    HBufC8* data = buf->Ptr( 0 ).AllocL();
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buf );
    return data;
    }

TBool CNcdNodeIdentifier::ContainsEmptyFields(
    TBool aCheckNameSpace,
    TBool aCheckId,
    TBool aCheckUri,
    TBool aCheckUid ) const
    {
    return ( aCheckNameSpace && NodeNameSpace() == KNullDesC() )
        || ( aCheckId && NodeId() == KNullDesC() )
        || ( aCheckUri && ServerUri() == KNullDesC() )
        || ( ClientUid() == TUid::Null() && aCheckUid );
    }


TBool CNcdNodeIdentifier::Equals( 
    const CNcdNodeIdentifier& aObject,
    TBool aCompareNameSpace,
    TBool aCompareId,
    TBool aCompareUri,
    TBool aCompareUid ) const
    {
    return ( NodeNameSpace() == aObject.NodeNameSpace() 
                || !aCompareNameSpace ) 
           && ( NodeId() == aObject.NodeId() 
                || !aCompareId )
           // By default aCompareUri is EFalse so this order is a little
           // bit faster in default cases
           && ( !aCompareUri || 
                ServerUri() == aObject.ServerUri() )
           && ( ClientUid() == aObject.ClientUid() 
                || !aCompareUid );
    }

TBool CNcdNodeIdentifier::Equals( 
    const CNcdNodeIdentifier& aObject1,
    const CNcdNodeIdentifier& aObject2,
    TBool aCompareNameSpace,
    TBool aCompareId,
    TBool aCompareUri,
    TBool aCompareUid )
    {
    return ( aObject1.NodeNameSpace() == aObject2.NodeNameSpace()
                || !aCompareNameSpace )
           && ( aObject1.NodeId() == aObject2.NodeId()
                || !aCompareId )
           // By default aCompareUri is EFalse so this order is a little
           // bit faster in default cases
           && ( !aCompareUri ||
                aObject1.ServerUri() == aObject2.ServerUri() )
           && ( aObject1.ClientUid() == aObject2.ClientUid()
                || !aCompareUid );
    }


TInt CNcdNodeIdentifier::Compare( 
    const CNcdNodeIdentifier& aObject1,
    const CNcdNodeIdentifier& aObject2 )
    {
    TInt comp = aObject1.NodeId().Compare( aObject2.NodeId() );    
    
    if ( !comp ) 
        {
        comp = aObject1.NodeNameSpace().Compare( aObject2.NodeNameSpace() );
        if ( !comp ) 
            {
            comp = aObject1.ClientUid().iUid - aObject2.ClientUid().iUid;
            }
        }
    return comp;        
    }


TInt CNcdNodeIdentifier::CompareOrderByUid( 
    const CNcdNodeIdentifier& aObject1,
    const CNcdNodeIdentifier& aObject2 )
    {
    TInt comp = aObject1.ClientUid().iUid - aObject2.ClientUid().iUid;
        
    if ( !comp ) 
        {
        comp = aObject1.NodeNameSpace().Compare( aObject2.NodeNameSpace() );
        if ( !comp ) 
            {
            comp = aObject1.NodeId().Compare( aObject2.NodeId() );            
            }
        }
    return comp;        
    }

#endif
    
