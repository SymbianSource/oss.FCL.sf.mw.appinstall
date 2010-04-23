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
* Description:   CNcdDependencyInfo
*
*/


#include <s32strm.h>

#include "ncddependencyinfo.h"
#include "ncdnodeidentifier.h"
#include "catalogsutils.h"


CNcdDependencyInfo* CNcdDependencyInfo::NewL( const TDesC& aName, 
                                              const TDesC& aVersion, 
                                              const TDesC& aUid,
                                              CNcdNodeIdentifier* aIdentifier )
    {
    CNcdDependencyInfo* self = 
        CNcdDependencyInfo::NewLC( aName, aVersion, aUid, aIdentifier );
    CleanupStack::Pop( self );
    return self;            
    }
    
CNcdDependencyInfo* CNcdDependencyInfo::NewLC( const TDesC& aName, 
                                               const TDesC& aVersion, 
                                               const TDesC& aUid,
                                               CNcdNodeIdentifier* aIdentifier )
    {
    CleanupStack::PushL( aIdentifier ); 
    CNcdDependencyInfo* self = 
        new( ELeave ) CNcdDependencyInfo( aIdentifier );
    CleanupStack::Pop( aIdentifier );
    CleanupStack::PushL( self );
    self->ConstructL( aName, aVersion, aUid );
    return self;            
    }

void CNcdDependencyInfo::ConstructL( const TDesC& aName, 
                                     const TDesC& aVersion, 
                                     const TDesC& aUid )
    {
    DLTRACEIN((""));

    TInt uid( 0 );
    
    // Error is ignored on purpose, if the given text doesn't contain
    // a hex int then uid is set a 0
    TRAP_IGNORE( uid = DesHexToIntL( aUid ) );
    DLTRACE(( _L("aUid: %S was converted to %x"), &aUid, uid ));
    iUid.iUid = uid;

    iName = aName.AllocL();
    iVersion = aVersion.AllocL();        
    }


CNcdDependencyInfo* CNcdDependencyInfo::NewL( const TDesC& aName, 
                                              const TDesC& aVersion, 
                                              TUid aUid,
                                              CNcdNodeIdentifier* aIdentifier )
    {
    CNcdDependencyInfo* self = 
        CNcdDependencyInfo::NewLC( aName, aVersion, aUid, aIdentifier );
    CleanupStack::Pop( self );
    return self;            
    }
    
CNcdDependencyInfo* CNcdDependencyInfo::NewLC( const TDesC& aName, 
                                               const TDesC& aVersion, 
                                               TUid aUid,
                                               CNcdNodeIdentifier* aIdentifier )
    {
    CleanupStack::PushL( aIdentifier );    
    CNcdDependencyInfo* self = 
        new( ELeave ) CNcdDependencyInfo( aIdentifier );
    CleanupStack::Pop( aIdentifier );
    CleanupStack::PushL( self );
    self->ConstructL( aName, aVersion, aUid );
    return self;            
    }

void CNcdDependencyInfo::ConstructL( const TDesC& aName, 
                                     const TDesC& aVersion, 
                                     TUid aUid )
    {
    DLTRACEIN(("aUid: %x", aUid.iUid));
    
    iUid.iUid = aUid.iUid;

    iName = aName.AllocL();
    iVersion = aVersion.AllocL();
    }


CNcdDependencyInfo* CNcdDependencyInfo::NewL( RReadStream& aReadStream )
    {
    CNcdDependencyInfo* self = 
        CNcdDependencyInfo::NewLC( aReadStream );
    CleanupStack::Pop( self );
    return self;            
    }
    
CNcdDependencyInfo* CNcdDependencyInfo::NewLC( RReadStream& aReadStream )
    {
    CNcdDependencyInfo* self = 
        new( ELeave ) CNcdDependencyInfo( NULL );
    CleanupStack::PushL( self );
    self->ConstructL( aReadStream );
    return self;            
    }

void CNcdDependencyInfo::ConstructL( RReadStream& aReadStream )
    {
    InternalizeL( aReadStream );
    }


CNcdDependencyInfo::CNcdDependencyInfo( CNcdNodeIdentifier* aIdentifier ): 
    CBase(), 
    iIdentifier( aIdentifier ),
    iDependencyState( ENcdDependencyMissing )
    {
    
    }
    
CNcdDependencyInfo::~CNcdDependencyInfo()
    {
    delete iName;
    delete iVersion;
    delete iIdentifier;
    }


const TDesC& CNcdDependencyInfo::Name() const
    {
    return *iName;
    }    

const TDesC& CNcdDependencyInfo::Version() const
    {
    return *iVersion;
    }
    
TUid CNcdDependencyInfo::Uid() const
    {
    return iUid;
    }


const CNcdNodeIdentifier* CNcdDependencyInfo::Identifier() const
    {
    return iIdentifier;
    }


void CNcdDependencyInfo::SetDependencyState( TNcdDependencyState aState )
    {
    DLTRACEIN(("aState: %d", aState));
    iDependencyState = aState;
    }


TNcdDependencyState CNcdDependencyInfo::DependencyState() const
    {
    DLTRACEIN(("Current state: %d", iDependencyState));
    return iDependencyState;
    }


void CNcdDependencyInfo::InternalizeL( RReadStream& aReadStream )
    {
    DLTRACEIN((""));
    InternalizeDesL( iName, aReadStream );
    InternalizeDesL( iVersion, aReadStream );
    iUid.iUid = aReadStream.ReadInt32L();
    delete iIdentifier;
    iIdentifier = NULL;
    
    if ( aReadStream.ReadInt8L() )
        {
        iIdentifier = CNcdNodeIdentifier::NewL( aReadStream );
        }
        
    iDependencyState = static_cast<TNcdDependencyState>( 
        aReadStream.ReadInt32L() );
    }
    

void CNcdDependencyInfo::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    DLTRACEIN((""));
    ExternalizeDesL( Name(), aWriteStream );
    ExternalizeDesL( Version(), aWriteStream );
    aWriteStream.WriteInt32L( Uid().iUid );
    if ( Identifier() == NULL )
        {
        aWriteStream.WriteInt8L( EFalse );
        }
    else
        {
        aWriteStream.WriteInt8L( ETrue );
        Identifier()->ExternalizeL( aWriteStream );
        }
    aWriteStream.WriteInt32L( iDependencyState );
    }
