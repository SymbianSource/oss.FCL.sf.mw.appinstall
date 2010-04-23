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
* Description:   Implements CNcdNodeUpgrade class
*
*/


#include "ncdnodeupgradeimpl.h"

#include "ncdnodemetadataimpl.h"
#include "ncddownloadinfo.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_pp_dataentitycontent.h"
#include "ncd_pp_entitydependency.h"
#include "ncd_cp_query.h"
#include "ncdnodeidentifier.h"
#include "ncddependencyinfo.h"
#include "catalogsutils.h"
#include "ncderrors.h"
#include "ncdproviderutils.h"
#include "ncdattributes.h"

#include "catalogsdebug.h"


CNcdNodeUpgrade::CNcdNodeUpgrade( CNcdNodeMetaData& aParentMetaData, 
                                  NcdNodeClassIds::TNcdNodeClassId aClassId )
: CNcdCommunicable(),
  iParentMetaData( aParentMetaData ),
  iClassId( aClassId )
    {
    }

void CNcdNodeUpgrade::ConstructL()
    {
    // These values have to be set.
    iUpgradeData = CNcdAttributes::NewL( EUpgradeDataInternal );
    }


CNcdNodeUpgrade* CNcdNodeUpgrade::NewL( CNcdNodeMetaData& aParentMetaData )
    {
    CNcdNodeUpgrade* self =   
        CNcdNodeUpgrade::NewLC( aParentMetaData );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeUpgrade* CNcdNodeUpgrade::NewLC( CNcdNodeMetaData& aParentMetaData )
    {
    CNcdNodeUpgrade* self = 
        new( ELeave ) CNcdNodeUpgrade( 
            aParentMetaData,
            NcdNodeClassIds::ENcdNodeUpgradeClassId );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeUpgrade::~CNcdNodeUpgrade()
    {
    DLTRACEIN((""));

    // Delete member variables here
    iUpgradeNodeTargets.ResetAndDestroy();
    iUpgradeContentTargets.ResetAndDestroy();
    delete iUpgradeData;
    // Do not delete parent metadata because it is not owned here.

    DLTRACEOUT((""));
    }        


NcdNodeClassIds::TNcdNodeClassId CNcdNodeUpgrade::ClassId() const
    {
    return iClassId;
    }


const RPointerArray<CNcdDownloadInfo>& CNcdNodeUpgrade::ContentTargets() const
    {
    return iUpgradeContentTargets;
    }


const RPointerArray<CNcdDependencyInfo>& CNcdNodeUpgrade::NodeTargets() const
    {
    return iUpgradeNodeTargets;
    }


// Internalization from the protocol

void CNcdNodeUpgrade::InternalizeL( 
    MNcdPreminetProtocolDataEntity& aData,
    const TDesC& aVersion )
    {
    DLTRACEIN((""));

    // First create the new values

    // Notice that ownerships are not transferred from the parser objects.
    const MNcdPreminetProtocolDataEntityContent* downloadableContent = 
        aData.DownloadableContent();
    if ( downloadableContent == NULL )
        {
        DLINFO(("No data present"));
        // Content data not present.
        User::Leave( KErrNotFound );
        }

    // Reset the array here.
    // If something here fails after this. Then, the owner of this class object 
    // should remove this class object from the metadata anyway.
    iUpgradeNodeTargets.ResetAndDestroy();

    const MNcdPreminetProtocolEntityDependency* dependency( NULL );
    CNcdNodeIdentifier* identifier( NULL );
    CNcdDependencyInfo* upgradeInfo( NULL );    
    for ( TInt i = 0; i < downloadableContent->EntityDependencyCount(); ++i )
        {
        // Check what nodes depend on this node.
        dependency = &downloadableContent->EntityDependencyL( i );
        if ( dependency->Type() == EUpgradeDependency )
            {
            // Check if the upgrade should be loaded as a node when
            // the download responsibility is transferred to the proxy side
            // or should we just download the content directly in server side.
            if ( dependency->EntityId() != KNullDesC )
                {
                DLINFO((_L("Upgrade entity id: %S"), &dependency->EntityId()));
                // The dependecy information contains  the
                // entity id of the target node.
                identifier = 
                    CNcdNodeIdentifier::NewL( iParentMetaData.Identifier().NodeNameSpace(),
                                              dependency->EntityId(),                                           
                                              iParentMetaData.Identifier().ServerUri(),
                                              iParentMetaData.Identifier().ClientUid() );
                // Note, that identifier ownership is transferred here.
                upgradeInfo = 
                    CNcdDependencyInfo::NewLC( dependency->Name(),
                                               aVersion,
                                               dependency->ContentId(),
                                               identifier );
                iUpgradeNodeTargets.AppendL( upgradeInfo );
                CleanupStack::Pop( upgradeInfo );   

                }
            else
                {
                // Ownership is not transferred here.
                const MNcdPreminetProtocolDownload* downloadDetails( 
                    dependency->DownloadDetails() );
                if ( downloadDetails != NULL )
                    {
                    DLINFO(("Download the content directly. New node will not be created."));
                    // Copy the content object and insert it into the content array.
                    // The purchase operation may use the array content when checking
                    // if upgrades should be downloaded and installed.
                    CNcdDownloadInfo* content( 
                        CNcdDownloadInfo::NewLC( *dependency ) );
                    iUpgradeContentTargets.AppendL( content );
                    content->SetContentUsage( MNcdPurchaseDownloadInfo::EUpgrade );
                    // Array took ownership. So, do not delete.
                    CleanupStack::Pop( content );                    
                    }
                }
            }
        }

    if ( iUpgradeNodeTargets.Count() == 0
         && iUpgradeContentTargets.Count() == 0 )
        {
        // No upgrade data was found. 
        // So, inform about that to the owner of this class object.
        DLINFO(("No content"));
        User::Leave( KErrNotFound );
        }

    DLTRACEOUT((""));
    }



TBool CNcdNodeUpgrade::AllUpgradesInstalledL() const
    {
    DLTRACEIN(("Checking %d upgrades", 
        iUpgradeContentTargets.Count() ));
        
    // Check if the upgrades have already been installed
    // Note that missing app is also considered as installed because
    // we can't upgrade "nothing" :)
    for ( TInt i = 0; i < iUpgradeContentTargets.Count(); ++i ) 
        {
        if ( CNcdProviderUtils::IsApplicationInstalledL( 
                iUpgradeContentTargets[i]->ContentId(), 
                iUpgradeContentTargets[i]->ContentVersion() ) 
             == ENcdApplicationOlderVersionInstalled )
            {
            DLTRACEOUT(("All upgrades have not been installed yet"));
            return EFalse;
            }
        }

    // Check if the upgrades have already been installed
    for ( TInt i = 0; i < iUpgradeNodeTargets.Count(); ++i ) 
        {
        if ( CNcdProviderUtils::IsApplicationInstalledL( 
                iUpgradeNodeTargets[i]->Uid(), 
                iUpgradeNodeTargets[i]->Version() ) 
             == ENcdApplicationOlderVersionInstalled )
            {
            DLTRACEOUT(("All upgrade node contents have not been installed yet"));
            return EFalse;
            }
        }
    
    DLTRACEOUT(("All upgrades have been installed"));
    return ETrue;
    }


void CNcdNodeUpgrade::SetContentUpgradesL( 
    TBool aContentUpgrades, 
    const TUid& aUid,
    const TDesC& aVersion )
    {
    DLTRACEIN(("aContentUpgrades = %d", aContentUpgrades ));
    iContentUpgrades = aContentUpgrades;        
    
    iUpgradeData->SetAttributeL( EUpgradeDataVersion, aVersion );
    iUpgradeData->SetAttributeL( EUpgradeDataUid, aUid.iUid );
    
    }

TBool CNcdNodeUpgrade::ContentUpgrades() const
    {
    DLTRACEIN(("ContentUpgrades: %d", iContentUpgrades));
    return iContentUpgrades;
    }


// Internalization from and externalization to the database
    
void CNcdNodeUpgrade::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // The upgrade may be provided even if arrays are empty if
    // the content upgrade flag is set.
    if ( iUpgradeNodeTargets.Count() == 0
         && iUpgradeContentTargets.Count() == 0
         && !ContentUpgrades() )
        {
        DLERROR(("No content"));
        DASSERT( EFalse );
        User::Leave( KErrNotFound );
        }

    // First insert data that the creator of this class object will use to
    // create this class object. Class id informs what class object
    // will be created.
    
    aStream.WriteInt32L( iClassId );
    
    ExternalizeNodeUpgradeArrayL( aStream );
    ExternalizeContentUpgradeArrayL( aStream );
    aStream.WriteInt32L( iContentUpgrades );
    }
    
void CNcdNodeUpgrade::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Read the class id first because it is set to the stream in internalize
    // function and it is not read from the stream anywhere else.
    TInt classId( aStream.ReadInt32L() );
    if ( classId != ClassId() )
        {
        DLTRACE(("Wrong class id"));
        DASSERT( EFalse );
        // Leave because the stream does not match this class object
        User::Leave( KErrCorrupt );
        }

    InternalizeNodeUpgradeArrayL( aStream ); 
    InternalizeContentUpgradeArrayL( aStream );

    iContentUpgrades = aStream.ReadInt32L();
    DLTRACEOUT((""));
    }


void CNcdNodeUpgrade::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                      TInt aFunctionNumber )
    {
    DLTRACEIN((""));    

    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
    
    TInt trapError( KErrNone );
    
    // Check which function is called by the proxy side object.
    // Function number are located in ncdnodefunctinoids.h file.
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            // Internalize the proxy side according to the data
            // of this object.
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            // The proxy does not want to use this object anymore.
            // So, release the handle from the session.
            ReleaseRequest( *aMessage );
            break;
                    
        default:
            DLERROR(("Unidentified function request"));
            DASSERT( EFalse );
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong, the complete has not been
        // yet called for the message.
        // So, inform the client about the error if the
        // message is still available.
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;        
    
    DLTRACEOUT((""));
    }

void CNcdNodeUpgrade::CounterPartLost( const MCatalogsSession& aSession )
    {
    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }
    }
                

void CNcdNodeUpgrade::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );


    // Include all the necessary node data to the stream
    ExternalizeDataForRequestL( stream );     
    
    
    // Commits data to the stream when closing.
    CleanupStack::PopAndDestroy( &stream );


    // If this leaves, ReceiveMessage will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure that ExternalizeDataForRequestL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );        
        
    
    DLTRACE(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }
    

void CNcdNodeUpgrade::ExternalizeDataForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // Also, check if the interface should be shown even if there is nothing to be
    // installed.
    if ( AllUpgradesInstalledL()
         && !ContentUpgrades() )
        {
        DLINFO(("No installable node targets. Leave with KErrNotFound."));
        User::Leave( KErrNotFound );        
        }

    if ( IsObsolete() )
        {
        DLINFO(("Set as obsolete. This means that server has removed the object."));
        User::Leave( KNcdErrorObsolete );
        }
    
    // First insert data that the creator of this class object will use to
    // create this class object. Class id informs what class object
    // will be created.
    
    aStream.WriteInt32L( iClassId );
        
    ExternalizeNodeUpgradeArrayL( aStream );

    // Also externalize some information about the possible content dependencies for the
    // proxy
    ExternalizeContentUpgradeArrayForRequestL( aStream );
    
    DASSERT( iUpgradeData );
    if ( ContentUpgrades() )
        {
        aStream.WriteInt8L( 1 );
        iUpgradeData->ExternalizeL( aStream );                        
        }
    else
        {
        aStream.WriteInt8L( 0 );
        }
    
    DLTRACEOUT((""));
    }
    

void CNcdNodeUpgrade::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );

    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( KErrNone );
        
    // Remove this object from the session.
    requestSession.RemoveObject( handle );
        
    DLTRACEOUT((""));
    }


void CNcdNodeUpgrade::ExternalizeNodeUpgradeArrayL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    aStream.WriteInt32L( iUpgradeNodeTargets.Count() );
    for ( TInt i = 0; i < iUpgradeNodeTargets.Count(); ++i )
        {
        iUpgradeNodeTargets[ i ]->ExternalizeL( aStream );
        }

    DLTRACEOUT((""));
    }

void CNcdNodeUpgrade::InternalizeNodeUpgradeArrayL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    CNcdDependencyInfo* upgradeInfo( NULL );
    TInt count( aStream.ReadInt32L() );

    iUpgradeNodeTargets.ResetAndDestroy();
    for( TInt i = 0; i < count; ++i )
        {
        upgradeInfo = CNcdDependencyInfo::NewLC( aStream );
        iUpgradeNodeTargets.AppendL( upgradeInfo );
        CleanupStack::Pop( upgradeInfo );
        }

    DLTRACEOUT((""));
    }


void CNcdNodeUpgrade::ExternalizeContentUpgradeArrayForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    aStream.WriteInt32L( iUpgradeContentTargets.Count() );
    CNcdDownloadInfo* downloadInfo( NULL );
    CNcdDependencyInfo* upgradeInfo( NULL );
    // Notice that here we will give dependency info objects to the proxy side.
    for ( TInt i = 0; i < iUpgradeContentTargets.Count(); ++i )
        {
        downloadInfo = iUpgradeContentTargets[ i ];
        upgradeInfo = CNcdDependencyInfo::NewLC( downloadInfo->ContentName(), 
                                                 downloadInfo->ContentVersion(), 
                                                 downloadInfo->ContentId(),
                                                 NULL );
        upgradeInfo->ExternalizeL( aStream );
        CleanupStack::PopAndDestroy( upgradeInfo );
        upgradeInfo = NULL;
        }

    DLTRACEOUT((""));
    }

void CNcdNodeUpgrade::ExternalizeContentUpgradeArrayL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    aStream.WriteInt32L( iUpgradeContentTargets.Count() );
    for ( TInt i = 0; i < iUpgradeContentTargets.Count(); ++i )
        {
        iUpgradeContentTargets[ i ]->ExternalizeL( aStream );
        }

    DLTRACEOUT((""));
    }

void CNcdNodeUpgrade::InternalizeContentUpgradeArrayL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    CNcdDownloadInfo* content( NULL );
    TInt count( aStream.ReadInt32L() );

    iUpgradeContentTargets.ResetAndDestroy();
    for( TInt i = 0; i < count; ++i )
        {
        content = CNcdDownloadInfo::NewLC();
        content->InternalizeL( aStream );
        iUpgradeContentTargets.AppendL( content );
        CleanupStack::Pop( content );
        }

    DLTRACEOUT((""));
    }


