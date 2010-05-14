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
* Description:   Implements CNcdNodeDependency class
*
*/


#include "ncdnodedependencyimpl.h"

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
#include "ncdprotocolwords.h"
#include "ncdpurchasehistoryutils.h"

#include "catalogsdebug.h"

CNcdNodeDependency::CNcdNodeDependency( CNcdNodeMetaData& aParentMetaData, 
                                        NcdNodeClassIds::TNcdNodeClassId aClassId )
: CNcdCommunicable(),
  iParentMetaData( aParentMetaData ),
  iClassId( aClassId )
    {
    }

void CNcdNodeDependency::ConstructL()
    {
    // These values have to be set.
    }


CNcdNodeDependency* CNcdNodeDependency::NewL( CNcdNodeMetaData& aParentMetaData )
    {
    CNcdNodeDependency* self =   
        CNcdNodeDependency::NewLC( aParentMetaData );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeDependency* CNcdNodeDependency::NewLC( CNcdNodeMetaData& aParentMetaData )
    {
    CNcdNodeDependency* self = 
        new( ELeave ) CNcdNodeDependency( 
            aParentMetaData,
            NcdNodeClassIds::ENcdNodeDependencyClassId );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeDependency::~CNcdNodeDependency()
    {
    DLTRACEIN((""));

    // Delete member variables here
    iDependencyNodeTargets.ResetAndDestroy();
    iDependencyContentTargets.ResetAndDestroy();

    iOldDependencyContentTargets.ResetAndDestroy();
    // Do not delete parent metadata because it is not owned here.

    DLTRACEOUT((""));
    }        


NcdNodeClassIds::TNcdNodeClassId CNcdNodeDependency::ClassId() const
    {
    return iClassId;
    }


const RPointerArray<CNcdDownloadInfo>& CNcdNodeDependency::ContentTargets() const
    {
    return iDependencyContentTargets;
    }


const RPointerArray<CNcdDependencyInfo>& CNcdNodeDependency::NodeTargets() const
    {
    return iDependencyNodeTargets;
    }


// Internalization from the protocol

void CNcdNodeDependency::InternalizeL( MNcdPreminetProtocolDataEntity& aData )
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
    iDependencyNodeTargets.ResetAndDestroy();
    iDependencyContentTargets.ResetAndDestroy();

    const MNcdPreminetProtocolEntityDependency* dependency( NULL );
    CNcdNodeIdentifier* identifier( NULL );
    CNcdDependencyInfo* dependencyInfo(NULL ); 
  
    DLTRACE(("Going through %d entity dependencies", 
        downloadableContent->EntityDependencyCount() ));
        
    for ( TInt i = 0; i < downloadableContent->EntityDependencyCount(); ++i )
        {
        // Check what nodes depend on this node.
        dependency = &downloadableContent->EntityDependencyL( i );
        if ( dependency->Type() == EDependency )
            {
            MNcdPurchaseDownloadInfo::TContentUsage usage = 
                DetermineContentUsage( dependency->Name() );
                
            // Check if the dependency should be loaded as a node when
            // the download responsibility is transferred to the proxy side
            // or should we just download the content directly in server side.
            if ( dependency->EntityId() != KNullDesC &&
                 // If the name of the dependency contains any of the launcher keywords,
                 // we handle it like a SIS-dependency even if it is really a entity
                 // dependency.
                 usage == MNcdPurchaseDownloadInfo::EDependency )
                {
                DLINFO((_L("Dependency entity id: %S"), &dependency->EntityId()));
                // The dependency information contains  the
                // entity id of the target node.
                identifier = 
                    CNcdNodeIdentifier::NewL( iParentMetaData.Identifier().NodeNameSpace(),
                                              dependency->EntityId(),                                           
                                              iParentMetaData.Identifier().ServerUri(),
                                              iParentMetaData.Identifier().ClientUid() );
                // Note, that identifier ownership is transferred here.
                dependencyInfo = 
                    CNcdDependencyInfo::NewLC( dependency->Name(),
                                               dependency->ContentVersion(),
                                               dependency->ContentId(),
                                               identifier );
                iDependencyNodeTargets.AppendL( dependencyInfo );
                CleanupStack::Pop( dependencyInfo );                
                }
            else
                {
                // Ownership is not transferred here.
                const MNcdPreminetProtocolDownload* downloadDetails( 
                    dependency->DownloadDetails() );
                if ( downloadDetails != NULL || 
                     // Launchers don't need to have download details
                     usage != MNcdPurchaseDownloadInfo::EDependency )
                    {
                    DLINFO(("Download the content directly. New node will not be created."));
                    // Copy the content object and insert it into the content array.
                    // The purchase operation may use the array content when checking
                    // if dependencies should be downloaded and installed.
                    CNcdDownloadInfo* content( 
                        CNcdDownloadInfo::NewLC( *dependency ) );
                    
                    TBool isLauncher = SetContentUsage( *dependency, *content );
                    // Dependencies are not launchable but launchers are
                    content->SetLaunchable( isLauncher );                    

                    iDependencyContentTargets.AppendL( content );
                        
                    // Array took ownership. So, do not delete.
                    CleanupStack::Pop( content );                    
                    DLTRACE(("Download info added"));
                    }
                }
            }
        }


    if ( iDependencyContentTargets.Count() == 0 ) 
        {
        // No dependency content targets so the current item on the server
        // doesn't have any dependencies so we don't show any 
        // through the API
        iOldDependencyContentTargets.ResetAndDestroy();
        }
        
    if ( iDependencyNodeTargets.Count() == 0
         && iDependencyContentTargets.Count() == 0 )
        {
        // No Dependency data was found. 
        // So, inform about that to the owner of this class object.
        DLINFO(("No content"));
        User::Leave( KErrNotFound );
        }
    
    DLTRACEOUT((""));
    }


TBool CNcdNodeDependency::InternalizeFromPurchaseDetailsL( 
    const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));
    
    iOldDependencyContentTargets.ResetAndDestroy();
    
    TArray<MNcdPurchaseDownloadInfo*> dlInfo ( aDetails.DownloadInfoL() );
    
    if ( !dlInfo.Count() ) 
        {
        DLTRACEOUT(("No download infos so no dependencies"));
        return EFalse;
        }
    
    TArray<MNcdPurchaseInstallInfo*> installInfo ( aDetails.InstallInfoL() );
            
    
    for ( TInt i = 0; i < dlInfo.Count(); ++i )
        {
        // Launchers are dependencies        
        if ( NcdPurchaseHistoryUtils::IsDependency( *dlInfo[i] ) )
            {
            DLTRACE(("Internalizing dependency from purchase history"));
            DASSERT( installInfo.Count() > i );
            CNcdDownloadInfo* info = CNcdDownloadInfo::NewLC( 
                *dlInfo[i],
                installInfo[i]->ApplicationUid(),
                installInfo[i]->ApplicationVersion() );
            iOldDependencyContentTargets.AppendL( info );
            CleanupStack::Pop( info );  
            }            
        else 
            {            
            // Dependencies are before other content so we
            // can stop here
            break;
            }
        }
    
    if ( iOldDependencyContentTargets.Count() ) 
        {
        DLTRACEOUT(("Internalization successful"));
        return ETrue;
        }
    
    DLTRACEOUT(("Nothing to internalize"));
    return EFalse;
    }


TBool CNcdNodeDependency::UpdateDependencyStatesL()
    {
    DLTRACEIN((""));
    MoveOldContentTargetsToNew();
    
    TBool allInstalled = ETrue;
    
    // Check if the dependencies have already been installed
    for ( TInt i = 0; i < iDependencyContentTargets.Count(); ++i ) 
        {        
        // allInstalled becomes false if any of the dependencies is
        // not installed
        allInstalled = allInstalled && 
            UpdateDependencyStateL( *iDependencyContentTargets[i] );
        }
        
        
    // Check if the dependencies have already been installed
    for ( TInt i = 0; i < iDependencyNodeTargets.Count(); ++i ) 
        {
        // EApplicationOlderVersionInstalled & EApplicationNotInstalled
        // are considered as missing
        if ( CNcdProviderUtils::IsApplicationInstalledL( 
            iDependencyNodeTargets[i]->Uid(), 
            iDependencyNodeTargets[i]->Version() ) <= 
                ENcdApplicationNotInstalled  )
            {
            DLTRACE(("Dependency not installed, updating state"));
            iDependencyNodeTargets[i]->SetDependencyState( 
                ENcdDependencyMissing );
            allInstalled = EFalse;
            }
        else
            {
            iDependencyNodeTargets[i]->SetDependencyState( 
                ENcdDependencyInstalled );
            
            }
        }
        
    DLTRACEOUT(("allInstalled: %d", allInstalled));
    return allInstalled;
    }


void CNcdNodeDependency::UpdateDependenciesL( 
    CNcdPurchaseDetails& aDetails ) const
    {
    DLTRACEIN((""));
    NcdPurchaseHistoryUtils::UpdateDependenciesL( 
        aDetails, 
        iDependencyContentTargets );
    }


// Internalization from and externalization to the database
    
void CNcdNodeDependency::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    if ( iDependencyNodeTargets.Count() == 0
         && iDependencyContentTargets.Count() == 0 )
        {
        DLERROR(("No content"));
        DASSERT( EFalse );
        User::Leave( KErrNotFound );
        }

    // First insert data that the creator of this class object will use to
    // create this class object. Class id informs what class object
    // will be created.
    
    aStream.WriteInt32L( iClassId );
    
    ExternalizeNodeDependencyArrayL( aStream );
    ExternalizeContentDependencyArrayL( aStream );
    
    DLTRACEOUT((""));
    }
    
void CNcdNodeDependency::InternalizeL( RReadStream& aStream )
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

    InternalizeNodeDependencyArrayL( aStream ); 
    InternalizeContentDependencyArrayL( aStream );
        
    DLTRACEOUT((""));
    }


void CNcdNodeDependency::ReceiveMessage( MCatalogsBaseMessage* aMessage,
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

void CNcdNodeDependency::CounterPartLost( const MCatalogsSession& aSession )
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
                

void CNcdNodeDependency::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
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
    

void CNcdNodeDependency::ExternalizeDataForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    
    if ( IsObsolete() )
        {
        DLINFO(("Set as obsolete. This means that server has removed the object."));
        User::Leave( KNcdErrorObsolete );
        }
    
    UpdateDependencyStatesL();
    
    // First insert data that the creator of this class object will use to
    // create this class object. Class id informs what class object
    // will be created.
    
    aStream.WriteInt32L( iClassId );
    
    ExternalizeNodeDependencyArrayL( aStream );

    // Also externalize some information about the possible content dependencies for the
    // proxy
    ExternalizeContentDependencyArrayForRequestL( aStream );
                
    DLTRACEOUT((""));
    }

void CNcdNodeDependency::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
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


void CNcdNodeDependency::ExternalizeNodeDependencyArrayL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    aStream.WriteInt32L( iDependencyNodeTargets.Count() );
    for ( TInt i = 0; i < iDependencyNodeTargets.Count(); ++i )
        {
        iDependencyNodeTargets[ i ]->ExternalizeL( aStream );
        }

    DLTRACEOUT((""));
    }

void CNcdNodeDependency::InternalizeNodeDependencyArrayL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    CNcdDependencyInfo* dependencyInfo( NULL );
    TInt count( aStream.ReadInt32L() );

    iDependencyNodeTargets.ResetAndDestroy();
    for( TInt i = 0; i < count; ++i )
        {
        dependencyInfo = CNcdDependencyInfo::NewLC( aStream );
        iDependencyNodeTargets.AppendL( dependencyInfo );
        CleanupStack::Pop( dependencyInfo );
        }

    DLTRACEOUT((""));
    }


void CNcdNodeDependency::ExternalizeContentDependencyArrayForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    aStream.WriteInt32L( iDependencyContentTargets.Count() );
    CNcdDownloadInfo* downloadInfo( NULL );
    CNcdDependencyInfo* dependencyInfo( NULL );
    // Notice that here we will give dependency info objects to the proxy side.
    for ( TInt i = 0; i < iDependencyContentTargets.Count(); ++i )
        {
        downloadInfo = iDependencyContentTargets[ i ];
        dependencyInfo = CNcdDependencyInfo::NewLC( downloadInfo->ContentName(), 
                                                    downloadInfo->ContentVersion(), 
                                                    downloadInfo->ContentId(),
                                                    NULL );
        dependencyInfo->SetDependencyState( downloadInfo->DependencyState() );                                                   
        dependencyInfo->ExternalizeL( aStream );
        CleanupStack::PopAndDestroy( dependencyInfo );
        dependencyInfo = NULL;
        }

    DLTRACEOUT((""));
    }


void CNcdNodeDependency::ExternalizeContentDependencyArrayL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    aStream.WriteInt32L( iDependencyContentTargets.Count() );
    for ( TInt i = 0; i < iDependencyContentTargets.Count(); ++i )
        {
        iDependencyContentTargets[ i ]->ExternalizeL( aStream );
        }

    DLTRACEOUT((""));
    }

void CNcdNodeDependency::InternalizeContentDependencyArrayL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    CNcdDownloadInfo* content( NULL );
    TInt count( aStream.ReadInt32L() );

    iDependencyContentTargets.ResetAndDestroy();
    for( TInt i = 0; i < count; ++i )
        {
        content = CNcdDownloadInfo::NewLC();
        content->InternalizeL( aStream );
        iDependencyContentTargets.AppendL( content );
        CleanupStack::Pop( content );
        }
    
    DLTRACEOUT((""));
    }


// This moves dependencies read from the purchase history as the 
// current dependencies if there are no current dependencies.
// This is used to ensure that MNcdNodeDependency show dependencies
// correctly even if the node cache has been cleared but the
// item has been bought
void CNcdNodeDependency::MoveOldContentTargetsToNew()
    {
    DLTRACEIN((""));
    
    if ( iDependencyContentTargets.Count() == 0 )
        {
        DLTRACE(("Moving dependencies"));
        iDependencyContentTargets.Close();
        iDependencyContentTargets = iOldDependencyContentTargets;
        iOldDependencyContentTargets = RPointerArray<CNcdDownloadInfo>();
        }
    }


TBool CNcdNodeDependency::UpdateDependencyStateL( 
    CNcdDownloadInfo& aContentTarget )
    {
    DLTRACEIN((""));
    // Compare current dependencies to those read from purchase history
    // If old dependencies exist:
    // - they determine whether dependency is installed or not
    // - their version is compared to the one received in the protocol
    //   if protocol contains newer, then that is used
    TInt oldCount = iOldDependencyContentTargets.Count();
    for ( TInt i = 0; i < oldCount; ++i ) 
        {
        if ( aContentTarget.ContentId() 
             == iOldDependencyContentTargets[i]->ContentId() ) 
            {
            TInt comp = 0;
            TRAPD( err, comp = CNcdProviderUtils::CompareVersionsL(                     
                iOldDependencyContentTargets[i]->ContentVersion(),
                aContentTarget.ContentVersion() ) );
            LeaveIfNotErrorL( err, KErrArgument, KErrGeneral );
            
            TNcdApplicationStatus status = 
                CNcdProviderUtils::IsApplicationInstalledL( 
                    iOldDependencyContentTargets[i]->ContentId(),
                    iOldDependencyContentTargets[i]->ContentVersion() );
            
            
            TBool installed = SetDependencyState( 
                aContentTarget, status, comp, EFalse );
        
            DLTRACEOUT(("Installed: %d", installed));
            return installed;
            }
        }
    
        
    // This is executed only if the dependency is not found from the
    // old dependencies which means that either there were no old dependencies
    // old dependencies have been updated on the server
    TNcdApplicationStatus status = 
        CNcdProviderUtils::IsApplicationInstalledL( 
        aContentTarget.ContentId(),
        aContentTarget.ContentVersion() );
 
    // Give 0 as version comp status so that state is set either
    // installed or missing, but if oldCount != 0 then the state will be
    // either installed or upgrade available
    TBool installed = SetDependencyState( 
        aContentTarget, status, 0, oldCount );
        
    DLTRACEOUT(("Installed: %d", installed));
    return installed;
    }



TBool CNcdNodeDependency::SetDependencyState( 
    CNcdDownloadInfo& aContentTarget,
    TNcdApplicationStatus aStatus,
    TInt aVersionCompResult,
    TBool aIsUpgrade )
    {
    DLTRACEIN((""));
    // Dependency is installed if the correct or newer version is installed
    TBool installed = 
        ( aStatus == ENcdApplicationInstalled ) || 
        ( aStatus == ENcdApplicationNewerVersionInstalled );
    
    // Update dependency state according to installation status and
    // version numbers
    if ( installed ) 
        {                    
        if ( aVersionCompResult >= 0 )
            {
            DLTRACE(("Same or newer installed"));
            aContentTarget.SetDependencyState( 
                ENcdDependencyInstalled );
            }
        else 
            {
            DLTRACE(("Older version installed"));
            aContentTarget.SetDependencyState( 
                ENcdDependencyUpgradeAvailable );                        
            }
        }
    // if old dependencies are installed, new uninstalled are considered to
    // be upgrades
    else if ( aIsUpgrade ) 
        {
        DLTRACE(("Dependency not installed but it's considered as an upgrade"));
        aContentTarget.SetDependencyState( 
            ENcdDependencyUpgradeAvailable );  
        }
    else
        {
        DLTRACE(("Dependency not installed"));
        aContentTarget.SetDependencyState( 
            ENcdDependencyMissing );  
        
        }
    return installed;
    }


TBool CNcdNodeDependency::SetContentUsage( 
    const MNcdPreminetProtocolEntityDependency& aDependency, 
    CNcdDownloadInfo& aTarget ) const
    {
    DLTRACEIN((""));
    MNcdPurchaseDownloadInfo::TContentUsage usage = 
        DetermineContentUsage( aDependency.Name() );
            
    aTarget.SetContentUsage( usage );
    
    // Must return ETrue if the dependency is a launcher application
    return usage != MNcdPurchaseDownloadInfo::EDependency;
    }


MNcdPurchaseDownloadInfo::TContentUsage CNcdNodeDependency::DetermineContentUsage(
    const TDesC& aDependencyName ) const
    {
    MNcdPurchaseDownloadInfo::TContentUsage usage = 
        MNcdPurchaseDownloadInfo::EDependency;
        
    if ( aDependencyName.MatchF( KNcdLauncher ) != KErrNotFound ) 
        {
        DLTRACE(("Launcher"));
        usage = MNcdPurchaseDownloadInfo::ELauncher;
        }
    else if ( aDependencyName.MatchF( KNcdLauncherOpen ) != KErrNotFound )
        {
        DLTRACE(("Launcher/open"));
        usage = MNcdPurchaseDownloadInfo::ELauncherOpen;
        }
    return usage;    
    }
