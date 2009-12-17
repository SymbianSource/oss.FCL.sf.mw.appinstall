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
* Description:   Contains CNcdNodeUpgradeProxy class implementation
*
*/


#include "ncdnodeupgradeproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdnodemanagerproxy.h"
#include "ncdnodeproxy.h"
#include "ncdnodemanagerproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodedependencyinfoimpl.h"
#include "ncddependencyinfo.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "ncdnodeidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncderrors.h"
#include "ncdattributes.h"
#include "ncdnodeupgradeimpl.h" // TUpgradeData


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdNodeUpgradeProxy::CNcdNodeUpgradeProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    : CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata ),
      iNodeManager( aMetadata.Node().NodeManager() ),
      iUpgradeType( MNcdNodeUpgrade::EUpgradeNotAvailable )
    {
    }


void CNcdNodeUpgradeProxy::ConstructL()
    {
    // Register the interface
    MNcdNodeUpgrade* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeUpgrade::KInterfaceUid ) );

    // Let the construction leave if we do not get the data from the server.    
    InternalizeL();
    }


CNcdNodeUpgradeProxy* CNcdNodeUpgradeProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeUpgradeProxy* self = 
        CNcdNodeUpgradeProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeUpgradeProxy* CNcdNodeUpgradeProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeUpgradeProxy* self = 
        new( ELeave ) CNcdNodeUpgradeProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeUpgradeProxy::~CNcdNodeUpgradeProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeUpgrade::KInterfaceUid );
    
    
    // Delete member variables here
    // Do not delete manager because
    // this object does not own it.
    
    // Casting is safe here because only this class creates the objects
    // that are inserted into the array.
    ResetAndDestroyWithCast<CNcdNodeDependencyInfo>( iUpgradeTargets );
    
    delete iUpgradeData;
    }


CNcdNodeManagerProxy& CNcdNodeUpgradeProxy::NodeManager() const
    {
    return iNodeManager;
    }


void CNcdNodeUpgradeProxy::InternalizeL()
    {
    DLTRACEIN((""));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    // Get all the data that is necessary to internalize this object
    // from the server side.
    TInt error(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                       KNullDesC8,
                       data,
                       Handle(),
                       0 ) );

    if ( error == KNcdErrorObsolete )
        {
        DLINFO(("Upgrade was obsolete"));
        // Remove interfaces implemented by this class from the top parent interface list.
        // So, the interface list is up to date after this object is removed
        // from its top parent.
        RemoveInterface( MNcdNodeUpgrade::KInterfaceUid );
        // Remove from the parent
        RemoveFromParent();
        // Now update the interface for this object just in case somebody needs it.
        // Register the interface
        MNcdNodeUpgrade* interface( this );
        AddInterfaceL( 
            CCatalogsInterfaceIdentifier::NewL( interface, this, MNcdNodeUpgrade::KInterfaceUid ) );
        
        }
    else if ( error == KErrNotFound ) 
        {
        DLTRACE(("Upgrade not available anymore"));
        // Reset upgrade type so that the interface user knows that there's
        // no more upgrade available
        iUpgradeType = MNcdNodeUpgrade::EUpgradeNotAvailable;
        }
        
    // If error occurred during data transfer, leave here and forward the error.
    User::LeaveIfError( error );
    
    if ( data == NULL )
        {
        DLERROR((""));
        User::Leave(  KErrNotFound );
        }

     CleanupStack::PushL( data );

     // Read the data from the stream and insert it to the memeber variables
     RDesReadStream stream( *data );
     CleanupClosePushL( stream );
    
     InternalizeDataL( stream );
    
     // Closes the stream
     CleanupStack::PopAndDestroy( &stream ); 
     CleanupStack::PopAndDestroy( data );

    DLTRACEOUT((""));    
    }


// MNcdNodeUpgrade functions

const TDesC& CNcdNodeUpgradeProxy::Name() const
    {
    // Even though we may have multiple possibilities in the array.
    // The API function only wants one. So choose the first one here.
    if ( iUpgradeTargets.Count() == 0 )
        {        
        return KNullDesC();
        }
        
    return iUpgradeTargets[0]->Name();        
    }


TUid CNcdNodeUpgradeProxy::Uid() const
    {
    DLTRACEIN((""));
    
    // Upgrades some content, return uid for that content
    if ( iUpgradeData &&
         iUpgradeData->AttributeType( CNcdNodeUpgrade::EUpgradeDataUid ) == 
            CNcdAttributes::EAttributeTypeInt32 ) 
        {
        DLTRACEOUT(("Return UID for content upgrade"))
        return TUid::Uid( iUpgradeData->AttributeInt32( 
            CNcdNodeUpgrade::EUpgradeDataUid ) );
        }

    // Even though we may have multiple possibilities in the array.
    // The API function only wants one. So choose the first one here.

    if ( iUpgradeTargets.Count() == 0 )
        {
        return TUid::Null();
        }
        

    return iUpgradeTargets[0]->Uid();        
    }


const TDesC& CNcdNodeUpgradeProxy::Version() const
    {
    DLTRACEIN((""));
    if ( iUpgradeData &&
         iUpgradeData->AttributeType( CNcdNodeUpgrade::EUpgradeDataVersion ) == 
            CNcdAttributes::EAttributeTypeString16 ) 
        {
        DLTRACEOUT(("Return version for content upgrade"))
        return iUpgradeData->AttributeString16( 
            CNcdNodeUpgrade::EUpgradeDataVersion );
        }


    // Even though we may have multiple possibilities in the array.
    // The API function only wants one. So choose the first one here.

    if ( iUpgradeTargets.Count() == 0 )
        {
        return KNullDesC();
        }        

    return iUpgradeTargets[0]->Version();        
    }


MNcdNode* CNcdNodeUpgradeProxy::UpgradeableNodeL() const
    {
    DLTRACEIN((""));
    
    // Even though we may have multiple possibilities in the array.
    // The API function only wants one. So choose the first one here.

    if ( iUpgradeTargets.Count() == 0 )
        {
        return NULL;
        }
        
    return iUpgradeTargets[0]->DependencyNodeL();    
    }



MNcdNodeUpgrade::TUpgradeType CNcdNodeUpgradeProxy::UpgradeType() const
    {
    return iUpgradeType;
    }
    

// Other functions

const RPointerArray<MNcdNodeDependencyInfo>& CNcdNodeUpgradeProxy::UpgradeTargets() const
    {
    return iUpgradeTargets;
    }


void CNcdNodeUpgradeProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Use catalogsutils.h functions to internalize
    // memebervariables according to the data received
    // from the server.
    // Make sure that the variables are set here in the same
    // order as they are externalized in the server side.
    // Small mistake here messes up everything!

    // First read the class id. Because, it is the first thing in the stream.
    TInt classId( aStream.ReadInt32L() );
    
    if ( classId != NcdNodeClassIds::ENcdNodeUpgradeClassId )
        {
        // classId is not recognized
        DLERROR(("Class id was not recognized!"));
        // For testing purposes assert here
        DASSERT( EFalse );
        
        // Otherwise leave is adequate
        User::Leave( KErrCorrupt );
        }

    InternalizeUpgradeArrayL( aStream );

    delete iUpgradeData;
    iUpgradeData = NULL;
    
    TBool upgradeDataExists = aStream.ReadInt8L();
    if ( upgradeDataExists ) 
        {        
        iUpgradeData = CNcdAttributes::NewL( 
            aStream, 
            CNcdNodeUpgrade::EUpgradeDataInternal, 
            0 );
        }
    
    if ( !iUpgradeData && !iUpgradeTargets.Count() ) 
        {
        DLERROR(("Corrupt data, leaving"));
        User::Leave( KErrCorrupt );
        }
    
    RefreshUpgradeType();    
    DLTRACEOUT((""));
    }


void CNcdNodeUpgradeProxy::InternalizeUpgradeArrayL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Casting is safe here because only this class creates the objects
    // that are inserted into the array.
    ResetAndDestroyWithCast<CNcdNodeDependencyInfo>( iUpgradeTargets );
    
    // First get the node dependency information that is sent from the server
    // and then convert it to the proxy side dependency information object
    // that is inserted into the array.
    TInt count( aStream.ReadInt32L() );
    CNcdDependencyInfo* upgradeInfo( NULL );
    CNcdNodeDependencyInfo* info( NULL );
    for( TInt i = 0; i < count; ++i )
        {
        upgradeInfo = CNcdDependencyInfo::NewLC( aStream );
        info = CNcdNodeDependencyInfo::NewLC( *upgradeInfo, NodeManager() );
        iUpgradeTargets.AppendL( info );
        // Ownership was transferred to the array
        CleanupStack::Pop( info );
        CleanupStack::PopAndDestroy( upgradeInfo );
        }

    // Second get the content dependency information
    count = aStream.ReadInt32L();
    upgradeInfo = NULL;
    info = NULL;
    for( TInt i = 0; i < count; ++i )
        {
        upgradeInfo = CNcdDependencyInfo::NewLC( aStream );
        info = CNcdNodeDependencyInfo::NewLC( *upgradeInfo, NodeManager() );
        iUpgradeTargets.AppendL( info );
        CleanupStack::Pop( info );
        CleanupStack::PopAndDestroy( upgradeInfo );
        }

    DLTRACEOUT((""));
    }


void CNcdNodeUpgradeProxy::RefreshUpgradeType()
    {
    DLTRACEIN((""));
    if ( iUpgradeData ||
         !static_cast<CNcdNodeDependencyInfo*>( 
            iUpgradeTargets[0] )->Identifier() )
        {
        DLTRACE(("Upgrade content"));
        iUpgradeType = MNcdNodeUpgrade::EUpgradeContent;
        }
    else
        {
        DLTRACE(("Upgrade node"));
        iUpgradeType = MNcdNodeUpgrade::EUpgradeNode;
        }
    }
